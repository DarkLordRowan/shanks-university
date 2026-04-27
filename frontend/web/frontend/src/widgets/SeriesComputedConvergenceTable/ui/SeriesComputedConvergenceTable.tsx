import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import * as XLSX from "xlsx-js-style";
import type { Experiment, Series } from "@/entities/experiment/model/experiment";
import type { MonotonicityType, SeriesComputedConvergenceAnalysis, SideType } from "../model/types";
import { useSeriesComputedConvergence } from "../model/useSeriesComputedConvergence";
import {
    applyMonotonicityThreshold,
    applySideThreshold,
    buildArgsSummary,
    buildDetailPoints,
    formatIntervals,
    getSeriesRowDomId,
} from "../model/seriesComputedConvergenceUtils";
import {
    buildSeriesComputedClassLegendTitle,
    computeSeriesComputedDevStats,
    formatAmplitudeOrders,
    formatComplexValue,
    formatDeviationValue,
    getSeriesComputedClassInfo,
    type SeriesComputedClassInfo,
    type SeriesComputedDevStats,
} from "../model/seriesComputedSummary";
import { SeriesComputedDetailChart } from "./SeriesComputedDetailChart";
import { ExperimentMatrixFilterScope } from "@/shared/ui/Matrix/filters/ExperimentMatrixFilterScope";
import { MatrixExportWrapper } from "@/shared/ui/Matrix/MatrixExportWrapper";
import {
    appendSheet,
    buildKeyValueSheet,
    buildSheetFromAoa,
    createWorkbook,
} from "@/shared/lib/xlsxExport";
import {
    buildExperimentSessionStateKey,
    useInMemorySessionState,
} from "@/shared/lib/inMemorySessionState";
import {
    getSeriesComputedColumnAnchorId,
    SERIES_COMPUTED_TABLE_DOCS,
    type SeriesComputedDocsColumnKey,
} from "@/shared/lib/docs/tableDocs";
import { DocsAnchorButton } from "@/shared/ui/docs/DocsAnchorButton";

export interface SeriesComputedConvergenceTableProps {
    experiment: Experiment | null;
    className?: string;
}

type SortKey = SeriesComputedDocsColumnKey;
type SortDir = "asc" | "desc";

interface SeriesComputedConvergenceTableViewState {
    maxSignChangesForOneSided: number;
    maxViolationsForMonotone: number;
    sort: { key: SortKey; dir: SortDir } | null;
}

interface RowData {
    series: Series;
    analysis: SeriesComputedConvergenceAnalysis;
    side: SideType;
    monotonicity: MonotonicityType;
    argsSummary: string;
    classInfo: SeriesComputedClassInfo;
    dev: SeriesComputedDevStats;
}

