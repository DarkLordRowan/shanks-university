import React, { useMemo } from "react";
import * as XLSX from "xlsx-js-style";
import type { Experiment, ProfilingMatrix, SelectedCell, OpsTrendType } from "../model/types";
import { buildExperimentSessionStateKey } from "@/shared/lib/inMemorySessionState";
import {
    formatOps,
    formatTrendShort,
    getProfilingCellDomId,
} from "../model/profilingUtils";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries.tsx";
import { appendAlgorithmArgsTooltipLines } from "@/shared/lib/matrixTooltip";
import { buildSeriesAccelPairKey } from "@/shared/lib/experimentIndex";
import {
    appendSheet,
    buildKeyValueSheet,
    buildSheetFromAoa,
    createWorkbook,
} from "@/shared/lib/xlsxExport";

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

function getTrendFillColor(trend: OpsTrendType): string {
    switch (trend) {
        case "constant":
            return "334155";
        case "non_decreasing":
            return "A16207";
        case "increasing":
            return "B91C1C";
        case "decreasing":
            return "047857";
        case "unknown":
        default:
            return "1F2937";
    }
}

export const ProfilingMatrixTable: React.FC<ProfilingMatrixTableProps> = ({
                                                                              experiment,
                                                                              matrix,
                                                                              maxSeries,
                                                                              selectedCell,
                                                                              onCellSelect,
                                                                          }) => {
    const filterSessionKey = buildExperimentSessionStateKey(experiment.id, "matrix-filters");
    const algoByKey = useMemo(() => new Map(matrix.algoList.map((a) => [a.key, a])), [matrix.algoList]);
    const seriesByKey = useMemo(() => new Map(matrix.seriesList.map((s) => [s.key, s])), [matrix.seriesList]);
    const buildWorkbook = useMemo(
        () =>
            ({
                accelList,
                seriesList,
            }: {
                accelList: import("@/entities/experiment/model/experiment").Accel[];
                seriesList: import("@/entities/experiment/model/experiment").Series[];
                pager: { startIndex: number; endIndex: number; totalCols: number };
            }): XLSX.WorkBook => {
                const accelIds = new Set(accelList.map((item) => item.id));
                const seriesIds = new Set(seriesList.map((item) => item.id));
                const filteredAlgoList = matrix.algoList.filter((item) => accelIds.has(item.key));
                const filteredSeriesList = matrix.seriesList.filter((item) => seriesIds.has(item.key));
                const workbook = createWorkbook("Profiling matrix", "Profiling matrix export");

                appendSheet(
                    workbook,
                    buildKeyValueSheet([
                        { key: "algorithms", value: accelList.length },
                        { key: "series", value: seriesList.length },
                        { key: "cells", value: Object.keys(matrix.cells).length },
                        {
                            key: "selected cell",
                            value: selectedCell
                                ? `${selectedCell.accelId} × ${selectedCell.seriesId}`
                                : "none",
                        },
                    ]),
                    "overview"
                );

                const summaryRows: Array<Array<string | number | boolean | null>> = [
                    [
                        "algorithm",
                        "m",
                        "args",
                        "series",
                        "x",
                        "precision",
                        "trend",
                        "max total ops",
                        "steps",
                        "decrease ns",
                    ],
                ];

                for (const algo of filteredAlgoList) {
                    for (const series of filteredSeriesList) {
                        const analysis = matrix.cells[buildSeriesAccelPairKey(algo.key, series.key)];
                        if (!analysis) continue;
                        summaryRows.push([
                            algo.algorithmName,
                            algo.m,
                            algo.argsSummary || null,
                            series.seriesName,
                            series.xLabel,
                            series.precision,
                            analysis.trendTotal,
                            analysis.maxTotalOps,
                            analysis.stepsAnalyzed,
                            analysis.decreaseNsTotal.join(", "),
                        ]);
                    }
                }

                appendSheet(
                    workbook,
                    buildSheetFromAoa(summaryRows, {
                        cols: [
                            { wch: 24 },
                            { wch: 8 },
                            { wch: 28 },
                            { wch: 24 },
                            { wch: 10 },
                            { wch: 12 },
                            { wch: 16 },
                            { wch: 16 },
                            { wch: 10 },
                            { wch: 24 },
                        ],
                        headerRows: 1,
                        rowHeaderCols: 1,
                        decorateCell: ({ rowIndex, colIndex, cell }) => {
                            if (rowIndex === 0) return;
                            if ([1, 7, 8].includes(colIndex)) cell.z = "0";
                        },
                    }),
                    "summary"
                );

                const matrixSheetRows: Array<Array<string | number | boolean | null>> = [
                    [
                        "algorithm \\ series",
                        ...filteredSeriesList.map(
                            (series) =>
                                `${series.seriesName}\nx=${series.xLabel}\nprec=${series.precision}`
                        ),
                    ],
                    ...filteredAlgoList.map((algo) => [
                        `${algo.algorithmName}${algo.m != null ? `\nm=${algo.m}` : ""}${
                            algo.argsSummary ? `\n${algo.argsSummary}` : ""
                        }`,
                        ...filteredSeriesList.map((series) => {
                            const analysis =
                                matrix.cells[buildSeriesAccelPairKey(algo.key, series.key)];
                            if (!analysis) return "—";
                            return `${formatTrendShort(analysis.trendTotal)}\n${analysis.maxTotalOps != null ? formatOps(analysis.maxTotalOps) : "—"}`;
                        }),
                    ]),
                ];

                appendSheet(
                    workbook,
                    buildSheetFromAoa(matrixSheetRows, {
                        cols: [{ wch: 34 }, ...filteredSeriesList.map(() => ({ wch: 18 }))],
                        rows: [{ hpt: 40 }, ...filteredAlgoList.map(() => ({ hpt: 34 }))],
                        headerRows: 1,
                        rowHeaderCols: 1,
                        decorateCell: ({ rowIndex, colIndex, cell }) => {
                            if (rowIndex === 0 || colIndex === 0) return;
                            const algo = filteredAlgoList[rowIndex - 1];
                            const series = filteredSeriesList[colIndex - 1];
                            if (!algo || !series) return;
                            const analysis =
                                matrix.cells[buildSeriesAccelPairKey(algo.key, series.key)];
                            if (!analysis) return;
                            cell.s = {
                                fill: {
                                    patternType: "solid",
                                    fgColor: { rgb: getTrendFillColor(analysis.trendTotal) },
                                },
                                font: { color: { rgb: "F9FAFB" }, bold: true },
                                alignment: { horizontal: "center", vertical: "center", wrapText: true },
                                border: {
                                    top: { style: "thin", color: { rgb: "374151" } },
                                    bottom: { style: "thin", color: { rgb: "374151" } },
                                    left: { style: "thin", color: { rgb: "374151" } },
                                    right: { style: "thin", color: { rgb: "374151" } },
                                },
                            };
                        },
                    }),
                    "matrix"
                );

                return workbook;
            },
        [matrix.algoList, matrix.cells, matrix.seriesList, selectedCell]
    );

    return (
        <MatrixAlgorithmSeries
            accelList={experiment?.accelList ?? []}
            seriesList={experiment?.seriesList ?? []}
            filterSessionKey={filterSessionKey}
            maxColsPerPage={maxSeries && maxSeries > 0 ? maxSeries : 0}
            thClassName="px-0 py-0"
            tdClassName="px-0 py-0"
            minCellHeightPx={48}
            emptyFallback={<div className="text-textDim text-sm">Нет данных для отображения.</div>}
            export={{
                fileBaseName: "profiling-matrix",
                enablePng: true,
                enableXlsx: true,
                buildWorkbook,
            }}
            renderTitle={() => "Profiling: операции (алгоритмы × ряды)"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {matrix.algoList.length} · Ряды: {matrix.seriesList.length}
                </>
            )}
            renderCell={(row, col) => {
                const algo = algoByKey.get(row.id);
                const s = seriesByKey.get(col.id);
                if (!algo || !s) return null;

                const key = buildSeriesAccelPairKey(algo.key, s.key);
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
                appendAlgorithmArgsTooltipLines(titleLines, algo.algorithmArgs, algo.argsSummary);
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
