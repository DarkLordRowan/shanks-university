import React, { useMemo } from "react";
import * as XLSX from "xlsx-js-style";
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
    buildConvergenceDetailPoints,
    computeSeriesAlgoAmplitudeOrders,
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
import {
    ALGO_SERIES_CONVERGENCE_TABLE_DOCS,
    getAlgoSeriesConvergenceColumnAnchorId,
} from "@/shared/lib/docs/tableDocs";
import { DocsAnchorButton } from "@/shared/ui/docs/DocsAnchorButton";
import {
    appendSheet,
    buildKeyValueSheet,
    buildSheetFromAoa,
    createWorkbook,
} from "@/shared/lib/xlsxExport";

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
    last: number | null;
    lastN: number | null;
    lastMinusMin: number | null;
    amplitudeOrders: number | null;
    maxAmplitudeOrders: number | null;
    seriesAlgoAmplitudeOrders: number | null;
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
    maxAmplitudeOrders: null,
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

function getClassFillColor(colorToken: ConvergenceClassInfo["colorToken"]): string {
    switch (colorToken) {
        case "violet":
            return "6D28D9";
        case "green":
            return "047857";
        case "greenDark":
            return "065F46";
        case "yellow":
            return "A16207";
        case "yellowDark":
            return "854D0E";
        case "orange":
            return "C2410C";
        case "orangeDark":
            return "9A3412";
        case "red":
            return "B91C1C";
        case "redDark":
            return "7F1D1D";
        case "neutral":
        default:
            return "1F2937";
    }
}

function formatSignedError(point: { err: number | null; sign: -1 | 0 | 1 | null }): number | null {
    if (point.err == null || !Number.isFinite(point.err)) return null;
    if (point.sign == null || point.sign === 0) return point.err;
    return point.err * point.sign;
}

function getExportSideLabel(classInfo: ConvergenceClassInfo): string {
    if (classInfo.kind === "unknown") return "?";
    if (classInfo.kind === "static") return "1s";
    return classInfo.kind.endsWith("_two_sided") ? "2s" : "1s";
}

function getSeriesMinDeviation(series: Experiment["seriesList"][number] | null | undefined): number | null {
    let min: number | null = null;

    for (const point of series?.computed ?? []) {
        const deviation = point.deviation;
        if (deviation == null || !Number.isFinite(deviation)) continue;

        const absDeviation = Math.abs(deviation);
        if (min == null || absDeviation < min) {
            min = absDeviation;
        }
    }

    return min;
}

interface ExportRow {
    algorithmKey: string;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    seriesKey: string;
    seriesName: string;
    xLabel: string;
    precision: string;
    limitText: string;
    classInfo: ConvergenceClassInfo;
    sideLabel: string;
    steps: number;
    signChanges: number;
    violations: number;
    min: number | null;
    minN: number | null;
    last: number | null;
    lastN: number | null;
    lastMinusMin: number | null;
    amplitudeOrders: number | null;
    maxAmplitudeOrders: number | null;
    seriesAlgoAmplitudeOrders: number | null;
}

function buildOverviewSheet(args: {
    algoCount: number;
    seriesCount: number;
    pairCount: number;
    maxSignChangesForOneSided: number;
    maxViolationsForMonotone: number;
    selectedCell: SelectedCell | null;
}): XLSX.WorkSheet {
    return buildKeyValueSheet([
        { key: "algorithms", value: args.algoCount },
        { key: "series", value: args.seriesCount },
        { key: "pairs", value: args.pairCount },
        { key: "max sign changes", value: args.maxSignChangesForOneSided },
        { key: "max violations", value: args.maxViolationsForMonotone },
        {
            key: "selected cell",
            value: args.selectedCell
                ? `${args.selectedCell.accelId} × ${args.selectedCell.seriesId}`
                : "none",
        },
    ]);
}

