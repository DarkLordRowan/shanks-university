import React from "react";
import type { Experiment, ProfilingMatrix, SelectedCell, OpsTrendType } from "../model/types";
import {
    formatOps,
    formatTrendShort,
    getProfilingCellDomId,
    nonNullEntries,
} from "../model/profilingUtils";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries.tsx";

interface ProfilingMatrixTableProps {
    experiment: Experiment;
    matrix: ProfilingMatrix;
    maxSeries?: number;
    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
}

function getCellColorClass(trend: OpsTrendType, selected: boolean): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (trend === "unknown") return "border-border/60 text-textDim/70 bg-surface/30 hover:bg-surface/40" + sel;
    if (trend === "constant") return "border-border text-textDim bg-slate-500/20 hover:bg-slate-500/30" + sel;
    if (trend === "non_decreasing")
        return "border-border text-textDim bg-yellow-500/30 hover:bg-yellow-500/40" + sel;
    if (trend === "increasing")
        return "border-border text-textDim bg-red-500/30 hover:bg-red-500/40" + sel;

    // decreasing_error
    return "border-border text-textDim bg-emerald-500/25 hover:bg-emerald-500/35" + sel;
}

export const ProfilingMatrixTable: React.FC<ProfilingMatrixTableProps> = ({
                                                                              experiment,
                                                                              matrix,
                                                                              maxSeries,
                                                                              selectedCell,
                                                                              onCellSelect,
                                                                          }) => {
    return (
        <MatrixAlgorithmSeries
            accelList={experiment?.accelList ?? []}
            seriesList={experiment?.seriesList ?? []}
            maxColsPerPage={maxSeries && maxSeries > 0 ? maxSeries : 0}
            thClassName="px-0 py-0"
            tdClassName="px-0 py-0"
            minCellHeightPx={48}
            emptyFallback={<div className="text-textDim text-sm">Нет данных для отображения.</div>}
            export={{
                fileBaseName: "profiling-matrix",
                enablePng: true,
                enableXlsx: true,
            }}
            renderTitle={() => "Profiling: операции (алгоритмы × ряды)"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {matrix.algoList.length} · Ряды: {matrix.seriesList.length}
                </>
            )}
            renderCell={(row, col) => {
                const algo = matrix.algoList.find((a) => a.key === row.id);
                const s = matrix.seriesList.find((x) => x.key === col.id);
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

                const trendShort = formatTrendShort(analysis.trendTotal);
                const maxText =
                    analysis.maxTotalOps != null
                        ? `${formatOps(analysis.maxTotalOps)}`
                        : "∅";

                const isSelected = selectedCell?.seriesId === s.key && selectedCell?.accelId === algo.key;
                const colorClass = getCellColorClass(analysis.trendTotal, isSelected);
                const domId = getProfilingCellDomId(algo.key, s.key);

                const titleLines: string[] = [];
                titleLines.push(`Ряд: ${s.seriesName} (x=${s.xLabel}, prec=${s.precision})`);
                titleLines.push(
                    `Алгоритм: ${algo.algorithmName}` + (algo.m != null ? `, m=${algo.m}` : "")
                );
                titleLines.push("Аргументы алгоритма:");
                const algoEntries = nonNullEntries(algo.algorithmArgs);
                for (const [k, v] of algoEntries.sort(([a], [b]) => a.localeCompare(b))) {
                    titleLines.push(`  ${k} = ${String(v)}`);
                }
                if (algo.argsSummary) titleLines.push(`  (${String(algo.argsSummary)})`);
                titleLines.push("");
                titleLines.push(`Trend(T): ${analysis.trendTotal}`);
                titleLines.push(`Max(T): ${maxText}`);
                if (analysis.decreaseNsTotal.length) {
                    titleLines.push(`decrease ns: ${analysis.decreaseNsTotal.slice(0, 20).join(", ")}${analysis.decreaseNsTotal.length > 20 ? ", …" : ""}`);
                }
                titleLines.push(`k (pairs): ${analysis.stepsAnalyzed}`);
                titleLines.push("");
                titleLines.push("Клик — детально.");

                return (
                    <div
                        id={domId}
                        title={titleLines.join("\n")}
                        className={
                            "w-full h-full min-h-[32px] cursor-pointer border border-transparent " + colorClass
                        }
                        onClick={() => onCellSelect({ seriesId: s.key, accelId: algo.key })}
                    >
                        <div className="flex select-none flex-col items-center justify-center gap-[1px] leading-tight py-[2px]">
                            <span className="font-mono text-[10px]">
                                {trendShort}
                            </span>
                            <span className="font-mono text-[10px]">
                                {maxText}
                            </span>
                            <span className="text-[9px] text-textDim/80">k: {analysis.stepsAnalyzed}</span>
                        </div>
                    </div>
                );
            }}
        />
    );
};
