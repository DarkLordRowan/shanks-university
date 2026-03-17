import React, { useMemo } from "react";
import {
    type ConvergenceMatrix,
    type Experiment,
    type SelectedCell,
} from "../model/types";
import {
    formatMonotonicityWithMax,
    formatSideWithMax,
    getConvergenceCellDomId,
} from "../model/convergenceUtils";
import {
    buildConvergenceClassLegendTitle,
    computeConvergenceDevStatsFromSeriesAccel,
    formatAmplitudeOrders,
    formatComplexValue,
    formatDeviationValue,
    getConvergenceClassInfo,
    type ConvergenceClassInfo,
} from "../model/convergenceSummary";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries.tsx";
import { appendAlgorithmArgsTooltipLines } from "@/shared/lib/matrixTooltip";
import { buildSeriesAccelPairKey } from "@/shared/lib/experimentIndex";

interface ConvergenceMatrixTableProps {
    experiment: Experiment;
    matrix: ConvergenceMatrix;
    maxSeries?: number;
    maxSignChangesForOneSided: number;
    maxViolationsForMonotone: number;
    onMaxSignChangesForOneSidedChange: (value: number) => void;
    onMaxViolationsForMonotoneChange: (value: number) => void;
    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
}

interface CellSummary {
    classInfo: ConvergenceClassInfo;
    minN: number | null;
    min: number | null;
    lastMinusMin: number | null;
    amplitudeOrders: number | null;
}

const EMPTY_CLASS_INFO = getConvergenceClassInfo("unknown", "unknown", {
    count: 0,
    first: null,
    firstN: null,
    min: null,
    minN: null,
    mean: null,
    median: null,
    max: null,
    last: null,
    lastN: null,
    lastMinusMin: null,
    amplitudeOrders: null,
    plateauStartN: null,
});

function formatIntervals(ns: number[], maxRanges = 5): string {
    if (!ns.length) return "—";

    const sorted = Array.from(new Set(ns)).sort((a, b) => a - b);
    const ranges: Array<{ start: number; end: number }> = [];
    let start = sorted[0];
    let prev = sorted[0];

    for (let i = 1; i < sorted.length; i++) {
        const value = sorted[i];
        if (value === prev + 1) {
            prev = value;
            continue;
        }
        ranges.push({ start, end: prev });
        start = value;
        prev = value;
    }
    ranges.push({ start, end: prev });

    const parts = ranges
        .slice(0, maxRanges)
        .map((range) =>
            range.start === range.end ? `${range.start}` : `${range.start}–${range.end}`
        );

    if (ranges.length > maxRanges) {
        parts.push("…");
    }

    return parts.join(", ");
}

function getCellColorClass(classInfo: ConvergenceClassInfo, selected: boolean): string {
    const selectedClass = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    switch (classInfo.colorToken) {
        case "violet":
            return "border-border text-textDim bg-violet-500/25 hover:bg-violet-500/35" + selectedClass;
        case "green":
            return "border-border text-textDim bg-emerald-400/25 hover:bg-emerald-400/35" + selectedClass;
        case "greenDark":
            return "border-border text-textDim bg-emerald-700/35 hover:bg-emerald-700/45" + selectedClass;
        case "yellow":
            return "border-border text-textDim bg-yellow-300/35 hover:bg-yellow-300/45" + selectedClass;
        case "yellowDark":
            return "border-border text-textDim bg-yellow-700/35 hover:bg-yellow-700/45" + selectedClass;
        case "orange":
            return "border-border text-textDim bg-orange-400/30 hover:bg-orange-400/40" + selectedClass;
        case "orangeDark":
            return "border-border text-textDim bg-orange-700/35 hover:bg-orange-700/45" + selectedClass;
        case "red":
            return "border-border text-textDim bg-red-400/30 hover:bg-red-400/40" + selectedClass;
        case "redDark":
            return "border-border text-textDim bg-red-800/35 hover:bg-red-800/45" + selectedClass;
        case "neutral":
        default:
            return "border-border/60 text-textDim/70 bg-surface/30 hover:bg-surface/40" + selectedClass;
    }
}