function buildSummarySheet(rows: ExportRow[]): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | boolean | null>> = [
        [
            "algorithm",
            "m",
            "args",
            "series",
            "x",
            "precision",
            "limit",
            "side",
            "class",
            "class title",
            "steps",
            "sign changes",
            "violations",
            "min |A_n-lim|",
            "min n",
            "last |A_n-lim|",
            "last n",
            "last - min",
            "last/min amp",
            "max/min amp",
            "series/algo amp",
        ],
        ...rows.map((row) => [
            row.algorithmName,
            row.m,
            row.argsSummary || null,
            row.seriesName,
            row.xLabel,
            row.precision,
            row.limitText,
            row.sideLabel,
            row.classInfo.symbol,
            row.classInfo.title,
            row.steps,
            row.signChanges,
            row.violations,
            row.min,
            row.minN,
            row.last,
            row.lastN,
            row.lastMinusMin,
            row.amplitudeOrders,
            row.maxAmplitudeOrders,
            row.seriesAlgoAmplitudeOrders,
        ]),
    ];

    return buildSheetFromAoa(aoa, {
        cols: [
            { wch: 24 },
            { wch: 8 },
            { wch: 28 },
            { wch: 24 },
            { wch: 10 },
            { wch: 12 },
            { wch: 20 },
            { wch: 8 },
            { wch: 12 },
            { wch: 18 },
            { wch: 10 },
            { wch: 14 },
            { wch: 12 },
            { wch: 16 },
            { wch: 10 },
            { wch: 16 },
            { wch: 10 },
            { wch: 16 },
            { wch: 12 },
            { wch: 12 },
            { wch: 14 },
        ],
        headerRows: 1,
        rowHeaderCols: 1,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0) return;
            if ([1, 10, 11, 12, 14, 16].includes(colIndex)) cell.z = "0";
            if ([13, 15, 17].includes(colIndex)) cell.z = "0.000E+00";
            if ([18, 19, 20].includes(colIndex)) cell.z = "0.00";
        },
    });
}

function buildMatrixSheet(args: {
    algoList: Array<{ key: string; algorithmName: string; m: number | null; argsSummary: string }>;
    seriesList: Array<{ key: string; seriesName: string; xLabel: string; precision: string }>;
    cellSummaryByKey: Map<string, CellSummary>;
}): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | boolean | null>> = [
        [
            "algorithm \\ series",
            ...args.seriesList.map(
                (series) => `${series.seriesName}\nx=${series.xLabel}\nprec=${series.precision}`
            ),
        ],
    ];

    for (const algo of args.algoList) {
        aoa.push([
            `${algo.algorithmName}${algo.m != null ? `\nm=${algo.m}` : ""}${
                algo.argsSummary ? `\n${algo.argsSummary}` : ""
            }`,
            ...args.seriesList.map((series) => {
                const summary = args.cellSummaryByKey.get(buildSeriesAccelPairKey(algo.key, series.key));
                if (!summary) return "—";
                return `${summary.classInfo.label}\nmin@${summary.minN ?? "—"}`;
            }),
        ]);
    }

    return buildSheetFromAoa(aoa, {
        cols: [{ wch: 34 }, ...args.seriesList.map(() => ({ wch: 18 }))],
        rows: [{ hpt: 42 }, ...args.algoList.map(() => ({ hpt: 34 }))],
        headerRows: 1,
        rowHeaderCols: 1,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0 || colIndex === 0) return;
            const algo = args.algoList[rowIndex - 1];
            const series = args.seriesList[colIndex - 1];
            if (!algo || !series) return;

            const summary = args.cellSummaryByKey.get(buildSeriesAccelPairKey(algo.key, series.key));
            if (!summary) return;

            cell.s = {
                fill: { patternType: "solid", fgColor: { rgb: getClassFillColor(summary.classInfo.colorToken) } },
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
    });
}

