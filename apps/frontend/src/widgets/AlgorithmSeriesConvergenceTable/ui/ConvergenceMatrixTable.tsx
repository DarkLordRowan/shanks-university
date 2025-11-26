import React, { useEffect, useMemo, useState } from "react";
import {
    type ConvergenceMatrix,
    type MonotonicityType,
    type SelectedCell,
    type SideType,
} from "../model/types";
import {
    formatMonotonicityShort,
    formatSideShort,
    nonNullEntries,
} from "../model/convergenceUtils";

interface ConvergenceMatrixTableProps {
    matrix: ConvergenceMatrix;
    maxSeries?: number;
    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
}

/** Единый способ построить id для DOM-элемента ячейки. */
export function getConvergenceCellDomId(accelId: string, seriesId: string): string {
    return `conv-cell-${accelId}::${seriesId}`;
}

export const ConvergenceMatrixTable: React.FC<ConvergenceMatrixTableProps> = ({
    matrix,
    maxSeries,
    selectedCell,
    onCellSelect,
}) => {
    const rawSeriesList = matrix.seriesList ?? [];
    const algoList = matrix.algoList ?? [];
    const cells = matrix.cells ?? {};

    const thresholds = useMemo(() => {
        let maxSignChanges = 0;
        let maxViolations = 0;

        for (const key of Object.keys(cells)) {
            const analysis = cells[key];
            if (!analysis) continue;

            const sc =
                typeof analysis.signChangesCount === "number" ? analysis.signChangesCount : 0;
            if (sc > maxSignChanges) maxSignChanges = sc;

            const violationsRaw =
                typeof analysis.growthViolationsCount === "number"
                    ? analysis.growthViolationsCount
                    : 0;

            if (violationsRaw > maxViolations) maxViolations = violationsRaw;
        }

        return { maxSignChanges, maxViolations };
    }, [cells]);

    const [maxSignChangesForOneSided, setMaxSignChangesForOneSided] = useState<number>(0);
    const [maxViolationsForMonotone, setMaxViolationsForMonotone] = useState<number>(0);

    useEffect(() => {
        setMaxSignChangesForOneSided(0);
        setMaxViolationsForMonotone(0);
    }, [matrix]);

    const allPrecisions = useMemo(() => {
        const set = new Set<string>();
        for (const s of rawSeriesList) {
            if (s.precision != null) set.add(String(s.precision));
        }
        return Array.from(set).sort();
    }, [rawSeriesList]);

    const [precisionFilter, setPrecisionFilter] = useState<"ALL" | string>("ALL");

    const seriesList = useMemo(() => {
        if (precisionFilter === "ALL") return rawSeriesList;
        return rawSeriesList.filter((s) => String(s.precision) === precisionFilter);
    }, [rawSeriesList, precisionFilter]);

    const [page, setPage] = useState(0);

    const pageSize = maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;
    const totalPages = Math.max(1, Math.ceil((seriesList.length || 1) / pageSize));

    useEffect(() => {
        setPage(0);
    }, [matrix, maxSeries, precisionFilter]);

    useEffect(() => {
        setPage((prev) => {
            if (prev >= totalPages) return totalPages - 1;
            if (prev < 0) return 0;
            return prev;
        });
    }, [totalPages]);

    const signChangesSliderMax = thresholds.maxSignChanges > 0 ? thresholds.maxSignChanges : 5;
    const violationsSliderMax = thresholds.maxViolations > 0 ? thresholds.maxViolations : 5;

    if (rawSeriesList.length === 0 || algoList.length === 0) {
        return (
            <div className="text-textDim text-sm">
                Нет пар ряд × алгоритм для анализа (seriesList или accelList пусты).
            </div>
        );
    }

    const startIndex = page * pageSize;
    const endIndex = Math.min(startIndex + pageSize, seriesList.length);
    const seriesSlice = seriesList.slice(startIndex, endIndex);

    return (
        <>
            <div className="mb-2 flex items-center justify-between text-xs text-textDim">
                <div className="flex flex-col gap-1">
                    <span className="text-sm font-semibold text-textDim">
                        Монотонность и направление: алгоритмы × ряды
                    </span>
                    <span className="text-[11px] text-textDim/80">
                        Алгоритмы: {algoList.length} · Ряды: {seriesList.length}
                        {precisionFilter !== "ALL"
                            ? ` (из ${rawSeriesList.length}, precision=${precisionFilter})`
                            : ""}
                    </span>
                </div>

                <div className="flex items-center gap-3">
                    {/* выбор precision как в примере heatmap */}
                    <div className="flex items-center gap-1 text-[10px]">
                        <span>precision:</span>
                        <select
                            className="rounded border border-border bg-surface px-2 py-[2px]"
                            value={precisionFilter}
                            onChange={(e) =>
                                setPrecisionFilter(
                                    e.target.value === "ALL" ? "ALL" : e.target.value
                                )
                            }
                        >
                            <option value="ALL">Все</option>
                            {allPrecisions.map((p) => (
                                <option key={p} value={p}>
                                    {p}
                                </option>
                            ))}
                        </select>
                    </div>

                    {/* пороги пересчёта направления и монотонности */}
                    <div className="flex flex-col gap-[2px] text-[10px]">
                        <div className="flex items-center gap-1">
                            <span
                                className="whitespace-nowrap"
                                title="Если число смен знака ≤ X, пара считается односторонней"
                            >
                                max sign changes:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={signChangesSliderMax}
                                value={maxSignChangesForOneSided}
                                onChange={(e) =>
                                    setMaxSignChangesForOneSided(Number(e.target.value))
                                }
                                className="h-[4px] w-28 cursor-pointer"
                            />
                            <span className="w-6 text-right tabular-nums">
                                {maxSignChangesForOneSided}
                            </span>
                        </div>
                        <div className="flex items-center gap-1">
                            <span
                                className="whitespace-nowrap"
                                title="Если число расхождений ≤ Y, ошибка считается монотонной"
                            >
                                max deviations:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={violationsSliderMax}
                                value={maxViolationsForMonotone}
                                onChange={(e) =>
                                    setMaxViolationsForMonotone(Number(e.target.value))
                                }
                                className="h-[4px] w-28 cursor-pointer"
                            />
                            <span className="w-6 text-right tabular-nums">
                                {maxViolationsForMonotone}
                            </span>
                        </div>
                    </div>

                    {seriesList.length > pageSize && (
                        <div className="flex items-center gap-1 text-[10px]">
                            <button
                                type="button"
                                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                                onClick={() => setPage(0)}
                                disabled={page === 0}
                            >
                                «
                            </button>
                            <button
                                type="button"
                                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
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
                                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                                onClick={() => setPage((p) => Math.min(totalPages - 1, p + 1))}
                                disabled={page >= totalPages - 1}
                            >
                                ›
                            </button>
                            <button
                                type="button"
                                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                                onClick={() => setPage(totalPages - 1)}
                                disabled={page >= totalPages - 1}
                            >
                                »
                            </button>
                        </div>
                    )}
                </div>
            </div>

            <div className="overflow-auto rounded-xl2 border border-border bg-panel shadow-panel">
                <table className="min-w-full border-collapse text-[10px] leading-tight text-textDim">
                    <thead className="bg-surface/80">
                        <tr>
                            <th className="sticky left-0 top-0 z-20 border border-border bg-surface/90 px-1 py-1 text-left text-[10px] align-bottom">
                                Алгоритм \ Ряд
                            </th>

                            {seriesSlice.map((s) => (
                                <th
                                    key={s.key}
                                    className="border border-border px-0 py-0 text-center align-bottom"
                                    title={`${s.seriesName}\n x = ${s.xLabel}\n prec = ${s.precision}`}
                                >
                                    <div className="relative flex h-28 w-[44px] items-center justify-center">
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
                                                className="border border-border px-[2px] py-[2px] text-center text-[10px] text-textDim/50"
                                            >
                                                —
                                            </td>
                                        );
                                    }

                                    const rawSide = analysis.side;
                                    const rawMon = analysis.monotonicity;

                                    const signChanges =
                                        typeof analysis.signChangesCount === "number"
                                            ? analysis.signChangesCount
                                            : 0;

                                    const violationsCount =
                                        typeof analysis.growthViolationsCount === "number"
                                            ? analysis.growthViolationsCount
                                            : 0;

                                    // направление с учётом порога X
                                    let effectiveSide: SideType = rawSide;
                                    if (rawSide !== "no_limit") {
                                        if (signChanges <= maxSignChangesForOneSided) {
                                            effectiveSide = "one_sided";
                                        } else {
                                            effectiveSide = "two_sided";
                                        }
                                    }

                                    // монотонность с учётом порога Y
                                    let effectiveMon: MonotonicityType = rawMon;
                                    if (rawMon !== "no_limit" && rawMon !== "not_enough_data") {
                                        if (violationsCount <= maxViolationsForMonotone) {
                                            // если формально "есть рост", но он в пределах допуска,
                                            // считаем как не возрастающую ошибку
                                            if (rawMon === "has_growth") {
                                                effectiveMon = "non_increasing_error";
                                            }
                                        }
                                    }

                                    const sideShort = formatSideShort(effectiveSide);
                                    const monShort = formatMonotonicityShort(effectiveMon);

                                    const titleLines: string[] = [];

                                    // Ряд / алгоритм
                                    titleLines.push(
                                        `Ряд: ${s.seriesName} (x=${s.xLabel}, prec=${s.precision})`
                                    );
                                    titleLines.push(
                                        `Алгоритм: ${algo.algorithmName}` +
                                            (algo.m != null ? `, m=${algo.m}` : "")
                                    );
                                    titleLines.push("Аргументы алгоритма:");

                                    // Конфигурация алгоритма (кратко)
                                    const algoEntries = nonNullEntries(algo.algorithmArgs);
                                    if (algoEntries.length > 0) {
                                        for (const [k, v] of algoEntries.sort(([a], [b]) =>
                                            a.localeCompare(b)
                                        )) {
                                            titleLines.push(`  ${k} = ${String(v)}`);
                                        }
                                    }
                                    if (algo.argsSummary) {
                                        titleLines.push(`  (${algo.argsSummary})`);
                                    }
                                    if (algoEntries.length > 0 || algo.argsSummary) {
                                        titleLines.push("");
                                    }

                                    // Итоговая классификация
                                    const classDescr = describeClass(effectiveSide, effectiveMon);
                                    titleLines.push(`Класс: ${classDescr}`);

                                    const signNsText =
                                        analysis.signChangeNs && analysis.signChangeNs.length > 0
                                            ? formatIntervals(analysis.signChangeNs)
                                            : analysis.firstSignChangeN != null
                                              ? String(analysis.firstSignChangeN)
                                              : "—";

                                    const growthNsText =
                                        analysis.growthNs && analysis.growthNs.length > 0
                                            ? formatIntervals(analysis.growthNs)
                                            : analysis.firstGrowthN != null
                                              ? String(analysis.firstGrowthN)
                                              : "—";

                                    titleLines.push(
                                        `Число смен знака: ${analysis.signChangesCount}, ns: ${signNsText}`
                                    );

                                    titleLines.push(
                                        `Число роста |Aₙ−lim|: ${analysis.growthViolationsCount}, ns: ${growthNsText}`
                                    );

                                    // Объём данных
                                    titleLines.push(
                                        `Пар (n−1,n) в анализе: ${analysis.stepsAnalyzed}`
                                    );

                                    titleLines.push("");
                                    titleLines.push("Клик — детальный график.");

                                    const title = titleLines.join("\n");

                                    const isSelected =
                                        selectedCell?.seriesId === s.key &&
                                        selectedCell?.accelId === algo.key;

                                    const baseCell =
                                        "min-w-[30px] border px-[2px] py-[2px] text-center text-[10px] cursor-pointer";

                                    const colorClass = getCellColorClass(
                                        effectiveSide,
                                        effectiveMon,
                                        isSelected
                                    );

                                    const domId = getConvergenceCellDomId(algo.key, s.key);

                                    return (
                                        <td
                                            key={key}
                                            id={domId}
                                            className={baseCell + " " + colorClass}
                                            title={title}
                                            onClick={() =>
                                                onCellSelect({
                                                    seriesId: s.key,
                                                    accelId: algo.key,
                                                })
                                            }
                                        >
                                            <div className="flex select-none flex-col items-center gap-[1px] leading-tight">
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
        </>
    );
};

/**
 * Цвета по правилам:
 *  - односторонний и монотонный  → зелёный
 *  - односторонний и немонотонный → синий
 *  - двусторонний и монотонный   → жёлтый
 *  - двусторонний и немонотонный → красный
 */
function getCellColorClass(side: SideType, mon: MonotonicityType, selected: boolean): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (side === "no_limit" || mon === "not_enough_data" || mon === "no_limit") {
        return "border-border/60 text-textDim/70 bg-surface/30 hover:bg-surface/40" + sel;
    }

    const mono = isMonotone(mon);

    if (side === "one_sided" && mono) {
        return "border-border text-textDim bg-emerald-500/25 hover:bg-emerald-500/35" + sel;
    }
    if (side === "one_sided" && !mono) {
        return "border-border text-textDim bg-sky-500/25 hover:bg-sky-500/35" + sel;
    }

    if (side === "two_sided" && mono) {
        return "border-border text-textDim bg-amber-300/35 hover:bg-amber-300/45" + sel;
    }
    if (side === "two_sided" && !mono) {
        return "border-border text-textDim bg-red-500/30 hover:bg-red-500/40" + sel;
    }

    return "border-border text-textDim bg-surface/40 hover:bg-surface/50" + sel;
}

function formatIntervals(ns: number[], maxRanges = 5): string {
    if (!ns.length) return "—";

    const sorted = Array.from(new Set(ns)).sort((a, b) => a - b);

    const ranges: Array<{ start: number; end: number }> = [];
    let start = sorted[0];
    let prev = sorted[0];

    for (let i = 1; i < sorted.length; i++) {
        const x = sorted[i];
        if (x === prev + 1) {
            prev = x;
            continue;
        }
        ranges.push({ start, end: prev });
        start = x;
        prev = x;
    }
    ranges.push({ start, end: prev });

    const parts = ranges
        .slice(0, maxRanges)
        .map((r) => (r.start === r.end ? `${r.start}` : `${r.start}–${r.end}`));

    if (ranges.length > maxRanges) {
        parts.push("…");
    }

    return parts.join(", ");
}

function isMonotone(mon: MonotonicityType): boolean {
    return (
        mon === "strict_decreasing_error" ||
        mon === "non_increasing_error" ||
        mon === "constant_error"
    );
}

function describeClass(side: SideType, mon: MonotonicityType): string {
    const mono = isMonotone(mon);

    if (side === "one_sided" && mono) return "односторонний и монотонный";
    if (side === "one_sided" && !mono) return "односторонний и немонотонный";
    if (side === "two_sided" && mono) return "двусторонний и монотонный";
    if (side === "two_sided" && !mono) return "двусторонний и немонотонный";

    if (side === "no_limit") return "предел не просматривается";
    if (mon === "not_enough_data") return "недостаточно данных по ошибке";

    return "тип не определён";
}