function getRowColorClass(classInfo: SeriesComputedClassInfo, selected: boolean): string {
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

function cmpNumNullable(a: number | null, b: number | null): number {
    if (a == null && b == null) return 0;
    if (a == null) return 1;
    if (b == null) return -1;
    return a - b;
}

function formatSignedError(point: { err: number | null; sign: -1 | 0 | 1 | null }): number | null {
    if (point.err == null || !Number.isFinite(point.err)) return null;
    if (point.sign == null || point.sign === 0) return point.err;
    return point.err * point.sign;
}

function getExportSideLabel(
    side: SideType,
    kind: SeriesComputedClassInfo["kind"]
): string {
    if (kind === "unknown" || side === "unknown") return "?";
    if (kind === "static") return "1s";
    return side === "two_sided" ? "2s" : "1s";
}

function buildSummarySheet(rows: RowData[]): XLSX.WorkSheet {
    const header = [
        "#",
        "series",
        "precision",
        "args",
        "limit",
        "side",
        "class",
        "class title",
        "steps",
        "sign changes",
        "violations",
        "min |S_n-S|",
        "min n",
        "last |S_n-S|",
        "last n",
        "last - min",
        "last/min amp",
        "max/min amp",
        "mean |S_n-S|",
        "median |S_n-S|",
        "max |S_n-S|",
    ];

    const aoa: Array<Array<string | number | null>> = [header];

    rows.forEach((row, index) => {
        aoa.push([
            index + 1,
            row.series.name,
            row.series.precision,
            row.argsSummary || null,
            formatComplexValue(row.series.limit ?? null),
            getExportSideLabel(row.side, row.classInfo.kind),
            row.classInfo.symbol,
            row.classInfo.title,
            row.analysis.stepsAnalyzed,
            row.analysis.signChangesCount,
            row.analysis.incCount,
            row.dev.min,
            row.dev.minN,
            row.dev.last,
            row.dev.lastN,
            row.dev.lastMinusMin,
            row.dev.amplitudeOrders,
            row.dev.maxAmplitudeOrders,
            row.dev.mean,
            row.dev.median,
            row.dev.max,
        ]);
    });

    return buildSheetFromAoa(aoa, {
        cols: [
            { wch: 6 },
            { wch: 24 },
            { wch: 12 },
            { wch: 28 },
            { wch: 26 },
            { wch: 8 },
            { wch: 10 },
            { wch: 24 },
            { wch: 10 },
            { wch: 14 },
            { wch: 12 },
            { wch: 16 },
            { wch: 10 },
            { wch: 16 },
            { wch: 10 },
            { wch: 14 },
            { wch: 12 },
            { wch: 12 },
            { wch: 16 },
            { wch: 16 },
            { wch: 16 },
        ],
        headerRows: 1,
        rowHeaderCols: 2,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0) return;
            if ([0, 8, 9, 10, 12, 14].includes(colIndex)) cell.z = "0";
            if ([11, 13, 15, 18, 19, 20].includes(colIndex)) cell.z = "0.000E+00";
            if ([16, 17].includes(colIndex)) cell.z = "0.00";
        },
    });
}

function buildSelectedMetaSheet(selected: {
    series: Series;
    analysis: SeriesComputedConvergenceAnalysis;
    side: SideType;
    argsSummary: string;
    classInfo: SeriesComputedClassInfo;
    dev: SeriesComputedDevStats;
}): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | null>> = [
        ["field", "value"],
        ["series", selected.series.name],
        ["precision", selected.series.precision],
        ["args", selected.argsSummary || null],
        ["limit", formatComplexValue(selected.series.limit ?? null)],
        ["side", getExportSideLabel(selected.side, selected.classInfo.kind)],
        ["class", selected.classInfo.symbol],
        ["class title", selected.classInfo.title],
        ["class description", selected.classInfo.description],
        ["steps", selected.analysis.stepsAnalyzed],
        ["sign changes", selected.analysis.signChangesCount],
        ["violations", selected.analysis.incCount],
        ["min |S_n-S|", selected.dev.min],
        ["min n", selected.dev.minN],
        ["last |S_n-S|", selected.dev.last],
        ["last n", selected.dev.lastN],
        ["last - min", selected.dev.lastMinusMin],
        ["last/min amp", selected.dev.amplitudeOrders],
        ["max/min amp", selected.dev.maxAmplitudeOrders],
        ["mean |S_n-S|", selected.dev.mean],
        ["median |S_n-S|", selected.dev.median],
        ["max |S_n-S|", selected.dev.max],
    ];

    return buildSheetFromAoa(aoa, {
        cols: [{ wch: 20 }, { wch: 28 }],
        headerRows: 1,
        rowHeaderCols: 1,
    });
}