function buildSelectedMetaSheet(args: {
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    seriesName: string;
    xLabel: string;
    precision: string;
    limitText: string;
    classInfo: ConvergenceClassInfo;
    sideLabel: string;
    steps: number;
    signChanges: number;
    violations: number;
    min: number | null;
    minN: number | null;
    last: number | null;
    lastN: number | null;
    lastMinusMin: number | null;
    amplitudeOrders: number | null;
    maxAmplitudeOrders: number | null;
    seriesAlgoAmplitudeOrders: number | null;
}): XLSX.WorkSheet {
    return buildKeyValueSheet([
        { key: "algorithm", value: args.algorithmName },
        { key: "m", value: args.m },
        { key: "args", value: args.argsSummary || null },
        { key: "series", value: args.seriesName },
        { key: "x", value: args.xLabel },
        { key: "precision", value: args.precision },
        { key: "limit", value: args.limitText },
        { key: "side", value: args.sideLabel },
        { key: "class", value: args.classInfo.symbol },
        { key: "class title", value: args.classInfo.title },
        { key: "class description", value: args.classInfo.description },
        { key: "steps", value: args.steps },
        { key: "sign changes", value: args.signChanges },
        { key: "violations", value: args.violations },
        { key: "min |A_n-lim|", value: args.min },
        { key: "min n", value: args.minN },
        { key: "last |A_n-lim|", value: args.last },
        { key: "last n", value: args.lastN },
        { key: "last - min", value: args.lastMinusMin },
        { key: "last/min amp", value: args.amplitudeOrders },
        { key: "max/min amp", value: args.maxAmplitudeOrders },
        { key: "series/algo amp", value: args.seriesAlgoAmplitudeOrders },
    ]);
}

function buildPointsSheet(
    points: ReturnType<typeof buildConvergenceDetailPoints>
): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | boolean | null>> = [
        ["n", "Re(A_n)", "Im(A_n)", "|A_n-lim|", "sgn*|A_n-lim|", "sgn(Re(A_n-lim))"],
        ...points.map((point) => [
            point.n,
            point.valueRe,
            point.valueIm,
            point.err,
            formatSignedError(point),
            point.sign,
        ]),
    ];

    return buildSheetFromAoa(aoa, {
        cols: [
            { wch: 10 },
            { wch: 18 },
            { wch: 18 },
            { wch: 16 },
            { wch: 16 },
            { wch: 16 },
        ],
        headerRows: 1,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0) return;
            if ([0, 5].includes(colIndex)) cell.z = "0";
            if ([1, 2, 3, 4].includes(colIndex)) cell.z = "0.000E+00";
        },
    });
}

