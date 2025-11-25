// src/widgets/AlgorithmSeriesConvergenceTable.tsx

import React, { useEffect, useState, useRef, useMemo } from "react";
import type {
    Experiment,
    Series,
    Accel,
    SeriesAccel,
    Complex,
    SeriesArgs,
    AccelArgs,
} from "@/entities/experiment/model/experiment";

type SideType = "one_sided" | "two_sided" | "unknown" | "no_limit";

type MonotonicityType =
    | "strict_decreasing_error"
    | "non_increasing_error"
    | "constant_error"
    | "has_growth"
    | "not_enough_data"
    | "no_limit";

interface ConvergenceAnalysis {
    seriesId: string;
    accelId: string;

    side: SideType;
    monotonicity: MonotonicityType;

    signChangesCount: number;
    firstSignChangeN: number | null;
    firstGrowthN: number | null;

    stepsAnalyzed: number;
}

type SeriesKey = string;
type AlgoKey = string;

interface SeriesInfo {
    key: SeriesKey;
    seriesName: string;
    xLabel: string;
    xSort: number | null;
    precision: string;
}

interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: AccelArgs | null;
}

interface ConvergenceMatrix {
    seriesList: SeriesInfo[];
    algoList: AlgoInfo[];
    cells: Record<string, ConvergenceAnalysis>;
}

interface ProgressState {
    running: boolean;
    current: number;
    total: number;
}

const EPS = 1e-15;
const CHUNK_SIZE = 24;

/* ================== Вспомогательные функции ================== */

function hasFiniteNumber(v: number | null | undefined): v is number {
    return v != null && Number.isFinite(v);
}

function errorNorm(
    value: { re: number | null; im: number | null } | null,
    limit: Complex | null
): number | null {
    if (!value || !limit) return null;

    const reV = value.re;
    const imV = value.im ?? 0;
    const reL = limit.re;
    const imL = limit.im ?? 0;

    if (!hasFiniteNumber(reV) || !hasFiniteNumber(reL)) return null;
    if (!hasFiniteNumber(imV) || !hasFiniteNumber(imL)) return null;

    const dr = reV - reL;
    const di = imV - imL;
    const e = Math.hypot(dr, di);

    return Number.isFinite(e) ? e : null;
}

function realDiffSign(
    value: { re: number | null; im: number | null } | null,
    limit: Complex | null
): -1 | 0 | 1 | null {
    if (!value || !limit) return null;
    const reV = value.re;
    const reL = limit.re;

    if (!hasFiniteNumber(reV) || !hasFiniteNumber(reL)) return null;

    const d = reV - reL;

    if (d > EPS) return 1;
    if (d < -EPS) return -1;
    return 0;
}

function getPointsSortedByN(
    sa: SeriesAccel
): { n: number; value: { re: number | null; im: number | null } | null }[] {
    const raw = sa.computed ?? [];
    const len = raw.length;

    if (len <= 1) {
        return raw;
    }

    let sorted = true;
    for (let i = 1; i < len; i++) {
        if (raw[i - 1].n > raw[i].n) {
            sorted = false;
            break;
        }
    }

    if (sorted) return raw;
    return [...raw].sort((a, b) => a.n - b.n);
}

/* ---- утилиты по аргументам / x, как в AlgorithmSeriesDiffHeatmap ---- */

type Scalar = number | string | boolean | null;

function toSortableNumber(v: Scalar): number | null {
    if (typeof v === "number") {
        return Number.isFinite(v) ? v : null;
    }
    if (typeof v === "string") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    return null;
}

function parseX(args: SeriesArgs | null): { xLabel: string; xSort: number | null } {
    const raw = args?.x as Scalar | undefined;
    if (raw == null) {
        return { xLabel: "∅", xSort: null };
    }
    return {
        xLabel: String(raw),
        xSort: toSortableNumber(raw),
    };
}

function nonNullEntries<T extends Record<string, unknown>>(obj: T | null | undefined) {
    if (!obj) return [] as [string, unknown][];
    return Object.entries(obj).filter(([, v]) => v !== null && v !== undefined);
}