export const ConvergenceMatrixTable: React.FC<ConvergenceMatrixTableProps> = ({
    experiment,
    matrix,
    maxSeries,
    maxSignChangesForOneSided,
    maxViolationsForMonotone,
    onMaxSignChangesForOneSidedChange,
    onMaxViolationsForMonotoneChange,
    selectedCell,
    onCellSelect,
}) => {
    const algoByKey = useMemo(() => new Map(matrix.algoList.map((algo) => [algo.key, algo])), [matrix.algoList]);
    const seriesByKey = useMemo(() => new Map(matrix.seriesList.map((series) => [series.key, series])), [matrix.seriesList]);
    const seriesLimitByKey = useMemo(
        () => new Map((experiment?.seriesList ?? []).map((series) => [series.id, series.limit ?? null])),
        [experiment]
    );

    const cellSummaryByKey = useMemo(() => {
        const summary = new Map<string, CellSummary>();

        for (const seriesAccel of experiment?.seriesAccelList ?? []) {
            const key = buildSeriesAccelPairKey(seriesAccel.accel_id, seriesAccel.series_id);
            const analysis = matrix.cells[key];
            if (!analysis) continue;

            const side = formatSideWithMax(analysis, maxSignChangesForOneSided);
            const monotonicity = formatMonotonicityWithMax(analysis, maxViolationsForMonotone);
            const dev = computeConvergenceDevStatsFromSeriesAccel(
                seriesAccel,
                seriesLimitByKey.get(seriesAccel.series_id) ?? null
            );
            const classInfo = getConvergenceClassInfo(side, monotonicity, dev);

            summary.set(key, {
                classInfo,
                minN: dev.minN,
                min: dev.min,
                lastMinusMin: dev.lastMinusMin,
                amplitudeOrders: dev.amplitudeOrders,
            });
        }

        return summary;
    }, [
        experiment,
        matrix.cells,
        maxSignChangesForOneSided,
        maxViolationsForMonotone,
        seriesLimitByKey,
    ]);

    const classLegendTitle = buildConvergenceClassLegendTitle(
        maxSignChangesForOneSided,
        maxViolationsForMonotone
    );

    return (
        <MatrixAlgorithmSeries
            accelList={experiment?.accelList ?? []}
            seriesList={experiment?.seriesList ?? []}
            maxColsPerPage={maxSeries && maxSeries > 0 ? maxSeries : 0}
            thClassName="px-0 py-0"
            tdClassName="px-0 py-0"
            maxBodyHeight="70vh"
            emptyFallback={<div className="text-textDim text-sm">Нет данных для отображения.</div>}
            export={{
                fileBaseName: "convergence-matrix",
                enablePng: true,
                enableXlsx: false,
            }}
            renderTitle={() => "Монотонность и направление: алгоритмы × ряды"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {matrix.algoList.length} · Ряды: {matrix.seriesList.length}
                </>
            )}
            renderHeaderRight={() => (
                <div className="flex items-center gap-3">
                    <div className="flex flex-col gap-[2px] text-[10px]">
                        <div className="flex items-center gap-1">
                            <span
                                className="whitespace-nowrap"
                                title="Максимальное число смен знака A_n - lim, при котором траектория все еще считается односторонней."
                            >
                                max sign changes:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={50}
                                value={maxSignChangesForOneSided}
                                onChange={(event) =>
                                    onMaxSignChangesForOneSidedChange(Number(event.target.value))
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
                                title="Максимальное число ростов |A_n - lim|, при котором траектория все еще считается почти монотонной."
                            >
                                max violations:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={50}
                                value={maxViolationsForMonotone}
                                onChange={(event) =>
                                    onMaxViolationsForMonotoneChange(Number(event.target.value))
                                }
                                className="h-[4px] w-28 cursor-pointer"
                            />
                            <span className="w-6 text-right tabular-nums">
                                {maxViolationsForMonotone}
                            </span>
                        </div>
                    </div>

                    <span
                        className="cursor-help rounded border border-border bg-surface px-2 py-[3px] text-[10px] text-textDim hover:bg-panel"
                        title={classLegendTitle}
                    >
                        класс
                    </span>
                </div>
            )}
            renderCell={(row, col) => {
                const algo = algoByKey.get(row.id);
                const series = seriesByKey.get(col.id);
                if (!algo || !series) return null;

                const key = buildSeriesAccelPairKey(algo.key, series.key);
                const analysis = matrix.cells[key];

                if (!analysis) {
                    return (
                        <div className="flex h-full min-h-[32px] w-full items-center justify-center text-[10px] text-textDim/50">
                            —
                        </div>
                    );
                }

                const summary = cellSummaryByKey.get(key);
                const classInfo = summary?.classInfo ?? EMPTY_CLASS_INFO;
                const isSelected =
                    selectedCell?.seriesId === series.key && selectedCell?.accelId === algo.key;
                const colorClass = getCellColorClass(classInfo, isSelected);
                const domId = getConvergenceCellDomId(algo.key, series.key);

                const signNsText =
                    analysis.signChangeNs.length > 0 ? formatIntervals(analysis.signChangeNs) : "—";
                const growthNsText =
                    analysis.violationsNs.length > 0 ? formatIntervals(analysis.violationsNs) : "—";

                const titleLines: string[] = [];
                titleLines.push(`Ряд: ${series.seriesName} (x=${series.xLabel}, prec=${series.precision})`);
                titleLines.push(`Предел ряда: ${formatComplexValue(seriesLimitByKey.get(series.key) ?? null)}`);
                titleLines.push(
                    `Алгоритм: ${algo.algorithmName}` + (algo.m != null ? `, m=${algo.m}` : "")
                );
                titleLines.push("Аргументы алгоритма:");
                const hasAlgoArgs = appendAlgorithmArgsTooltipLines(
                    titleLines,
                    algo.algorithmArgs,
                    algo.argsSummary
                );
                if (hasAlgoArgs) titleLines.push("");
                titleLines.push(`Класс: ${classInfo.label} (${classInfo.title})`);
                titleLines.push(classInfo.description);
                titleLines.push(
                    `min |A_n-lim|: ${formatDeviationValue(summary?.min ?? null)} at n=${summary?.minN ?? "—"}`
                );
                titleLines.push(
                    `last-min: ${formatDeviationValue(summary?.lastMinusMin ?? null)} | amp: ${formatAmplitudeOrders(summary?.amplitudeOrders ?? null)}`
                );
                titleLines.push(
                    `Число смен знака: ${analysis.signChangesCount}, ns: ${signNsText}`
                );
                titleLines.push(
                    `Число ростов |A_n-lim|: ${analysis.violationsCount}, ns: ${growthNsText}`
                );
                titleLines.push(`Пар (n-1,n) в анализе: ${analysis.stepsAnalyzed}`);
                titleLines.push("");
                titleLines.push("Клик — детальный график.");

                return (
                    <div
                        id={domId}
                        title={titleLines.join("\n")}
                        className={
                            "h-full min-h-[32px] w-full cursor-pointer border border-transparent " +
                            colorClass
                        }
                        onClick={() =>
                            onCellSelect({ seriesId: series.key, accelId: algo.key })
                        }
                    >
                        <div className="flex select-none flex-col items-center justify-center gap-[1px] py-[2px] leading-tight">
                            <span className="font-mono text-[10px]">{classInfo.label}</span>
                            <span className="text-[9px] text-textDim/80">
                                min@{summary?.minN ?? "—"}
                            </span>
                        </div>
                    </div>
                );
            }}
        />
    );
};
