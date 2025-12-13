// src/widgets/AlgorithmSeriesErrorMatrix.tsx

import React, { useEffect, useMemo, useState } from "react";
import type {
    Accel,
    AccelArgs,
    Complex,
    Experiment,
    Series,
    SeriesAccel,
    SeriesArgs,
} from "@/entities/experiment/model/experiment";
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";
import type { MatrixAxisItem, MatrixProps } from "@/shared/ui/Matrix/Matrix";

type SeriesKey = string;
type AlgoKey = string;

interface SeriesInfo {
    key: SeriesKey;
    seriesName: string;
    xLabel: string;
    xSort: number | null;
    limit: Complex | null;
    args: SeriesArgs | null;
    precision: string;
}

interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: unknown; // number | bigint | string | null
    argsSummary: string;
    algorithmArgs: AccelArgs | null;
}

interface AlgorithmSeriesErrorMatrixProps {
    experiment: Experiment | null;
    /** размер страницы по столбцам; если не задан, показываем все */
    maxSeries?: number;
}

/* ---------------- utils ---------------- */

/** Безопасное приведение к числу для сортировки. */
function toSortableNumber(v: unknown): number | null {
    if (typeof v === "number") return Number.isFinite(v) ? v : null;
    if (typeof v === "bigint") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    if (typeof v === "string") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    return null;
}

