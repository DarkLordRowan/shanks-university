import React, { useEffect, useState } from "react";
import {
    type ConvergenceMatrix,
    type Experiment,
    type MonotonicityType,
    type SelectedCell,
    type SideType,
} from "../model/types";
import {
    formatMonotonicityShort,
    formatMonotonicityWithMax,
    formatSideShort,
    formatSideWithMax,
    getConvergenceCellDomId,
    nonNullEntries,
} from "../model/convergenceUtils";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries.tsx";

interface ConvergenceMatrixTableProps {
    experiment: Experiment;
    matrix: ConvergenceMatrix;
    maxSeries?: number;
    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
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

    if (side === "unknown") return "недостаточно данных";
    if (mon === "unknown") return "недостаточно данных";

    return "недостаточно данных";
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

function getCellColorClass(side: SideType, mon: MonotonicityType, selected: boolean): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (side === "unknown" || mon === "unknown") {
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

export const ConvergenceMatrixTable: React.FC<ConvergenceMatrixTableProps> = ({
    experiment,
    matrix,
    maxSeries,
    selectedCell,
    onCellSelect,
}) => {
    const [maxSignChangesForOneSided, setMaxSignChangesForOneSided] = useState<number>(0);
    const [maxViolationsForMonotone, setMaxViolationsForMonotone] = useState<number>(0);

    useEffect(() => {
        setMaxSignChangesForOneSided(0);
        setMaxViolationsForMonotone(0);
    }, [experiment]);

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
                                title="Максимальное количество смен знака разности Aₖ - lim, при котором приближение считается односторонним. Если смен знака больше этого значения, то считается двухсторонним."
                            >
                                max sign changes:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={50}
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
                                title="Максимальное количество увеличений ошибки |Aₖ - lim|, при котором приближение всё ещё считается монотонным (неубывающим или невозрастающим). Если увеличений больше, то ошибка считается случайной (random)."
                            >
                                max violations:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={50}
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
                </div>
            )}
            renderCell={(row, col) => {
                const algo = matrix.algoList.find((a) => a.key === row.id);
                const s = matrix.seriesList.find((s) => s.key === col.id);
                if (!algo || !s) return null;

                const key = `${algo.key}::${s.key}`;
                const analysis = matrix.cells[key];

                if (!analysis) {
                    return (
                        <div className="w-full h-full min-h-[32px] flex items-center justify-center text-[10px] text-textDim/50">
                            —
                        </div>
                    );
                }

                const side = formatSideWithMax(analysis, maxSignChangesForOneSided);
                const monotonicity = formatMonotonicityWithMax(analysis, maxViolationsForMonotone);

                const sideShort = formatSideShort(side);

                const monShort = formatMonotonicityShort(monotonicity);

                const isSelected =
                    selectedCell?.seriesId === s.key && selectedCell?.accelId === algo.key;

                const colorClass = getCellColorClass(side, monotonicity, isSelected);
                const domId = getConvergenceCellDomId(algo.key, s.key);

                const titleLines: string[] = [];
                titleLines.push(`Ряд: ${s.seriesName} (x=${s.xLabel}, prec=${s.precision})`);
                titleLines.push(
                    `Алгоритм: ${algo.algorithmName}` + (algo.m != null ? `, m=${algo.m}` : "")
                );
                titleLines.push("Аргументы алгоритма:");

                const algoEntries = nonNullEntries(algo.algorithmArgs);
                if (algoEntries.length > 0) {
                    for (const [k, v] of algoEntries.sort(([a, b]) => a.localeCompare(b))) {
                        titleLines.push(`  ${k} = ${String(v)}`);
                    }
                }
                if (algo.argsSummary) titleLines.push(`  (${algo.argsSummary})`);
                if (algoEntries.length > 0 || algo.argsSummary) titleLines.push("");

                titleLines.push(`Класс: ${describeClass(side, monotonicity)}`);

                const signNsText =
                    analysis.signChangeNs && analysis.signChangeNs.length > 0
                        ? formatIntervals(analysis.signChangeNs)
                        : "—";

                const growthNsText =
                    analysis.violationsNs && analysis.violationsNs.length > 0
                        ? formatIntervals(analysis.violationsNs)
                        : "—";

                titleLines.push(
                    `Число смен знака: ${analysis.signChangesCount}, ns: ${signNsText}`
                );
                titleLines.push(
                    `Число расхождений |Aₙ−lim|: ${analysis.violationsCount}, ns: ${growthNsText}`
                );
                titleLines.push(`Пар (n−1,n) в анализе: ${analysis.stepsAnalyzed}`);
                titleLines.push("");
                titleLines.push("Клик — детальный график.");

                return (
                    <div
                        id={domId}
                        title={titleLines.join("\n")}
                        className={
                            "w-full h-full min-h-[32px] cursor-pointer border border-transparent " +
                            colorClass
                        }
                        onClick={() => onCellSelect({ seriesId: s.key, accelId: algo.key })}
                    >
                        <div className="flex select-none flex-col items-center justify-center gap-[1px] leading-tight py-[2px]">
                            <span className="font-mono text-[10px]">
                                {sideShort} | {monShort}
                            </span>
                            <span className="text-[9px] text-textDim/80">
                                k: {analysis.stepsAnalyzed}
                            </span>
                        </div>
                    </div>
                );
            }}
        />
    );
};
