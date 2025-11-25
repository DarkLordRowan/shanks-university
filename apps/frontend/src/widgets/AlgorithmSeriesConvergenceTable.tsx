// src/widgets/AlgorithmSeriesConvergenceTable.tsx

import React, { useEffect, useState } from "react";
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

/**
 * Цвет ячейки в зависимости от комбинации (side, monotonicity).
 */
function getCellColorClass(side: SideType, mon: MonotonicityType): string {
    let base = "border-border text-textDim";

    if (mon === "strict_decreasing_error" || mon === "non_increasing_error") {
        if (side === "one_sided") {
            return base + " bg-emerald-500/25";
        }
        if (side === "two_sided") {
            return base + " bg-emerald-400/20";
        }
        if (side === "unknown") {
            return base + " bg-emerald-400/10";
        }
        if (side === "no_limit") {
            return "border-border/70 text-textDim/70 bg-surface/40";
        }
    }

    if (mon === "constant_error") {
        if (side === "one_sided") {
            return base + " bg-surface/70";
        }
        if (side === "two_sided") {
            return base + " bg-surface/60";
        }
        return "border-border/70 text-textDim/70 bg-surface/50";
    }

    if (mon === "has_growth") {
        if (side === "one_sided") {
            return "border-amber-500 text-textDim bg-amber-500/25";
        }
        if (side === "two_sided") {
            return "border-red-500 text-textDim bg-red-500/30";
        }
        if (side === "unknown") {
            return "border-amber-500/80 text-textDim bg-amber-500/20";
        }
        if (side === "no_limit") {
            return "border-red-500/70 text-textDim/80 bg-red-500/20";
        }
    }

    if (mon === "not_enough_data" || mon === "no_limit") {
        if (side === "no_limit") {
            return "border-border/60 text-textDim/60 bg-surface/30";
        }
        return "border-border/50 text-textDim/60 bg-surface/40";
    }

    return "border-border text-textDim bg-surface/40";
}

/* ================== Компонент таблицы ================== */

export interface AlgorithmSeriesConvergenceTableProps {
    experiment: Experiment | null;
    className?: string;
    /** максимальное число рядов (колонок) на страницу */
    maxSeries?: number;
}

export const AlgorithmSeriesConvergenceTable: React.FC<AlgorithmSeriesConvergenceTableProps> = ({
    experiment,
    className,
    maxSeries,
}) => {
    const { matrix, progress } = useConvergenceMatrix(experiment);
    const [page, setPage] = useState(0);

    useEffect(() => {
        setPage(0);
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
                                            : "Смен знака A_k - lim не обнаружено.",
                                        analysis.firstGrowthN != null
                                            ? `Первый рост ошибки при n = ${analysis.firstGrowthN}.`
                                            : "Рост ошибки |A_k - lim| не обнаружен или данных недостаточно.",
                                        `Сравнено шагов (пар): ${analysis.stepsAnalyzed}.`,
                                    ].join("\n");

                                    const baseCell =
                                        "min-w-[30px] border px-[2px] py-[2px] text-center text-[10px] cursor-default";
                                    const colorClass = getCellColorClass(
                                        analysis.side,
                                        analysis.monotonicity
                                    );

                                    return (
                                        <td
                                            key={key}
                                            className={baseCell + " " + colorClass}
                                            title={title}
                                        >
                                            <div className="flex flex-col items-center gap-[1px] leading-tight">
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
        </div>
    );
};
