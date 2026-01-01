import React, { useState } from "react";
import {
    type ConvergenceAnalysis,
    type ConvergenceMatrix,
    type Experiment,
    type MonotonicityType,
    type SelectedCell,
    type SideType,
} from "../model/types";
import {
    analyzeSeriesAccelConvergence,
    formatMonotonicityShort,
    formatSideShort,
    getConvergenceCellDomId,
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
    maxSeries,
    selectedCell,
    onCellSelect,
}) => {
    const [maxSignChangesForOneSided, setMaxSignChangesForOneSided] = useState<number>(0);
    const [maxViolationsForMonotone, setMaxViolationsForMonotone] = useState<number>(0);

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
                    Алгоритмы: {experiment.accelList.length} · Ряды: {experiment.seriesList.length}
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
            renderCell={(algo, series) => {
                const seriesAccelData = experiment.seriesAccelList.find(
                    (item) => item.series_id === series.id && item.accel_id === algo.id
                );

                if (!series || !algo || !seriesAccelData) {
                    return (
                        <div className="w-full h-full min-h-[32px] flex items-center justify-center text-[10px] text-textDim/50">
                            —
                        </div>
                    );
                }

                const analysis: ConvergenceAnalysis = analyzeSeriesAccelConvergence(
                    series,
                    algo,
                    seriesAccelData,
                    maxSignChangesForOneSided,
                    maxViolationsForMonotone
                );

                const sideShort = formatSideShort(analysis.side);
                const monShort = formatMonotonicityShort(analysis.monotonicity);

                const isSelected =
                    selectedCell?.seriesId === series.id && selectedCell?.accelId === algo.id;

                const colorClass = getCellColorClass(
                    analysis.side,
                    analysis.monotonicity,
                    isSelected
                );
                const domId = getConvergenceCellDomId(algo.id, series.id);

                return (
                    <div
                        id={domId}
                        className={
                            "w-full h-full min-h-[32px] cursor-pointer border border-transparent " +
                            colorClass
                        }
                        onClick={() => onCellSelect({ seriesId: series.id, accelId: algo.id })}
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