function buildArgsSummary(args: AccelArgs | null): string {
    const entries = nonNullEntries(args);
    if (entries.length === 0) return "";
    entries.sort(([a], [b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

/* ================== Анализ одной пары ================== */

function analyzeSeriesAccelConvergence(
    series: Series | undefined,
    _accel: Accel | undefined,
    sa: SeriesAccel
): ConvergenceAnalysis {
    const limit = series?.limit ?? null;

    if (!limit || !hasFiniteNumber(limit.re ?? null)) {
        return {
            seriesId: sa.series_id,
            accelId: sa.accel_id,
            side: "no_limit",
            monotonicity: "no_limit",
            signChangesCount: 0,
            firstSignChangeN: null,
            firstGrowthN: null,
            stepsAnalyzed: 0,
        };
    }

    const points = getPointsSortedByN(sa);

    let prevErr: number | null = null;
    let prevSign: -1 | 0 | 1 | null = null;

    let signChangesCount = 0;
    let firstSignChangeN: number | null = null;

    let hasGrowth = false;
    let firstGrowthN: number | null = null;

    let hasStrictDecrease = false;
    let hasEqual = false;

    let stepsAnalyzed = 0;

    for (const p of points) {
        const value = p.value;
        const err = errorNorm(value, limit);
        const sgn = realDiffSign(value, limit);

        if (sgn !== null && sgn !== 0) {
            if (prevSign === null) {
                prevSign = sgn;
            } else if (prevSign !== 0 && prevSign !== sgn) {
                signChangesCount += 1;
                if (firstSignChangeN === null) {
                    firstSignChangeN = p.n;
                }
                prevSign = sgn;
            }
        }

        if (err !== null) {
            if (prevErr !== null) {
                stepsAnalyzed += 1;
                if (err > prevErr + EPS) {
                    if (!hasGrowth) {
                        hasGrowth = true;
                        firstGrowthN = p.n;
                    }
                } else if (err < prevErr - EPS) {
                    hasStrictDecrease = true;
                } else {
                    hasEqual = true;
                }
            }
            prevErr = err;
        }
    }

    let side: SideType;
    if (prevSign === null) {
        side = "unknown";
    } else if (signChangesCount === 0) {
        side = "one_sided";
    } else {
        side = "two_sided";
    }

    let monotonicity: MonotonicityType;

    if (stepsAnalyzed === 0) {
        monotonicity = "not_enough_data";
    } else if (hasGrowth) {
        monotonicity = "has_growth";
    } else if (hasStrictDecrease && !hasEqual) {
        monotonicity = "strict_decreasing_error";
    } else if (!hasStrictDecrease && hasEqual) {
        monotonicity = "constant_error";
    } else if (hasStrictDecrease && hasEqual) {
        monotonicity = "non_increasing_error";
    } else {
        monotonicity = "not_enough_data";
    }

    return {
        seriesId: sa.series_id,
        accelId: sa.accel_id,
        side,
        monotonicity,
        signChangesCount,
        firstSignChangeN,
        firstGrowthN,
        stepsAnalyzed,
    };
}

/* ================== Инкрементальный расчёт ================== */

function useConvergenceMatrix(
    experiment: Experiment | null,
    chunkSize: number = CHUNK_SIZE
): { matrix: ConvergenceMatrix | null; progress: ProgressState } {
    const [matrix, setMatrix] = useState<ConvergenceMatrix | null>(null);
    const [progress, setProgress] = useState<ProgressState>({
        running: false,
        current: 0,
        total: 0,
    });

    useEffect(() => {
        let cancelled = false;

        if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
            setMatrix(null);
            setProgress({ running: false, current: 0, total: 0 });
            return;
        }

        const seriesListRaw = experiment.seriesList ?? [];
        const accelListRaw = experiment.accelList ?? [];
        const seriesAccelList = experiment.seriesAccelList;

        const seriesMap = new Map<string, Series>();
        for (const s of seriesListRaw) {
            seriesMap.set(s.id, s);
        }

        const accelMap = new Map<string, Accel>();
        for (const a of accelListRaw) {
            accelMap.set(a.id, a);
        }

        const usedSeriesIds = new Set<string>();
        const usedAccelIds = new Set<string>();

        const cells: Record<string, ConvergenceAnalysis> = {};

        const total = seriesAccelList.length;
        setProgress({ running: true, current: 0, total });
        setMatrix(null);

        function processChunk(startIndex: number) {
            if (cancelled) return;

            const endIndex = Math.min(startIndex + chunkSize, total);

            for (let i = startIndex; i < endIndex; i++) {
                const sa = seriesAccelList[i];

                usedSeriesIds.add(sa.series_id);
                usedAccelIds.add(sa.accel_id);

                const series = seriesMap.get(sa.series_id);
                const accel = accelMap.get(sa.accel_id);

                const analysis = analyzeSeriesAccelConvergence(series, accel, sa);
                const key = `${sa.accel_id}::${sa.series_id}`;
                cells[key] = analysis;
            }

            const current = endIndex;
            const running = endIndex < total;
            setProgress({ running, current, total });

            if (running) {
                setTimeout(() => processChunk(endIndex), 0);
            } else {
                const seriesInfos: SeriesInfo[] = seriesListRaw
                    .filter((s) => usedSeriesIds.has(s.id))
                    .map((s) => {
                        const { xLabel, xSort } = parseX(s.args ?? null);
                        return {
                            key: s.id,
                            seriesName: s.name,
                            xLabel,
                            xSort,
                            precision: s.precision,
                        };
                    })
                    .sort((a, b) => {
                        const byName = a.seriesName.localeCompare(b.seriesName);
                        if (byName !== 0) return byName;

                        const ax = a.xSort;
                        const bx = b.xSort;
                        if (ax != null && bx != null) return ax - bx;
                        if (ax != null) return -1;
                        if (bx != null) return 1;
                        return a.xLabel.localeCompare(b.xLabel);
                    });

                const algoInfos: AlgoInfo[] = accelListRaw
                    .filter((a) => usedAccelIds.has(a.id))
                    .map((a) => {
                        const args = a.args ?? null;
                        return {
                            key: a.id,
                            algorithmName: a.name,
                            m: a.m,
                            argsSummary: buildArgsSummary(args),
                            algorithmArgs: args,
                        };
                    })
                    .sort((a, b) => {
                        const byName = a.algorithmName.localeCompare(b.algorithmName);
                        if (byName !== 0) return byName;
                        return (a.m ?? 0) - (b.m ?? 0);
                    });

                if (!cancelled) {
                    setMatrix({
                        seriesList: seriesInfos,
                        algoList: algoInfos,
                        cells,
                    });
                }
            }
        }

        processChunk(0);

        return () => {
            cancelled = true;
        };
    }, [experiment, chunkSize]);

    return { matrix, progress };
}

/* ================== UI-утилиты ================== */

function formatSideShort(side: SideType): string {
    switch (side) {
        case "one_sided":
            return "1ст";
        case "two_sided":
            return "2ст";
        case "unknown":
            return "?";
        case "no_limit":
            return "∅";
    }
}

function formatMonotonicityShort(mon: MonotonicityType): string {
    switch (mon) {
        case "strict_decreasing_error":
            return "↓";
        case "non_increasing_error":
            return "⇘";
        case "constant_error":
            return "=";
        case "has_growth":
            return "↗";
        case "not_enough_data":
        case "no_limit":
            return "?";
    }
}

function formatSideDescription(side: SideType): string {
    switch (side) {
        case "one_sided":
            return "Одностороннее приближение к пределу (знак A_k - lim не меняется).";
        case "two_sided":
            return "Двустороннее приближение к пределу (знак A_k - lim меняется).";
        case "unknown":
            return "Недостаточно данных для определения односторонности.";
        case "no_limit":
            return "Нет значения предела для ряда (series.limit = null).";
    }
}

function formatMonotonicityDescription(mon: MonotonicityType): string {
    switch (mon) {
        case "strict_decreasing_error":
            return "|A_k - lim| строго убывает на всех шагах (монотонно сходится).";
        case "non_increasing_error":
            return "|A_k - lim| не возрастает (есть убывающие и равные шаги).";
        case "constant_error":
            return "|A_k - lim| одинаково на всех шагах.";
        case "has_growth":
            return "На некоторых шагах |A_k - lim| возрастает (есть рост ошибки).";
        case "not_enough_data":
            return "Недостаточно точек для анализа монотонности.";
        case "no_limit":
            return "Нет значения предела, анализ монотонности не выполняется.";
    }
}

function getCellColorClass(side: SideType, mon: MonotonicityType, selected: boolean): string {
    let base =
        "border-border text-textDim transition-colors " +
        (selected ? "ring-2 ring-accent ring-offset-1 ring-offset-surface" : "");

    if (mon === "strict_decreasing_error" || mon === "non_increasing_error") {
        if (side === "one_sided") {
            return base + " bg-emerald-500/25 hover:bg-emerald-500/35";
        }
        if (side === "two_sided") {
            return base + " bg-emerald-400/20 hover:bg-emerald-400/30";
        }
        if (side === "unknown") {
            return base + " bg-emerald-400/10 hover:bg-emerald-400/20";
        }
        if (side === "no_limit") {
            return (
                "border-border/70 text-textDim/70 bg-surface/40 hover:bg-surface/50 " +
                (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
            );
        }
    }

    if (mon === "constant_error") {
        if (side === "one_sided") {
            return base + " bg-surface/70 hover:bg-surface/60";
        }
        if (side === "two_sided") {
            return base + " bg-surface/60 hover:bg-surface/50";
        }
        return (
            "border-border/70 text-textDim/70 bg-surface/50 hover:bg-surface/40 " +
            (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
        );
    }

    if (mon === "has_growth") {
        if (side === "one_sided") {
            return (
                "border-amber-500 text-textDim bg-amber-500/25 hover:bg-amber-500/35 " +
                (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
            );
        }
        if (side === "two_sided") {
            return (
                "border-red-500 text-textDim bg-red-500/30 hover:bg-red-500/40 " +
                (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
            );
        }
        if (side === "unknown") {
            return (
                "border-amber-500/80 text-textDim bg-amber-500/20 hover:bg-amber-500/30 " +
                (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
            );
        }
        if (side === "no_limit") {
            return (
                "border-red-500/70 text-textDim/80 bg-red-500/20 hover:bg-red-500/30 " +
                (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
            );
        }
    }

    if (mon === "not_enough_data" || mon === "no_limit") {
        if (side === "no_limit") {
            return (
                "border-border/60 text-textDim/60 bg-surface/30 hover:bg-surface/40 " +
                (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
            );
        }
        return (
            "border-border/50 text-textDim/60 bg-surface/40 hover:bg-surface/50 " +
            (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
        );
    }

    return (
        "border-border text-textDim bg-surface/40 hover:bg-surface/50 " +
        (selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "")
    );
}

/* ================== Детальный график ================== */

interface DetailPoint {
    n: number;
    valueRe: number | null;
    valueIm: number | null;
    err: number | null;
    sign: -1 | 0 | 1 | null;
}

interface SelectedDetail {
    seriesInfo: SeriesInfo | null;
    algoInfo: AlgoInfo | null;
    series: Series | null;
    accel: Accel | null;
    analysis: ConvergenceAnalysis | null;
    limit: Complex | null;
    points: DetailPoint[];
}

function ConvergenceDetailChart({ detail }: { detail: SelectedDetail }) {
    const { seriesInfo, algoInfo, analysis, limit, points } = detail;

    if (!seriesInfo || !algoInfo || !analysis) {
        return null;
    }

    if (!points.length) {
        return (
            <div className="mt-4 rounded-xl border border-border bg-panel p-3 text-xs text-textDim">
                Для выбранной пары нет вычисленных точек.
            </div>
        );
    }

    const errPoints = points.filter(
        (p) => p.err != null && Number.isFinite(p.err) && (p.err as number) >= 0
    );

    const hasErrorCurve = limit != null && errPoints.length > 0;

    let pathD = "";
    let minN = 0;
    let maxN = 0;
    let minE = 0;
    let maxE = 0;

    if (hasErrorCurve) {
        minN = errPoints[0].n;
        maxN = errPoints[errPoints.length - 1].n;

        minE = errPoints.reduce(
            (acc, p) => Math.min(acc, p.err as number),
            errPoints[0].err as number
        );
        maxE = errPoints.reduce(
            (acc, p) => Math.max(acc, p.err as number),
            errPoints[0].err as number
        );

        if (maxE === minE) {
            maxE = minE + 1;
        }

        const width = 600;
        const height = 220;
        const marginLeft = 40;
        const marginRight = 10;
        const marginTop = 10;
        const marginBottom = 25;

        const innerW = width - marginLeft - marginRight;
        const innerH = height - marginTop - marginBottom;

        const xScale = (n: number) => marginLeft + (innerW * (n - minN)) / (maxN - minN || 1);
        const yScale = (e: number) =>
            marginTop + innerH - (innerH * (e - minE)) / (maxE - minE || 1);

        const parts: string[] = [];
        for (let i = 0; i < errPoints.length; i++) {
            const p = errPoints[i];
            const x = xScale(p.n);
            const y = yScale(p.err as number);
            parts.push(`${i === 0 ? "M" : "L"} ${x.toFixed(2)} ${y.toFixed(2)}`);
        }
        pathD = parts.join(" ");
    }

    const shortSide = formatSideShort(analysis.side);
    const shortMon = formatMonotonicityShort(analysis.monotonicity);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 shadow-panel text-xs text-textDim">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">
                        Детальный график сходимости
                    </div>
                    <div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">
                        <div>
                            Ряд: {seriesInfo.seriesName}, x={seriesInfo.xLabel}, prec=
                            {seriesInfo.precision}
                        </div>
                        <div>
                            Алгоритм: {algoInfo.algorithmName}
                            {algoInfo.m != null ? `, m=${algoInfo.m}` : ""}
                        </div>
                        {algoInfo.argsSummary && <div>Аргументы: {algoInfo.argsSummary}</div>}
                    </div>
                </div>
                <div className="space-y-1 text-[11px] text-textDim/80">
                    <div>
                        Тип: {shortSide} | {shortMon}
                    </div>
                    <div>
                        Смен знака: {analysis.signChangesCount}
                        {analysis.firstSignChangeN != null
                            ? `, первая при n=${analysis.firstSignChangeN}`
                            : ""}
                    </div>
                    <div>
                        Первый рост ошибки:{" "}
                        {analysis.firstGrowthN != null ? `n=${analysis.firstGrowthN}` : "не был"}
                    </div>
                    <div>Сравнено шагов (пар): {analysis.stepsAnalyzed}</div>
                </div>
            </div>

            {hasErrorCurve ? (
                <div className="mb-3">
                    <svg
                        viewBox="0 0 600 220"
                        className="w-full rounded bg-surface/80"
                        preserveAspectRatio="none"
                    >
                        <rect x={0} y={0} width={600} height={220} className="fill-surface/80" />
                        <line
                            x1={40}
                            y1={20}
                            x2={40}
                            y2={195}
                            className="stroke-border"
                            strokeWidth={0.5}
                        />
                        <line
                            x1={40}
                            y1={195}
                            x2={590}
                            y2={195}
                            className="stroke-border"
                            strokeWidth={0.5}
                        />

                        <path d={pathD} className="stroke-primary" strokeWidth={1.2} fill="none" />

                        <text
                            x={320}
                            y={210}
                            className="fill-textDim/70 text-[9px]"
                            textAnchor="middle"
                        >
                            n (номер шага)
                        </text>
                        <text
                            x={15}
                            y={15}
                            className="fill-textDim/70 text-[9px]"
                            textAnchor="start"
                        >
                            |Aₙ − lim|
                        </text>

                        <text
                            x={45}
                            y={207}
                            className="fill-textDim/60 text-[8px]"
                            textAnchor="start"
                        >
                            n={minN}
                        </text>
                        <text
                            x={585}
                            y={207}
                            className="fill-textDim/60 text-[8px]"
                            textAnchor="end"
                        >
                            n={maxN}
                        </text>

                        <text
                            x={45}
                            y={32}
                            className="fill-textDim/60 text-[8px]"
                            textAnchor="start"
                        >
                            err≈{minE.toExponential(2)}
                        </text>
                        <text
                            x={45}
                            y={190}
                            className="fill-textDim/60 text-[8px]"
                            textAnchor="start"
                        >
                            err≈{maxE.toExponential(2)}
                        </text>
                    </svg>
                    <div className="mt-1 text-[10px] text-textDim/70">
                        График |Aₙ − lim| по n. Отображаются только точки, где ошибка корректно
                        определена и конечна.
                    </div>
                </div>
            ) : (
                <div className="mb-3 text-[11px] text-amber-300/80">
                    Невозможно построить график ошибки: нет предела или нет корректных значений |Aₙ
                    − lim|. Ниже приведена таблица чисел.
                </div>
            )}

            <div className="mt-2 max-h-64 overflow-auto rounded border border-border bg-surface/60">
                <table className="min-w-full border-collapse text-[10px]">
                    <thead className="bg-surface/80">
                        <tr>
                            <th className="border-b border-border px-2 py-1 text-left">n</th>
                            <th className="border-b border-border px-2 py-1 text-left">Re(Aₙ)</th>
                            <th className="border-b border-border px-2 py-1 text-left">Im(Aₙ)</th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                |Aₙ − lim|
                            </th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                sgn(Re(Aₙ − lim))
                            </th>
                        </tr>
                    </thead>
                    <tbody>
                        {points.slice(0, 200).map((p) => (
                            <tr key={p.n} className="odd:bg-surface/40 even:bg-surface/20">
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.n}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.valueRe != null && Number.isFinite(p.valueRe)
                                        ? p.valueRe.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.valueIm != null && Number.isFinite(p.valueIm)
                                        ? p.valueIm.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.err != null && Number.isFinite(p.err)
                                        ? p.err.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.sign === 1 ? "+" : p.sign === -1 ? "−" : "0 / ?"}
                                </td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
            {points.length > 200 && (
                <div className="mt-1 text-[10px] text-textDim/60">
                    Показаны первые 200 точек из {points.length}.
                </div>
            )}
        </div>
    );
}

/* ================== Компонент таблицы ================== */

export interface AlgorithmSeriesConvergenceTableProps {
    experiment: Experiment | null;
    className?: string;
    /** максимальное число рядов (колонок) на страницу */
    maxSeries?: number;
}

interface SelectedCell {
    seriesId: string;
    accelId: string;
}

export const AlgorithmSeriesConvergenceTable: React.FC<AlgorithmSeriesConvergenceTableProps> = ({
    experiment,
    className,
    maxSeries,
}) => {
    const { matrix, progress } = useConvergenceMatrix(experiment);
    const [page, setPage] = useState(0);
    const [selectedCell, setSelectedCell] = useState<SelectedCell | null>(null);
    const chartRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setPage(0);
        setSelectedCell(null);
    }, [experiment]);

    const seriesList = matrix?.seriesList ?? [];
    const algoList = matrix?.algoList ?? [];
    const cells = matrix?.cells ?? {};

    const pageSize = maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;
    const totalPages = Math.max(1, Math.ceil((seriesList.length || 1) / pageSize));

    useEffect(() => {
        setPage((prev) => {
            if (prev >= totalPages) return totalPages - 1;
            if (prev < 0) return 0;
            return prev;
        });
    }, [totalPages]);

    useEffect(() => {
        if (selectedCell && chartRef.current) {
            chartRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!selectedCell || !experiment || !matrix) return null;

        const { seriesId, accelId } = selectedCell;

        const series = (experiment.seriesList ?? []).find((s) => s.id === seriesId) ?? null;
        const accel = (experiment.accelList ?? []).find((a) => a.id === accelId) ?? null;
        const sa =
            (experiment.seriesAccelList ?? []).find(
                (x) => x.series_id === seriesId && x.accel_id === accelId
            ) ?? null;

        const seriesInfo = matrix.seriesList.find((s) => s.key === seriesId) ?? null;
        const algoInfo = matrix.algoList.find((a) => a.key === accelId) ?? null;

        const analysis = matrix.cells[`${accelId}::${seriesId}`] ?? null;
        const limit = series?.limit ?? null;

        const points: DetailPoint[] = sa
            ? getPointsSortedByN(sa).map((p) => {
                  const valueRe = p.value?.re ?? null;
                  const valueIm = p.value?.im ?? null;
                  const err = errorNorm(p.value, limit);
                  const sign = realDiffSign(p.value, limit);
                  return { n: p.n, valueRe, valueIm, err, sign };
              })
            : [];

        return {
            seriesInfo,
            algoInfo,
            series,
            accel,
            analysis,
            limit,
            points,
        };
    }, [selectedCell, experiment, matrix]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет данных для анализа (experiment = null).
                </div>
            </div>
        );
    }

    if (!matrix || progress.running) {
        const { current, total } = progress;
        const pct = total > 0 ? Math.max(0, Math.min(100, Math.round((current / total) * 100))) : 0;

        return (
            <div className={className}>
                <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
                    <div className="mb-2 flex items-center justify-between text-sm text-textDim">
                        <span>Подсчёт монотонности и направления...</span>
                        <span>
                            {current} / {total} ({pct}%)
                        </span>
                    </div>
                    <div className="h-2 w-full rounded bg-surface/60 overflow-hidden">
                        <div
                            className="h-2 bg-primary transition-[width]"
                            style={{ width: `${pct}%` }}
                        />
                    </div>
                </div>
            </div>
        );
    }

    if (seriesList.length === 0 || algoList.length === 0) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет пар ряд × алгоритм для анализа (seriesList или accelList пусты).
                </div>
            </div>
        );
    }

    const startIndex = page * pageSize;
    const endIndex = Math.min(startIndex + pageSize, seriesList.length);
    const seriesSlice = seriesList.slice(startIndex, endIndex);

    return (
        <div className={className}>
            <div className="mb-2 flex items-center justify-between text-xs text-textDim">
                <div className="flex flex-col gap-1">
                    <span className="text-sm font-semibold text-textDim">
                        Монотонность и направление: алгоритмы × ряды
                    </span>
                    <span className="text-[11px] text-textDim/80">
                        Алгоритмы: {algoList.length} · Ряды: {seriesList.length}
                    </span>
                </div>

                {seriesList.length > pageSize && (
                    <div className="flex items-center gap-1 text-[10px]">
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                            onClick={() => setPage(0)}
                            disabled={page === 0}
                        >
                            «
                        </button>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                            onClick={() => setPage((p) => Math.max(0, p - 1))}
                            disabled={page === 0}
                        >
                            ‹
                        </button>
                        <span className="px-1">
                            стр. {page + 1} / {totalPages}
                        </span>
                        <span className="text-textDim/60">
                            колонки {startIndex + 1}–{endIndex}
                        </span>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                            onClick={() => setPage((p) => Math.min(totalPages - 1, p + 1))}
                            disabled={page >= totalPages - 1}
                        >
                            ›
                        </button>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                            onClick={() => setPage(totalPages - 1)}
                            disabled={page >= totalPages - 1}
                        >
                            »
                        </button>
                    </div>
                )}
            </div>

            <div className="overflow-auto rounded-xl2 border border-border bg-panel shadow-panel">
                <table className="border-collapse text-[10px] leading-tight text-textDim min-w-full">
                    <thead className="bg-surface/80">
                        <tr>
                            <th className="sticky left-0 top-0 z-20 border border-border bg-surface/90 px-1 py-1 text-left align-bottom text-[10px]">
                                Алгоритм \ Ряд
                            </th>

                            {seriesSlice.map((s) => (
                                <th
                                    key={s.key}
                                    className="border border-border px-0 py-0 text-center align-bottom"
                                    title={`${s.seriesName}\n x = ${s.xLabel}\n prec = ${s.precision}`}
                                >
                                    <div className="relative h-28 w-[44px] flex items-center justify-center">
                                        <span className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 rotate-[-90deg] whitespace-nowrap text-[9px] leading-tight">
                                            {s.seriesName}
                                        </span>
                                        <span className="absolute bottom-3 text-[8px] text-textDim/70">
                                            x={s.xLabel}
                                        </span>
                                        <span className="absolute bottom-0 text-[8px] text-textDim/60">
                                            prec={s.precision}
                                        </span>
                                    </div>
                                </th>
                            ))}
                        </tr>
                    </thead>
                    <tbody>
                        {algoList.map((algo) => (
                            <tr key={algo.key}>
                                <th
                                    className="sticky left-0 z-10 border border-border bg-panel px-1 py-[2px] text-left align-top"
                                    title={(() => {
                                        const lines: string[] = [];
                                        lines.push(`Алгоритм: ${algo.algorithmName}`);
                                        lines.push(`m = ${algo.m != null ? String(algo.m) : "∅"}`);
                                        const entries = nonNullEntries(algo.algorithmArgs);
                                        if (entries.length > 0) {
                                            lines.push("Аргументы:");
                                            for (const [k, v] of entries.sort(([a], [b]) =>
                                                a.localeCompare(b)
                                            )) {
                                                lines.push(`  ${k}: ${v}`);
                                            }
                                        }
                                        return lines.join("\n");
                                    })()}
                                >
                                    <div className="whitespace-pre leading-tight">
                                        <span className="block max-w-[150px] truncate">
                                            {algo.algorithmName}
                                        </span>
                                        <span className="text-[9px] text-textDim/70">
                                            {algo.m != null ? `m=${String(algo.m)}` : "m=∅"}
                                        </span>
                                        {algo.argsSummary && (
                                            <div className="mt-[1px] max-w-[150px] truncate text-[8px] text-textDim/60">
                                                {algo.argsSummary}
                                            </div>
                                        )}
                                    </div>
                                </th>

                                {seriesSlice.map((s) => {
                                    const key = `${algo.key}::${s.key}`;
                                    const analysis = cells[key];

                                    if (!analysis) {
                                        return (
                                            <td
                                                key={key}
                                                className="border border-border px-[2px] py-[2px] text-center text-textDim/50 text-[10px]"
                                            >
                                                —
                                            </td>
                                        );
                                    }

                                    const sideShort = formatSideShort(analysis.side);
                                    const monShort = formatMonotonicityShort(analysis.monotonicity);

                                    const title = [
                                        `Ряд: ${s.seriesName}, x=${s.xLabel}, prec=${s.precision}`,
                                        `Алгоритм: ${algo.algorithmName}, m=${algo.m ?? "∅"}`,
                                        "",
                                        formatSideDescription(analysis.side),
                                        formatMonotonicityDescription(analysis.monotonicity),
                                        analysis.signChangesCount > 0
                                            ? `Число смен знака: ${analysis.signChangesCount}, первое при n = ${analysis.firstSignChangeN}.`
                                            : "Число смен знака не обнаружено.",
                                        analysis.firstGrowthN != null
                                            ? `Первый рост ошибки при n = ${analysis.firstGrowthN}.`
                                            : "Первый рост ошибки не обнаружен или данных недостаточно.",
                                        `Сравнено шагов (пар): ${analysis.stepsAnalyzed}.`,
                                        "",
                                        "Нажмите по ячейке, чтобы посмотреть детальный график.",
                                    ].join("\n");

                                    const isSelected =
                                        selectedCell?.seriesId === s.key &&
                                        selectedCell?.accelId === algo.key;

                                    const baseCell =
                                        "min-w-[30px] border px-[2px] py-[2px] text-center text-[10px] cursor-pointer";
                                    const colorClass = getCellColorClass(
                                        analysis.side,
                                        analysis.monotonicity,
                                        isSelected
                                    );

                                    return (
                                        <td
                                            key={key}
                                            className={baseCell + " " + colorClass}
                                            title={title}
                                            onClick={() =>
                                                setSelectedCell({
                                                    seriesId: s.key,
                                                    accelId: algo.key,
                                                })
                                            }
                                        >
                                            <div className="flex flex-col items-center gap-[1px] leading-tight select-none">
                                                <span className="font-mono text-[10px]">
                                                    {sideShort} | {monShort}
                                                </span>
                                                <span className="text-[9px] text-textDim/80">
                                                    k: {analysis.stepsAnalyzed}
                                                </span>
                                            </div>
                                        </td>
                                    );
                                })}
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>

            <div ref={chartRef}>
                {selectedDetail && selectedDetail.analysis && (
                    <ConvergenceDetailChart detail={selectedDetail} />
                )}
            </div>
        </div>
    );
};