function buildDiffsSheet(
    points: ReturnType<typeof buildConvergenceDetailPoints>
): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | boolean | null>> = [
        ["n", "Re(A_n-A_{n-1})", "Im(A_n-A_{n-1})", "|A_n-A_{n-1}|"],
        ...points
            .filter((point) => point.diffNorm != null)
            .map((point) => [point.n, point.diffRe, point.diffIm, point.diffNorm]),
    ];

    return buildSheetFromAoa(aoa, {
        cols: [{ wch: 10 }, { wch: 18 }, { wch: 18 }, { wch: 18 }],
        headerRows: 1,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0) return;
            if (colIndex === 0) cell.z = "0";
            if ([1, 2, 3].includes(colIndex)) cell.z = "0.000E+00";
        },
    });
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
    const seriesMinByKey = useMemo(
        () => new Map((experiment?.seriesList ?? []).map((series) => [series.id, getSeriesMinDeviation(series)])),
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
                last: dev.last,
                lastN: dev.lastN,
                lastMinusMin: dev.lastMinusMin,
                amplitudeOrders: dev.amplitudeOrders,
                maxAmplitudeOrders: dev.maxAmplitudeOrders,
                seriesAlgoAmplitudeOrders: computeSeriesAlgoAmplitudeOrders(
                    seriesMinByKey.get(seriesAccel.series_id) ?? null,
                    dev.min
                ),
            });
        }

        return summary;
    }, [
        experiment,
        matrix.cells,
        maxSignChangesForOneSided,
        maxViolationsForMonotone,
        seriesMinByKey,
        seriesLimitByKey,
    ]);

    const classLegendTitle = buildConvergenceClassLegendTitle(
        maxSignChangesForOneSided,
        maxViolationsForMonotone
    );

    const exportRows = useMemo<ExportRow[]>(() => {
        const rows: ExportRow[] = [];

        for (const seriesAccel of experiment?.seriesAccelList ?? []) {
            const algo = algoByKey.get(seriesAccel.accel_id);
            const series = seriesByKey.get(seriesAccel.series_id);
            const analysis = matrix.cells[buildSeriesAccelPairKey(seriesAccel.accel_id, seriesAccel.series_id)];
            const summary = cellSummaryByKey.get(
                buildSeriesAccelPairKey(seriesAccel.accel_id, seriesAccel.series_id)
            );

            if (!algo || !series || !analysis || !summary) continue;

            rows.push({
                algorithmKey: algo.key,
                algorithmName: algo.algorithmName,
                m: algo.m,
                argsSummary: algo.argsSummary,
                seriesKey: series.key,
                seriesName: series.seriesName,
                xLabel: series.xLabel,
                precision: series.precision,
                limitText: formatComplexValue(seriesLimitByKey.get(series.key) ?? null),
                classInfo: summary.classInfo,
                sideLabel: getExportSideLabel(summary.classInfo),
                steps: analysis.stepsAnalyzed,
                signChanges: analysis.signChangesCount,
                violations: analysis.violationsCount,
                min: summary.min,
                minN: summary.minN,
                last: summary.last,
                lastN: summary.lastN,
                lastMinusMin: summary.lastMinusMin,
                amplitudeOrders: summary.amplitudeOrders,
                maxAmplitudeOrders: summary.maxAmplitudeOrders,
                seriesAlgoAmplitudeOrders: summary.seriesAlgoAmplitudeOrders,
            });
        }

        rows.sort((left, right) => {
            const byAlgo = left.algorithmName.localeCompare(right.algorithmName);
            if (byAlgo !== 0) return byAlgo;
            const byM = (left.m ?? 0) - (right.m ?? 0);
            if (byM !== 0) return byM;
            const bySeries = left.seriesName.localeCompare(right.seriesName);
            if (bySeries !== 0) return bySeries;
            return left.xLabel.localeCompare(right.xLabel);
        });

        return rows;
    }, [algoByKey, cellSummaryByKey, experiment, matrix.cells, seriesByKey, seriesLimitByKey]);

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
                const filteredExportRows = exportRows.filter(
                    (row) => accelIds.has(row.algorithmKey) && seriesIds.has(row.seriesKey)
                );

                const workbook = createWorkbook(
                    "Algorithm × series convergence",
                    "Algorithm × series convergence export"
                );

                appendSheet(
                    workbook,
                    buildOverviewSheet({
                        algoCount: accelList.length,
                        seriesCount: seriesList.length,
                        pairCount: filteredExportRows.length,
                        maxSignChangesForOneSided,
                        maxViolationsForMonotone,
                        selectedCell,
                    }),
                    "overview"
                );
                appendSheet(workbook, buildSummarySheet(filteredExportRows), "summary");
                appendSheet(
                    workbook,
                    buildMatrixSheet({
                        algoList: filteredAlgoList,
                        seriesList: filteredSeriesList,
                        cellSummaryByKey,
                    }),
                    "matrix"
                );

                if (selectedCell) {
                    const selectedAlgo = algoByKey.get(selectedCell.accelId);
                    const selectedSeries = seriesByKey.get(selectedCell.seriesId);
                    const selectedAnalysis = matrix.cells[
                        buildSeriesAccelPairKey(selectedCell.accelId, selectedCell.seriesId)
                    ];
                    const selectedSummary = cellSummaryByKey.get(
                        buildSeriesAccelPairKey(selectedCell.accelId, selectedCell.seriesId)
                    );
                    const selectedSeriesAccel =
                        experiment.seriesAccelList.find(
                            (item) =>
                                item.accel_id === selectedCell.accelId &&
                                item.series_id === selectedCell.seriesId
                        ) ?? null;

                    if (
                        selectedAlgo &&
                        selectedSeries &&
                        selectedAnalysis &&
                        selectedSummary &&
                        selectedSeriesAccel
                    ) {
                        const points = buildConvergenceDetailPoints(
                            selectedSeriesAccel,
                            seriesLimitByKey.get(selectedSeries.key) ?? null
                        );

                        appendSheet(
                            workbook,
                            buildSelectedMetaSheet({
                                algorithmName: selectedAlgo.algorithmName,
                                m: selectedAlgo.m,
                                argsSummary: selectedAlgo.argsSummary,
                                seriesName: selectedSeries.seriesName,
                                xLabel: selectedSeries.xLabel,
                                precision: selectedSeries.precision,
                                limitText: formatComplexValue(
                                    seriesLimitByKey.get(selectedSeries.key) ?? null
                                ),
                                classInfo: selectedSummary.classInfo,
                                sideLabel: getExportSideLabel(selectedSummary.classInfo),
                                steps: selectedAnalysis.stepsAnalyzed,
                                signChanges: selectedAnalysis.signChangesCount,
                                violations: selectedAnalysis.violationsCount,
                                min: selectedSummary.min,
                                minN: selectedSummary.minN,
                                last: selectedSummary.last,
                                lastN: selectedSummary.lastN,
                                lastMinusMin: selectedSummary.lastMinusMin,
                                amplitudeOrders: selectedSummary.amplitudeOrders,
                                maxAmplitudeOrders: selectedSummary.maxAmplitudeOrders,
                                seriesAlgoAmplitudeOrders: selectedSummary.seriesAlgoAmplitudeOrders,
                            }),
                            "selected_meta"
                        );
                        appendSheet(workbook, buildPointsSheet(points), "selected_points");
                        appendSheet(workbook, buildDiffsSheet(points), "selected_diffs");
                    }
                }

                return workbook;
            },
        [
            algoByKey,
            cellSummaryByKey,
            experiment.seriesAccelList,
            exportRows,
            matrix.algoList,
            matrix.cells,
            matrix.seriesList,
            maxSignChangesForOneSided,
            maxViolationsForMonotone,
            selectedCell,
            seriesByKey,
            seriesLimitByKey,
        ]
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
                enableXlsx: true,
                buildWorkbook,
            }}
            renderTitle={() => (
                <span className="group inline-flex items-center gap-2">
                    <span>Монотонность и направление: алгоритмы × ряды</span>
                    <DocsAnchorButton
                        anchorId={ALGO_SERIES_CONVERGENCE_TABLE_DOCS.id}
                        label={ALGO_SERIES_CONVERGENCE_TABLE_DOCS.title}
                    />
                </span>
            )}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {matrix.algoList.length} · Ряды: {matrix.seriesList.length}
                </>
            )}
            renderHeaderRight={() => (
                <div className="flex items-center gap-3">
                    <div className="group relative rounded border border-border/70 bg-surface/50 px-2 py-1">
                        <div className="flex flex-col gap-[2px] pr-6 text-[10px]">
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

                        <DocsAnchorButton
                            anchorId={getAlgoSeriesConvergenceColumnAnchorId("thresholds")}
                            label={`${ALGO_SERIES_CONVERGENCE_TABLE_DOCS.title}: пороги классификации`}
                            className="absolute right-1 top-1"
                        />
                    </div>

                    <span className="group relative inline-flex items-center rounded border border-border bg-surface px-2 py-[3px] pr-7 text-[10px] text-textDim">
                        ячейка
                        <DocsAnchorButton
                            anchorId={getAlgoSeriesConvergenceColumnAnchorId("cell")}
                            label={`${ALGO_SERIES_CONVERGENCE_TABLE_DOCS.title}: ячейка матрицы`}
                            className="absolute right-1 top-1"
                        />
                    </span>

                    <span className="group relative inline-flex items-center rounded border border-border bg-surface px-2 py-[3px] pr-7 text-[10px] text-textDim hover:bg-panel">
                        <span className="cursor-help" title={classLegendTitle}>
                            класс
                        </span>

                        <DocsAnchorButton
                            anchorId={getAlgoSeriesConvergenceColumnAnchorId("class")}
                            label={`${ALGO_SERIES_CONVERGENCE_TABLE_DOCS.title}: класс`}
                            className="absolute right-1 top-1"
                        />
                    </span>
                </div>
            )}
            renderCorner={() => (
                <span className="group inline-flex items-center gap-2 text-left">
                    <span>Алгоритм \ Ряд</span>
                    <DocsAnchorButton
                        anchorId={getAlgoSeriesConvergenceColumnAnchorId("axes")}
                        label={`${ALGO_SERIES_CONVERGENCE_TABLE_DOCS.title}: оси матрицы`}
                    />
                </span>
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
                    `last-min: ${formatDeviationValue(summary?.lastMinusMin ?? null)} | last/min amp: ${formatAmplitudeOrders(summary?.amplitudeOrders ?? null)} | max/min amp: ${formatAmplitudeOrders(summary?.maxAmplitudeOrders ?? null)} | series/algo amp: ${formatAmplitudeOrders(summary?.seriesAlgoAmplitudeOrders ?? null)}`
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