function buildPointsSheet(points: ReturnType<typeof buildDetailPoints>): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | null>> = [
        ["n", "Re(S_n)", "Im(S_n)", "|S_n-S|", "sgn*|S_n-S|", "sgn(Re(S_n-S))"],
    ];

    for (const point of points) {
        aoa.push([
            point.n,
            point.valueRe,
            point.valueIm,
            point.err,
            formatSignedError(point),
            point.sign,
        ]);
    }

    return buildSheetFromAoa(aoa, {
        cols: [
            { wch: 10 },
            { wch: 18 },
            { wch: 18 },
            { wch: 16 },
            { wch: 16 },
            { wch: 14 },
        ],
        headerRows: 1,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0) return;
            if ([0, 5].includes(colIndex)) cell.z = "0";
            if ([1, 2, 3, 4].includes(colIndex)) cell.z = "0.000E+00";
        },
    });
}

function buildDiffsSheet(points: ReturnType<typeof buildDetailPoints>): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | null>> = [
        ["n", "Re(S_n-S_{n-1})", "Im(S_n-S_{n-1})", "|S_n-S_{n-1}|", "sgn*|S_n-S_{n-1}|"],
    ];

    for (const point of points.filter((entry) => entry.diffNorm != null)) {
        const signedDiff =
            point.diffNorm != null &&
            Number.isFinite(point.diffNorm) &&
            point.diffRe != null &&
            Number.isFinite(point.diffRe)
                ? point.diffRe >= 0
                    ? point.diffNorm
                    : -point.diffNorm
                : point.diffNorm;

        aoa.push([point.n, point.diffRe, point.diffIm, point.diffNorm, signedDiff]);
    }

    return buildSheetFromAoa(aoa, {
        cols: [
            { wch: 10 },
            { wch: 18 },
            { wch: 18 },
            { wch: 18 },
            { wch: 18 },
        ],
        headerRows: 1,
        decorateCell: ({ rowIndex, colIndex, cell }) => {
            if (rowIndex === 0) return;
            if (colIndex === 0) cell.z = "0";
            if ([1, 2, 3, 4].includes(colIndex)) cell.z = "0.000E+00";
        },
    });
}

const SeriesComputedConvergenceTableView: React.FC<
    SeriesComputedConvergenceTableProps & { totalRowsBeforeFilter?: number }