function parseX(args: SeriesArgs | null): { xLabel: string; xSort: number | null } {
    const raw = args?.x;
    if (raw == null) return { xLabel: "∅", xSort: null };
    return { xLabel: String(raw), xSort: toSortableNumber(raw) };
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

function formatComplex(c: Complex | null): string {
    if (!c) return "∅";
    const { re, im } = c;
    if (re == null && im == null) return "∅";
    if (im == null || im === 0) return String(re);
    if (re == null || re === 0) return `${im}i`;
    const sign = im >= 0 ? "+" : "-";
    return `${re} ${sign} ${Math.abs(im)}i`;
}

/* ---------------- cell summary ---------------- */

interface CellSummary {
    state: "no-data" | "all-ok" | "ok-with-errors" | "only-errors";
    totalN: number;
    okCount: number;
    firstOkN: number | null;
    lastOkN: number | null;
    firstErrorN: number | null;
    lastErrorN: number | null;
    errorCount: number;
    divergentCount: number;
    uniqueErrorMessages: string[];
}

/** Строим сводку по SeriesAccel. */
function summarizeSeriesAccel(sa: SeriesAccel): CellSummary {
    const computed = sa.computed ?? [];
    const errors = sa.errors ?? [];
    const events = sa.events ?? [];

    const totalN = computed.length;

    const errorNs = new Set<number>();
    const errorMessages: string[] = [];

    for (const e of errors) {
        if (typeof e.n === "number") errorNs.add(e.n);
        const msg =
            typeof e.message === "string" ? e.message.trim() : String(e.message ?? "").trim();
        if (msg.length > 0) errorMessages.push(msg);
    }

    const uniqueErrorMessages = Array.from(new Set(errorMessages));

    const okNs: number[] = [];
    for (const cp of computed) {
        if (!cp) continue;
        const n = cp.n;
        if (typeof n !== "number") continue;

        const hasValue = cp.value != null && (cp.value.re != null || cp.value.im != null);
        if (hasValue && !errorNs.has(n)) okNs.push(n);
    }

    okNs.sort((a, b) => a - b);

    const firstOkN = okNs.length > 0 ? okNs[0] : null;
    const lastOkN = okNs.length > 0 ? okNs[okNs.length - 1] : null;

    const errorList = [...errorNs].sort((a, b) => a - b);
    const firstErrorN = errorList.length > 0 ? errorList[0] : null;
    const lastErrorN = errorList.length > 0 ? errorList[errorList.length - 1] : null;

    const divergentCount = events.filter((ev) => ev.name === "divergent_accel_method").length;

    let state: CellSummary["state"];
    if (totalN === 0 && errorList.length === 0) state = "no-data";
    else if (okNs.length === 0 && errorList.length > 0) state = "only-errors";
    else if (okNs.length > 0 && errorList.length === 0) state = "all-ok";
    else state = "ok-with-errors";

    return {
        state,
        totalN,
        okCount: okNs.length,
        firstOkN,
        lastOkN,
        firstErrorN,
        lastErrorN,
        errorCount: errorList.length,
        divergentCount,
        uniqueErrorMessages,
    };
}

/* ---------------- component ---------------- */

export const AlgorithmSeriesErrorMatrix: React.FC<AlgorithmSeriesErrorMatrixProps> = ({
    experiment,
    maxSeries,
}) => {
    /** null = все precision, конкретная строка = фильтр по precision */
    const [precisionFilter, setPrecisionFilter] = useState<string | null>(null);

    const { precisionsOrder, seriesList, algoList, cellMap, totalCells } = useMemo(() => {
        if (!experiment) {
            return {
                precisionsOrder: [] as string[],
                seriesList: [] as SeriesInfo[],
                algoList: [] as AlgoInfo[],
                cellMap: new Map<string, SeriesAccel>(),
                totalCells: 0,
            };
        }

        const seriesListRaw: Series[] = experiment.seriesList ?? [];
        const accelListRaw: Accel[] = experiment.accelList ?? [];
        const seriesAccelListRaw: SeriesAccel[] = experiment.seriesAccelList ?? [];

        const precisionsOrder: string[] = [];
        for (const s of seriesListRaw) {
            if (!precisionsOrder.includes(s.precision)) precisionsOrder.push(s.precision);
        }

        const seriesMap = new Map<SeriesKey, SeriesInfo>();
        const algoMap = new Map<AlgoKey, AlgoInfo>();
        const cells = new Map<string, SeriesAccel>();

        // series (с учётом фильтра по precision)
        for (const s of seriesListRaw) {
            if (precisionFilter && s.precision !== precisionFilter) continue;

            const key: SeriesKey = s.id;
            if (!seriesMap.has(key)) {
                const { xLabel, xSort } = parseX(s.args ?? null);
                seriesMap.set(key, {
                    key,
                    seriesName: s.name,
                    xLabel,
                    xSort,
                    limit: s.limit,
                    args: s.args,
                    precision: s.precision,
                });
            }
        }

        // algorithms
        for (const a of accelListRaw) {
            const key: AlgoKey = a.id;
            if (!algoMap.has(key)) {
                const args = a.args ?? null;
                algoMap.set(key, {
                    key,
                    algorithmName: a.name,
                    m: a.m,
                    argsSummary: buildArgsSummary(args),
                    algorithmArgs: args,
                });
            }
        }

        // cells: SeriesAccel по (series, accel), только для отфильтрованных series
        for (const sa of seriesAccelListRaw) {
            const sKey: SeriesKey = sa.series_id;
            const aKey: AlgoKey = sa.accel_id;

            if (!seriesMap.has(sKey)) continue;
            if (!algoMap.has(aKey)) continue;

            const cellKey = `${aKey}||${sKey}`;
            if (!cells.has(cellKey)) cells.set(cellKey, sa);
        }

        const seriesList = Array.from(seriesMap.values()).sort((a, b) => {
            const byName = a.seriesName.localeCompare(b.seriesName);
            if (byName !== 0) return byName;

            const ax = a.xSort;
            const bx = b.xSort;
            if (ax != null && bx != null) return ax - bx;
            if (ax != null) return -1;
            if (bx != null) return 1;
            return a.xLabel.localeCompare(b.xLabel);
        });

        const algoList = Array.from(algoMap.values()).sort((a, b) => {
            const byName = a.algorithmName.localeCompare(b.algorithmName);
            if (byName !== 0) return byName;

            const am = toSortableNumber(a.m);
            const bm = toSortableNumber(b.m);
            if (am != null && bm != null) return am - bm;
            if (am != null) return -1;
            if (bm != null) return 1;
            return 0;
        });

        return {
            precisionsOrder,
            seriesList,
            algoList,
            cellMap: cells,
            totalCells: seriesAccelListRaw.length,
        };
    }, [experiment, precisionFilter]);

    // если фильтр указывает на precision, которого больше нет, сбрасываем его
    useEffect(() => {
        if (precisionFilter && !precisionsOrder.includes(precisionFilter)) setPrecisionFilter(null);
    }, [precisionFilter, precisionsOrder]);

    const rowsAxis: MatrixAxisItem<AlgoInfo>[] = useMemo(
        () => algoList.map((a) => ({ id: a.key, meta: a })),
        [algoList]
    );

    const colsAxis: MatrixAxisItem<SeriesInfo>[] = useMemo(
        () => seriesList.map((s) => ({ id: s.key, meta: s })),
        [seriesList]
    );

    const renderRowHeader: MatrixProps<AlgoInfo, SeriesInfo>["renderRowHeader"] = (row) => {
        const algo = row.meta!;
        return (
            <div
                className="whitespace-pre leading-tight"
                title={(() => {
                    const lines: string[] = [];
                    lines.push(`Алгоритм: ${algo.algorithmName}`);
                    lines.push(`m = ${algo.m != null ? String(algo.m) : "∅"}`);

                    const entries = nonNullEntries(algo.algorithmArgs);
                    if (entries.length > 0) {
                        lines.push("Аргументы:");
                        for (const [k, v] of entries.sort(([a], [b]) => a.localeCompare(b))) {
                            lines.push(`  ${k}: ${v}`);
                        }
                    }
                    return lines.join("\n");
                })()}
            >
                <span className="block max-w-[160px] truncate">{algo.algorithmName}</span>
                <span className="text-[9px] text-textDim/70">
                    {algo.m != null ? `m=${String(algo.m)}` : "m=∅"}
                </span>
                {algo.argsSummary && (
                    <div className="mt-[1px] max-w-[160px] truncate text-[8px] text-textDim/60">
                        {algo.argsSummary}
                    </div>
                )}
            </div>
        );
    };

    const renderColHeader: MatrixProps<AlgoInfo, SeriesInfo>["renderColHeader"] = (col) => {
        const s = col.meta!;
        return (
            <div
                className="relative h-28 w-[32px] flex items-center justify-center"
                title={`${s.seriesName}\n x = ${s.xLabel}\n precision = ${s.precision}\n lim = ${formatComplex(
                    s.limit
                )}`}
            >
                <span className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 rotate-[-90deg] whitespace-nowrap text-[9px] leading-tight">
                    {s.seriesName}
                </span>
                <span className="absolute bottom-1 text-[8px] text-textDim">x={s.xLabel}</span>
            </div>
        );
    };

    const renderCell: MatrixProps<AlgoInfo, SeriesInfo>["renderCell"] = (row, col) => {
        const algo = row.meta!;
        const s = col.meta!;
        const cellKey = `${algo.key}||${s.key}`;
        const sa = cellMap.get(cellKey);

        if (!sa) {
            return (
                <span className="text-[9px] text-textDim/40" title="Нет данных">
                    —
                </span>
            );
        }

        const summary = summarizeSeriesAccel(sa);

        let bgClass = "bg-surface/40 hover:bg-surface/60";
        let borderClass = "border-border/60";
        let content: React.ReactNode = "—";

        switch (summary.state) {
            case "no-data":
                bgClass = "bg-surface/40 hover:bg-surface/60";
                borderClass = "border-border/60";
                content = "—";
                break;
            case "all-ok":
                bgClass = "bg-emerald-900/40 hover:bg-emerald-800/60";
                borderClass = "border-emerald-500/70";
                content = <span className="font-semibold text-emerald-200">✓</span>;
                break;
            case "only-errors":
                bgClass = "bg-red-900/40 hover:bg-red-800/60";
                borderClass = "border-red-500/70";
                content = <span className="font-semibold text-red-200">err</span>;
                break;
            case "ok-with-errors":
                bgClass = "bg-amber-900/40 hover:bg-amber-800/60";
                borderClass = "border-amber-500/70";
                if (summary.firstOkN != null && summary.lastOkN != null) {
                    content = (
                        <span className="font-semibold text-amber-100">
                            {summary.firstOkN === summary.lastOkN
                                ? `${summary.firstOkN}`
                                : `${summary.firstOkN}–${summary.lastOkN}`}
                        </span>
                    );
                } else {
                    content = <span className="font-semibold text-amber-100">err</span>;
                }
                break;
        }

        const tooltipLines: string[] = [];

        // ряд
        tooltipLines.push(`Ряд: ${s.seriesName}`);
        tooltipLines.push(`x = ${s.xLabel}`);
        tooltipLines.push(`precision = ${s.precision}`);
        tooltipLines.push(`lim = ${formatComplex(s.limit)}`);
        tooltipLines.push("");

        // алгоритм
        tooltipLines.push(`Алгоритм: ${algo.algorithmName}`);
        tooltipLines.push(`m = ${algo.m != null ? String(algo.m) : "∅"}`);
        const argEntries = nonNullEntries(algo.algorithmArgs);
        if (argEntries.length > 0) {
            tooltipLines.push("Аргументы:");
            for (const [k, v] of argEntries.sort(([a], [b]) => a.localeCompare(b))) {
                tooltipLines.push(`  ${k}: ${v}`);
            }
        }

        tooltipLines.push("");
        tooltipLines.push(`Всего n: ${summary.totalN}`);
        tooltipLines.push(
            `OK-точек: ${summary.okCount}${
                summary.firstOkN != null && summary.lastOkN != null
                    ? ` (n=${summary.firstOkN}…${summary.lastOkN})`
                    : ""
            }`
        );
        tooltipLines.push(
            `Ошибок: ${summary.errorCount}${
                summary.firstErrorN != null && summary.lastErrorN != null
                    ? ` (n=${summary.firstErrorN}…${summary.lastErrorN})`
                    : ""
            }`
        );
        if (summary.divergentCount > 0) {
            tooltipLines.push(`divergent_accel_method: ${summary.divergentCount}`);
        }
        if (summary.uniqueErrorMessages.length > 0) {
            tooltipLines.push("");
            tooltipLines.push("Типы ошибок:");
            for (const msg of summary.uniqueErrorMessages) tooltipLines.push(`  • ${msg}`);
        }

        return (
            <div
                title={tooltipLines.join("\n")}
                className={`w-full h-full border px-[2px] py-[2px] text-center text-[10px] cursor-default ${borderClass} ${bgClass}`}
            >
                {content}
            </div>
        );
    };

    const totalAlgos = algoList.length;
    const totalSeries = seriesList.length;

    return (
        <MatrixPaged<AlgoInfo, SeriesInfo>
            rows={rowsAxis}
            cols={colsAxis}
            maxColsPerPage={maxSeries && maxSeries > 0 ? maxSeries : 0}
            resetKey={`${(experiment as any)?.id ?? "no-exp"}::${precisionFilter ?? "all"}`}
            className="space-y-2"
            renderTitle={() => "Матрица ошибок: алгоритмы × ряды"}
            renderSubtitle={() =>
                precisionFilter ? `precision: ${precisionFilter}` : "precision: все"
            }
            renderHeaderRight={({ page, totalPages, startIndex, endIndex }) => (
                <div className="flex items-center gap-3 text-[10px] text-textDim">
                    <div className="flex items-center gap-2">
                        <span>precision:</span>
                        <select
                            className="rounded border border-border bg-surface px-1 py-[1px]"
                            value={precisionFilter ?? ""}
                            onChange={(e) =>
                                setPrecisionFilter(e.target.value === "" ? null : e.target.value)
                            }
                        >
                            <option value="">все</option>
                            {precisionsOrder.map((p) => (
                                <option key={p} value={p}>
                                    {p}
                                </option>
                            ))}
                        </select>
                    </div>

                    <div className="whitespace-nowrap">
                        Алгоритмы: {totalAlgos} · Ряды:{" "}
                        {Math.min(endIndex, totalSeries) - startIndex} из {totalSeries} · Связок
                        series-accel: {totalCells}
                    </div>

                    {totalPages > 1 ? (
                        <div className="text-textDim/60 whitespace-nowrap">
                            стр. {page + 1}/{totalPages} · колонки {startIndex + 1}–{endIndex}
                        </div>
                    ) : null}
                </div>
            )}
            // Matrix props
            enableInnerScroll
            maxBodyHeight="70vh"
            stickyHeaders
            rowWidth={220}
            colWidth={32}
            tableClassName="border-separate border-spacing-0"
            thClassName="bg-surface"
            tdClassName="p-0"
            renderCorner={() => <span className="text-left">Алгоритм \ Ряд</span>}
            renderRowHeader={renderRowHeader}
            renderColHeader={renderColHeader}
            renderCell={renderCell}
            emptyFallback={
                <div className="rounded-xl2 border border-border bg-panel p-3 text-[11px] text-textDim/70">
                    Нет данных
                </div>
            }
        />
    );
};