> = ({ experiment, className, totalRowsBeforeFilter }) => {
    const { seriesList, analysisBySeriesId, progress } = useSeriesComputedConvergence(experiment);

    const [selectedSeriesId, setSelectedSeriesId] = useState<string | null>(null);
    const viewSessionKey = experiment
        ? buildExperimentSessionStateKey(experiment.id, "view:series-computed-convergence")
        : undefined;
    const [viewState, setViewState] =
        useInMemorySessionState<SeriesComputedConvergenceTableViewState>({
            key: viewSessionKey,
            initialValue: {
                maxSignChangesForOneSided: 0,
                maxViolationsForMonotone: 0,
                sort: null,
            },
        });
    const { maxSignChangesForOneSided, maxViolationsForMonotone, sort } = viewState;

    const detailRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setSelectedSeriesId(null);
    }, [experiment?.id]);

    useEffect(() => {
        if (selectedSeriesId && detailRef.current) {
            detailRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedSeriesId]);

    const scrollBackToSelected = useCallback(() => {
        if (!selectedSeriesId) return;
        const element = document.getElementById(getSeriesRowDomId(selectedSeriesId));
        if (element) element.scrollIntoView({ behavior: "smooth", block: "center" });
    }, [selectedSeriesId]);

    const toggleSort = useCallback((key: SortKey) => {
        setViewState((current) => ({
            ...current,
            sort:
                !current.sort || current.sort.key !== key
                    ? { key, dir: "asc" }
                    : {
                        key,
                        dir: current.sort.dir === "asc" ? "desc" : "asc",
                    },
        }));
    }, [setViewState]);

    const sortMark = useCallback(
        (key: SortKey) => {
            if (!sort || sort.key !== key) return "";
            return sort.dir === "asc" ? "▲" : "▼";
        },
        [sort]
    );

    const renderSortHeader = useCallback(
        (
            key: SortKey,
            label: string,
            title: string,
            align: "left" | "right" = "left"
        ) => (
            <th
                key={key}
                className={[
                    "group relative cursor-pointer select-none border-b border-border px-2 py-2",
                    align === "right" ? "text-right" : "text-left",
                ].join(" ")}
                onClick={() => toggleSort(key)}
                title={title}
            >
                <span className="pr-6">
                    {label}
                    <span className="ml-1 text-[9px] text-textDim/70">{sortMark(key)}</span>
                </span>

                <DocsAnchorButton
                    anchorId={getSeriesComputedColumnAnchorId(key)}
                    label={`${SERIES_COMPUTED_TABLE_DOCS.title}: ${label}`}
                    className="absolute right-1 top-1"
                />
            </th>
        ),
        [sortMark, toggleSort]
    );

    const rows = useMemo<RowData[]>(() => {
        return seriesList
            .map((series) => {
                const analysis = analysisBySeriesId[series.id];
                if (!analysis) return null;

                const side = applySideThreshold(
                    analysis.sideRaw,
                    analysis.signChangesCount,
                    maxSignChangesForOneSided
                );
                const monotonicity = applyMonotonicityThreshold(
                    analysis.monotonicityRaw,
                    analysis.incCount,
                    analysis.decCount,
                    analysis.eqCount,
                    maxViolationsForMonotone
                );

                const dev = computeSeriesComputedDevStats(series);
                const classInfo = getSeriesComputedClassInfo(side, monotonicity, dev);

                return {
                    series,
                    analysis,
                    side,
                    monotonicity,
                    argsSummary: buildArgsSummary(series.args ?? null),
                    classInfo,
                    dev,
                };
            })
            .filter((row): row is RowData => row != null);
    }, [seriesList, analysisBySeriesId, maxSignChangesForOneSided, maxViolationsForMonotone]);

    const sortedRows = useMemo(() => {
        if (!sort) return rows;

        const direction = sort.dir === "asc" ? 1 : -1;
        const compareText = (a: string, b: string) => a.localeCompare(b, "ru");

        const getValue = (row: RowData) => {
            switch (sort.key) {
                case "name":
                    return row.series.name ?? "";
                case "precision":
                    return row.series.precision ?? "";
                case "args":
                    return row.argsSummary ?? "";
                case "class":
                    return row.classInfo.order;
                case "k":
                    return row.analysis.stepsAnalyzed;
                case "sign":
                    return row.analysis.signChangesCount;
                case "viol":
                    return row.analysis.incCount;
                case "devMin":
                    return row.dev.min;
                case "minN":
                    return row.dev.minN;
                case "devLast":
                    return row.dev.last;
                case "lastN":
                    return row.dev.lastN;
                case "devMean":
                    return row.dev.mean;
                case "devMedian":
                    return row.dev.median;
                case "devMax":
                    return row.dev.max;
                case "lastMinusMin":
                    return row.dev.lastMinusMin;
                case "ampOrders":
                    return row.dev.amplitudeOrders;
                case "maxAmpOrders":
                    return row.dev.maxAmplitudeOrders;
            }
        };

        const numericKey =
            sort.key === "class" || sort.key === "k" || sort.key === "sign" || sort.key === "viol";
        const nullableNumericKey =
            sort.key === "devMin" ||
            sort.key === "minN" ||
            sort.key === "devLast" ||
            sort.key === "lastN" ||
            sort.key === "devMean" ||
            sort.key === "devMedian" ||
            sort.key === "devMax" ||
            sort.key === "lastMinusMin" ||
            sort.key === "ampOrders" ||
            sort.key === "maxAmpOrders";

        const result = [...rows];
        result.sort((left, right) => {
            if (nullableNumericKey) {
                const base = cmpNumNullable(
                    getValue(left) as number | null,
                    getValue(right) as number | null
                );
                if (base !== 0) return direction * base;
            } else if (numericKey) {
                const base = Number(getValue(left)) - Number(getValue(right));
                if (base !== 0) return direction * base;
            } else {
                const base = compareText(String(getValue(left)), String(getValue(right)));
                if (base !== 0) return direction * base;
            }

            return compareText(left.series.name ?? "", right.series.name ?? "");
        });

        return result;
    }, [rows, sort]);

    useEffect(() => {
        if (!selectedSeriesId) return;
        if (sortedRows.some((row) => row.series.id === selectedSeriesId)) return;
        setSelectedSeriesId(null);
    }, [selectedSeriesId, sortedRows]);

    const selected = useMemo(() => {
        if (!experiment || !selectedSeriesId) return null;

        const series = (experiment.seriesList ?? []).find((item) => item.id === selectedSeriesId) ?? null;
        const analysis = analysisBySeriesId[selectedSeriesId] ?? null;
        if (!series || !analysis) return null;

        const side = applySideThreshold(
            analysis.sideRaw,
            analysis.signChangesCount,
            maxSignChangesForOneSided
        );
        const monotonicity = applyMonotonicityThreshold(
            analysis.monotonicityRaw,
            analysis.incCount,
            analysis.decCount,
            analysis.eqCount,
            maxViolationsForMonotone
        );
        const dev = computeSeriesComputedDevStats(series);

        return {
            series,
            analysis,
            side,
            monotonicity,
            classInfo: getSeriesComputedClassInfo(side, monotonicity, dev),
            points: buildDetailPoints(series),
            argsSummary: buildArgsSummary(series.args ?? null),
            dev,
        };
    }, [
        experiment,
        selectedSeriesId,
        analysisBySeriesId,
        maxSignChangesForOneSided,
        maxViolationsForMonotone,
    ]);

    const buildWorkbook = useCallback((): XLSX.WorkBook => {
        const workbook = createWorkbook(
            "Series computed convergence",
            "Series computed convergence export"
        );

        appendSheet(
            workbook,
            buildKeyValueSheet([
                { key: "rows", value: sortedRows.length },
                { key: "total rows before filter", value: totalRowsBeforeFilter ?? sortedRows.length },
                { key: "max sign changes", value: maxSignChangesForOneSided },
                { key: "max violations", value: maxViolationsForMonotone },
                { key: "sort", value: sort ? `${sort.key} (${sort.dir})` : "default" },
                { key: "selected series", value: selected?.series.name ?? "none" },
            ]),
            "overview"
        );

        appendSheet(workbook, buildSummarySheet(sortedRows), "summary");

        if (selected) {
            appendSheet(workbook, buildSelectedMetaSheet(selected), "selected_meta");
            appendSheet(workbook, buildPointsSheet(selected.points), "selected_points");
            appendSheet(workbook, buildDiffsSheet(selected.points), "selected_diffs");
        }

        return workbook;
    }, [
        maxSignChangesForOneSided,
        maxViolationsForMonotone,
        selected,
        sort,
        sortedRows,
        totalRowsBeforeFilter,
    ]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim">Нет данных для анализа (experiment = null).</div>
            </div>
        );
    }

    if (progress.running) {
        const { current, total } = progress;
        const pct = total > 0 ? Math.max(0, Math.min(100, Math.round((current / total) * 100))) : 0;

        return (
            <div className={className}>
                <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
                    <div className="mb-2 flex items-center justify-between text-sm text-textDim">
                        <span>Подсчёт свойств частичных сумм...</span>
                        <span>
                            {current} / {total} ({pct}%)
                        </span>
                    </div>
                    <div className="h-2 w-full overflow-hidden rounded bg-surface/60">
                        <div
                            className="h-2 bg-primary transition-[width]"
                            style={{ width: `${pct}%` }}
                        />
                    </div>
                </div>
            </div>
        );
    }

    if (rows.length === 0) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim">
                    Нет рядов с рассчитанными частичными суммами (`series.computed` пуст).
                </div>
            </div>
        );
    }

    const classLegendTitle = buildSeriesComputedClassLegendTitle(
        maxSignChangesForOneSided,
        maxViolationsForMonotone
    );

    return (
        <div className={className}>
            <MatrixExportWrapper
                fileBaseName="series-computed-convergence"
                enableXlsx
                buildWorkbook={buildWorkbook}
            >
                {({ captureRef }) => (
                    <div ref={captureRef}>
                        <div className="mb-2 rounded-xl border border-border bg-panel p-3 text-[11px] text-textDim shadow-panel">
                            <div className="mb-2">
                                <div className="group inline-flex items-center gap-2 text-sm font-semibold">
                                    <span>Частичные суммы: анализ по рядам</span>
                                    <DocsAnchorButton
                                        anchorId={SERIES_COMPUTED_TABLE_DOCS.id}
                                        label={SERIES_COMPUTED_TABLE_DOCS.title}
                                    />
                                </div>
                            </div>

                            <div className="flex flex-col gap-2">
                                <div className="flex items-center gap-2">
                                    <span
                                        className="whitespace-nowrap"
                                        title="Максимальное число смен знака Re(S_n - S), при котором ряд считается односторонним."
                                    >
                                        max sign changes:
                                    </span>
                                    <input
                                        type="range"
                                        min={0}
                                        max={50}
                                        value={maxSignChangesForOneSided}
                                        onChange={(event) =>
                                            setViewState((current) => ({
                                                ...current,
                                                maxSignChangesForOneSided: Number(
                                                    event.target.value
                                                ),
                                            }))
                                        }
                                        className="h-[4px] w-40 cursor-pointer"
                                    />
                                    <span className="w-8 text-right tabular-nums">
                                        {maxSignChangesForOneSided}
                                    </span>
                                </div>

                                <div className="flex items-center gap-2">
                                    <span
                                        className="whitespace-nowrap"
                                        title="Максимальное число ростов |S_n - S|, при котором ряд всё ещё считается почти монотонным."
                                    >
                                        max violations:
                                    </span>
                                    <input
                                        type="range"
                                        min={0}
                                        max={50}
                                        value={maxViolationsForMonotone}
                                        onChange={(event) =>
                                            setViewState((current) => ({
                                                ...current,
                                                maxViolationsForMonotone: Number(
                                                    event.target.value
                                                ),
                                            }))
                                        }
                                        className="h-[4px] w-40 cursor-pointer"
                                    />
                                    <span className="w-8 text-right tabular-nums">
                                        {maxViolationsForMonotone}
                                    </span>
                                </div>
                            </div>

                            <div className="mt-2 text-[10px] text-textDim/70">
                                Клик по строке открывает детальный график и таблицы. Наведи на заголовок
                                <span className="font-semibold"> класс</span>, чтобы увидеть легенду символов.
                            </div>
                            <div className="mt-1 text-[10px] text-textDim/70">
                                rows: {sortedRows.length} / {totalRowsBeforeFilter ?? rows.length}
                            </div>
                        </div>

                        <div
                            className="overflow-auto rounded-xl border border-border bg-surface/40"
                            style={{ maxHeight: "55vh" }}
                        >
                            <table className="min-w-full border-collapse text-[10px]">
                                <thead className="sticky top-0 z-10 bg-surface/80">
                                    <tr>
                                        {renderSortHeader("name", "Ряд", "Сортировать по имени ряда")}
                                        {renderSortHeader(
                                            "precision",
                                            "prec",
                                            "Сортировать по precision"
                                        )}
                                        {renderSortHeader(
                                            "args",
                                            "args",
                                            "Сортировать по аргументам ряда"
                                        )}
                                        {renderSortHeader("class", "класс", classLegendTitle)}
                                        {renderSortHeader(
                                            "k",
                                            "k",
                                            "Сортировать по числу пар (n-1, n) в анализе",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "sign",
                                            "sign",
                                            "Сортировать по числу смен знака",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "viol",
                                            "viol",
                                            "Сортировать по числу ростов |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "devMin",
                                            "min",
                                            "min_n |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "minN",
                                            "n min",
                                            "n, на котором достигается минимум |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "devLast",
                                            "last",
                                            "|S_last-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "lastN",
                                            "n last",
                                            "Последний n с конечным |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "lastMinusMin",
                                            "last-min",
                                            "Разность между |S_last-S| и min_n |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "ampOrders",
                                            "last/min amp",
                                            "last/min amp = log10(last) - log10(min)",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "maxAmpOrders",
                                            "max/min amp",
                                            "max/min amp = log10(max) - log10(min)",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "devMean",
                                            "mean",
                                            "mean_n |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "devMedian",
                                            "med",
                                            "median_n |S_n-S|",
                                            "right"
                                        )}
                                        {renderSortHeader(
                                            "devMax",
                                            "max",
                                            "max_n |S_n-S|",
                                            "right"
                                        )}
                                    </tr>
                                </thead>
                                <tbody>
                                    {sortedRows.map((row) => {
                                        const isSelected = selectedSeriesId === row.series.id;
                                        const rowClassName = getRowColorClass(row.classInfo, isSelected);
                                        const signNsText = row.analysis.signChangeNs.length
                                            ? formatIntervals(row.analysis.signChangeNs)
                                            : "—";
                                        const violNsText = row.analysis.violationsNs.length
                                            ? formatIntervals(row.analysis.violationsNs)
                                            : "—";
                                        const titleLines = [
                                            `Ряд: ${row.series.name}`,
                                            `prec: ${row.series.precision}`,
                                            `args: ${row.argsSummary || "—"}`,
                                            `limit: ${formatComplexValue(row.series.limit ?? null)}`,
                                            `class: ${row.classInfo.label} — ${row.classInfo.title}`,
                                            row.classInfo.description,
                                            "",
                                            `dev count: ${row.dev.count}`,
                                            `min |S_n-S|: ${formatDeviationValue(row.dev.min)} at n=${row.dev.minN ?? "—"}`,
                                            `last |S_n-S|: ${formatDeviationValue(row.dev.last)} at n=${row.dev.lastN ?? "—"}`,
                                            `last - min: ${formatDeviationValue(row.dev.lastMinusMin)}`,
                                            `last/min amp: ${formatAmplitudeOrders(row.dev.amplitudeOrders)}`,
                                            `max/min amp: ${formatAmplitudeOrders(row.dev.maxAmplitudeOrders)}`,
                                            `mean |S_n-S|: ${formatDeviationValue(row.dev.mean)}`,
                                            `med |S_n-S|: ${formatDeviationValue(row.dev.median)}`,
                                            `max |S_n-S|: ${formatDeviationValue(row.dev.max)}`,
                                            "",
                                            `pairs analyzed: ${row.analysis.stepsAnalyzed}`,
                                            `sign changes: ${row.analysis.signChangesCount}, ns: ${signNsText}`,
                                            `violations: ${row.analysis.incCount}, ns: ${violNsText}`,
                                        ];

                                        return (
                                            <tr
                                                key={row.series.id}
                                                id={getSeriesRowDomId(row.series.id)}
                                                className={"cursor-pointer " + rowClassName}
                                                title={titleLines.join("\n")}
                                                onClick={() => setSelectedSeriesId(row.series.id)}
                                            >
                                                <td className="border-t border-border px-2 py-2 font-medium">
                                                    {row.series.name}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 font-mono">
                                                    {row.series.precision}
                                                </td>
                                                <td
                                                    className="max-w-[240px] truncate border-t border-border px-2 py-2 font-mono"
                                                    title={row.argsSummary || "—"}
                                                >
                                                    {row.argsSummary || "—"}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 font-mono">
                                                    {row.classInfo.label}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {row.analysis.stepsAnalyzed}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {row.analysis.signChangesCount}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {row.analysis.incCount}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatDeviationValue(row.dev.min)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {row.dev.minN ?? "—"}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatDeviationValue(row.dev.last)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {row.dev.lastN ?? "—"}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatDeviationValue(row.dev.lastMinusMin)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatAmplitudeOrders(row.dev.amplitudeOrders)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatAmplitudeOrders(row.dev.maxAmplitudeOrders)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatDeviationValue(row.dev.mean)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatDeviationValue(row.dev.median)}
                                                </td>
                                                <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                                    {formatDeviationValue(row.dev.max)}
                                                </td>
                                            </tr>
                                        );
                                    })}
                                </tbody>
                            </table>
                        </div>

                        <div ref={detailRef}>
                            {selected ? (
                                <div className="mt-4">
                                    <div className="mb-2 flex items-start justify-between gap-2">
                                        <div className="text-[10px] text-textDim">
                                            <div className="font-semibold text-textDim/90">
                                                {selected.series.name}
                                            </div>
                                            <div className="font-mono">
                                                prec: {selected.series.precision}
                                                <span className="text-textDim/70"> | </span>
                                                args: {selected.argsSummary || "—"}
                                            </div>
                                            <div className="font-mono">
                                                limit: {formatComplexValue(selected.series.limit ?? null)}
                                                <span className="text-textDim/70"> | </span>
                                                class: {selected.classInfo.label} ({selected.classInfo.title})
                                            </div>
                                            <div className="font-mono">
                                                min: {formatDeviationValue(selected.dev.min)} @ n=
                                                {selected.dev.minN ?? "—"}
                                                <span className="text-textDim/70"> | </span>
                                                last: {formatDeviationValue(selected.dev.last)} @ n=
                                                {selected.dev.lastN ?? "—"}
                                                <span className="text-textDim/70"> | </span>
                                                last-min: {formatDeviationValue(selected.dev.lastMinusMin)}
                                                <span className="text-textDim/70"> | </span>
                                                last/min amp: {formatAmplitudeOrders(selected.dev.amplitudeOrders)}
                                                <span className="text-textDim/70"> | </span>
                                                max/min amp: {formatAmplitudeOrders(selected.dev.maxAmplitudeOrders)}
                                            </div>
                                        </div>

                                        <button
                                            type="button"
                                            className="rounded border border-border bg-surface px-2 py-[2px] text-[10px] text-textDim hover:bg-panel"
                                            onClick={scrollBackToSelected}
                                        >
                                            Вернуться к выбранному ряду
                                        </button>
                                    </div>

                                    <SeriesComputedDetailChart
                                        series={selected.series}
                                        classInfo={selected.classInfo}
                                        stepsAnalyzed={selected.analysis.stepsAnalyzed}
                                        points={selected.points}
                                        dev={selected.dev}
                                    />
                                </div>
                            ) : null}
                        </div>
                    </div>
                )}
            </MatrixExportWrapper>
        </div>
    );
};

export const SeriesComputedConvergenceTable: React.FC<SeriesComputedConvergenceTableProps> = ({
    experiment,
    className,
}) => {
    const totalRowsBeforeFilter = useMemo(
        () => (experiment?.seriesList ?? []).filter((series) => (series.computed ?? []).length > 0).length,
        [experiment]
    );

    return (
        <ExperimentMatrixFilterScope experiment={experiment} resetKey={experiment?.id ?? "no-exp"}>
            {({ experimentFiltered }) => (
                <SeriesComputedConvergenceTableView
                    experiment={experimentFiltered}
                    className={className}
                    totalRowsBeforeFilter={totalRowsBeforeFilter}
                />
            )}
        </ExperimentMatrixFilterScope>
    );
};
