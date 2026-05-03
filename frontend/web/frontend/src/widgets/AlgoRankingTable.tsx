import React, { useCallback, useEffect, useMemo } from "react";
import * as XLSX from "xlsx-js-style";
import type { Experiment } from "@/entities/experiment/model/experiment";
import {
    buildExperimentSessionStateKey,
    useInMemorySessionState,
} from "@/shared/lib/inMemorySessionState";
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";
import type { MatrixAxisItem, MatrixProps } from "@/shared/ui/Matrix/Matrix";
import {
    MatrixAlgoSeriesFilter,
    type MatrixAlgoSeriesFilterState,
} from "@/shared/ui/Matrix/filters/MatrixAlgoSeriesFilter";
import {
    buildAlgoStatsFromExperiment,
    getVisibleArgKeys,
    type AlgoRankingSortKey as BaseSortKey,
    type AlgoStats,
} from "./AlgoRankingTable.model";
import {
    appendSheet,
    buildKeyValueSheet,
    buildSheetFromAoa,
    createWorkbook,
} from "@/shared/lib/xlsxExport";
import {
    ALGO_RANKING_TABLE_DOCS,
    getAlgoRankingColumnAnchorId,
    type AlgoRankingDocsColumnKey,
} from "@/shared/lib/docs/tableDocs";
import { DocsAnchorButton } from "@/shared/ui/docs/DocsAnchorButton";

type SortDir = "asc" | "desc";

interface AlgoRankingTableViewState {
    epsilonExp: number;
    precisionFilter: string | null;
    sortKey: RankingSortKey;
    sortDir: SortDir;
}

export interface AlgoRankingTableProps {
    experiment: Experiment | null;
    epsilon?: number;
    className?: string;
}

type RowMeta = AlgoStats & { place: number; precisionLabel: string };

type DynamicArgColumnId = `arg:${string}`;
type DynamicArgSortKey = `arg:${string}`;
type RankingSortKey = BaseSortKey | DynamicArgSortKey;
type ColId = AlgoRankingDocsColumnKey | DynamicArgColumnId;

type ColMeta = {
    id: ColId;
    title: string;
    description: string;
    sortKey: RankingSortKey;
    defaultDir: SortDir;
    docsColumnId?: AlgoRankingDocsColumnKey;
    argKey?: string;
};

const BASE_COLUMNS: ColMeta[] = [
    {
        id: "precision",
        title: "precision",
        description:
            "Precision рядов, по которым считалась строка алгоритма. Если в строку попали разные precision без фильтра, может быть '-'.",
        sortKey: "precision",
        defaultDir: "asc",
    },
    {
        id: "m",
        title: "m",
        description: "Параметр m алгоритма.",
        sortKey: "m",
        defaultDir: "asc",
    },
    {
        id: "seriesCount",
        title: "series",
        description: "Число уникальных рядов, по которым этот алгоритм удалось сравнить.",
        sortKey: "seriesCount",
        defaultDir: "desc",
    },
    {
        id: "avgBestDeviation",
        title: "avg min |dev|",
        description: "Среднее по рядам от min |A_n - lim| для алгоритма. Меньше лучше.",
        sortKey: "avgBestDeviation",
        defaultDir: "asc",
    },
    {
        id: "avgRelativeError",
        title: "avg rel error",
        description:
            "Среднее отношение min |A_n - lim| алгоритма к min |S_n - lim| ряда. 1 = на уровне ряда, <1 = лучше ряда, >1 = хуже. В среднее входят только ряды с min |S_n - lim| > 0; случаи с нулевым минимумом ряда оцениваются через avg series/algo amp.",
        sortKey: "avgRelativeError",
        defaultDir: "asc",
    },
    {
        id: "avgOrdersGain",
        title: "avg series/algo amp",
        description:
            "Средний выигрыш по порядкам между минимумом ряда и минимумом алгоритма: lg10(min |S_n - lim|) - lg10(min |A_n - lim|). Больше лучше.",
        sortKey: "avgOrdersGain",
        defaultDir: "desc",
    },
    {
        id: "avgAmpAtMinN",
        title: "avg series@min n/algo amp",
        description:
            "Средний выигрыш по порядкам на первом n, где алгоритм достиг своего min |A_n - lim|: lg10(|S_n - lim| на этом n) - lg10(min |A_n - lim|). Больше лучше.",
        sortKey: "avgAmpAtMinN",
        defaultDir: "desc",
    },
    {
        id: "notBetterThanSeriesShare",
        title: "min algo >= min series, %",
        description:
            "Доля рядов, где min |A_n - lim| оказалось не лучше частичных сумм: min |A_n - lim| >= min |S_n - lim|. Идеально 0%. Меньше лучше.",
        sortKey: "notBetterThanSeriesShare",
        defaultDir: "asc",
    },
    {
        id: "avgMinDeviationN",
        title: "avg min dev n",
        description: "Среднее n, на котором алгоритм достигает своего min |A_n - lim|. Меньше лучше.",
        sortKey: "avgMinDeviationN",
        defaultDir: "asc",
    },
    {
        id: "avgLastMinusMin",
        title: "avg last-min",
        description:
            "Средний уход от лучшей точки к последней: |A_last - lim| - min_n |A_n - lim|. Меньше лучше.",
        sortKey: "avgLastMinusMin",
        defaultDir: "asc",
    },
    {
        id: "avgStepsToTol",
        title: "avg steps to eps",
        description:
            "Средний первый n, на котором |A_n - lim| <= epsilon. Если epsilon не достигнут, такой ряд в среднее не входит.",
        sortKey: "avgStepsToTol",
        defaultDir: "asc",
    },
    {
        id: "fracReachedTol",
        title: "reached eps, %",
        description: "Доля рядов, на которых алгоритм вообще достиг |A_n - lim| <= epsilon. Больше лучше.",
        sortKey: "fracReachedTol",
        defaultDir: "desc",
    },
    {
        id: "oneSidedShare",
        title: "1-sided, %",
        description:
            "Доля запусков, где алгоритм шел к пределу односторонне. Для класса → это тоже считается как 1-sided. Больше лучше.",
        sortKey: "oneSidedShare",
        defaultDir: "desc",
    },
    {
        id: "bestMinShare",
        title: "best min div, %",
        description:
            "Доля сравнимых рядов, где min |A_n - lim| у этого алгоритма был лучшим среди всех алгоритмов. Если несколько алгоритмов разделили одинаковый лучший минимум, процент засчитывается каждому из них, поэтому суммы по столбцу могут быть больше 100%. Больше лучше.",
        sortKey: "bestMinShare",
        defaultDir: "desc",
    },
    {
        id: "worstMinShare",
        title: "worst min div, %",
        description:
            "Доля сравнимых рядов, где min |A_n - lim| у этого алгоритма был худшим среди всех алгоритмов. Если несколько алгоритмов разделили одинаковый худший минимум, процент засчитывается каждому из них, поэтому суммы по столбцу могут быть больше 100%. Меньше лучше.",
        sortKey: "worstMinShare",
        defaultDir: "asc",
    },
    {
        id: "bestLastShare",
        title: "best last div, %",
        description:
            "Доля сравнимых рядов, где на последнем n ошибка алгоритма была лучшей среди всех алгоритмов. Больше лучше.",
        sortKey: "bestLastShare",
        defaultDir: "desc",
    },
    {
        id: "worstLastShare",
        title: "worst last div, %",
        description:
            "Доля сравнимых рядов, где на последнем n ошибка алгоритма была худшей среди всех алгоритмов. Меньше лучше.",
        sortKey: "worstLastShare",
        defaultDir: "asc",
    },
    {
        id: "rankPrecision",
        title: "rank precision",
        description:
            "Итоговый rank по точности: avg min |dev| + avg rel error + avg series/algo amp + min algo >= min series, % + best min div + worst min div. Меньше лучше.",
        sortKey: "rankPrecision",
        defaultDir: "asc",
    },
    {
        id: "rankSpeed",
        title: "rank speed",
        description: "Итоговый rank по скорости: avg min dev n + avg steps to eps. Меньше лучше.",
        sortKey: "rankSpeed",
        defaultDir: "asc",
    },
    {
        id: "rankStability",
        title: "rank stability",
        description:
            "Итоговый rank по устойчивости: reached eps + 1-sided share + avg last-min + best last div + worst last div. Меньше лучше.",
        sortKey: "rankStability",
        defaultDir: "asc",
    },
    {
        id: "totalRankScore",
        title: "total rank",
        description:
            "Сумма rank precision + rank speed + rank stability. Это общий ранг алгоритма, меньше лучше.",
        sortKey: "totalRankScore",
        defaultDir: "asc",
    },
];
function buildArgColumn(argKey: string): ColMeta {
    return {
        id: `arg:${argKey}`,
        title: argKey,
        description: `Значение args.${argKey} для конкретного варианта алгоритма.`,
        sortKey: `arg:${argKey}`,
        defaultDir: "asc",
        docsColumnId: "args",
        argKey,
    };
}

function buildColumns(argKeys: string[]): ColMeta[] {
    return [
        BASE_COLUMNS[0],
        BASE_COLUMNS[1],
        ...argKeys.map(buildArgColumn),
        ...BASE_COLUMNS.slice(2),
    ];
}

function formatNumber(x: number): string {
    if (x === Number.POSITIVE_INFINITY) return "∞";
    if (x === Number.NEGATIVE_INFINITY) return "-∞";
    if (!Number.isFinite(x)) return "-";
    const absX = Math.abs(x);
    if (absX === 0) return "0";
    if (absX < 1e-4 || absX >= 1e4) return x.toExponential(2);
    return x.toFixed(4);
}

function formatSteps(n: number): string {
    if (!Number.isFinite(n)) return "-";
    return String(Math.round(n));
}

function formatPercent(value: number): string {
    if (!Number.isFinite(value)) return "-";
    return `${(value * 100).toFixed(1)}%`;
}

function formatSortDir(dir: SortDir): string {
    return dir === "asc" ? "возрастанию" : "убыванию";
}

function buildColumnTooltip(meta: ColMeta, active: boolean, sortDir: SortDir): string {
    const nextDir = active ? (sortDir === "asc" ? "desc" : "asc") : meta.defaultDir;

    return [
        `${meta.title}: ${meta.description}`,
        "",
        active
            ? `Сейчас сортировка по этой колонке идет по ${formatSortDir(sortDir)}.`
            : "Сейчас сортировка идет по другой колонке.",
        `Клик: сортировать по ${formatSortDir(nextDir)}.`,
    ].join("\n");
}

function isDynamicArgColumnId(colId: ColId): colId is DynamicArgColumnId {
    return colId.startsWith("arg:");
}

function isDynamicArgSortKey(sortKey: RankingSortKey): sortKey is DynamicArgSortKey {
    return sortKey.startsWith("arg:");
}

function getArgCellText(row: Pick<AlgoStats, "args">, argKey: string): string {
    const value = row.args?.[argKey];
    if (value === null || value === undefined || value === "") return "-";
    return String(value);
}

function getSortValue(row: AlgoStats, sortKey: RankingSortKey): unknown {
    if (isDynamicArgSortKey(sortKey)) {
        return getArgCellText(row, sortKey.slice(4));
    }

    return row[sortKey];
}

function formatSortKey(sortKey: RankingSortKey): string {
    return isDynamicArgSortKey(sortKey) ? `arg.${sortKey.slice(4)}` : sortKey;
}

function compareValues(aVal: unknown, bVal: unknown, dir: SortDir): number {
    if (typeof aVal === "string" && typeof bVal === "string") {
        const cmp = aVal.localeCompare(bVal);
        return dir === "asc" ? cmp : -cmp;
    }

    const aNum =
        typeof aVal === "number" ? aVal : aVal == null ? Number.POSITIVE_INFINITY : 0;
    const bNum =
        typeof bVal === "number" ? bVal : bVal == null ? Number.POSITIVE_INFINITY : 0;

    if (aNum === bNum) return 0;
    if (dir === "asc") return aNum < bNum ? -1 : 1;
    return aNum > bNum ? -1 : 1;
}

function buildFilterStateKey(state: MatrixAlgoSeriesFilterState): string {
    return [
        state.accel.query,
        state.accel.variantMode,
        state.accel.groupMode,
        Array.from(state.accel.selectedGroupKeys).sort().join(","),
        state.accel.mMinText,
        state.accel.mMaxText,
        state.accel.argsOp,
        state.accel.argClauses.map((clause) => `${clause.key}=${clause.value}`).join("|"),
        state.series.query,
        state.series.noiseMode,
        state.series.groupMode,
        Array.from(state.series.selectedGroupKeys).sort().join(","),
        state.series.precisionMode,
        Array.from(state.series.selectedPrecisions).sort().join(","),
        state.series.argsOp,
        state.series.argClauses.map((clause) => `${clause.key}=${clause.value}`).join("|"),
    ].join("::");
}

function getCellText(row: RowMeta, colId: ColId): string {
    if (isDynamicArgColumnId(colId)) {
        return getArgCellText(row, colId.slice(4));
    }

    switch (colId) {
        case "precision":
            return row.precisionLabel;
        case "m":
            return row.m != null ? String(row.m) : "-";
        case "args":
            return row.argsSummary || "-";
        case "seriesCount":
            return String(row.seriesCount);
        case "avgBestDeviation":
            return formatNumber(row.avgBestDeviation);
        case "avgRelativeError":
            return formatNumber(row.avgRelativeError);
        case "avgOrdersGain":
            return formatNumber(row.avgOrdersGain);
        case "avgAmpAtMinN":
            return formatNumber(row.avgAmpAtMinN);
        case "notBetterThanSeriesShare":
            return row.seriesCount > 0 ? formatPercent(row.notBetterThanSeriesShare) : "-";
        case "avgMinDeviationN":
            return formatSteps(row.avgMinDeviationN);
        case "avgLastMinusMin":
            return formatNumber(row.avgLastMinusMin);
        case "avgStepsToTol":
            return formatSteps(row.avgStepsToTol);
        case "fracReachedTol":
            return row.seriesCount > 0 ? formatPercent(row.fracReachedTol) : "-";
        case "oneSidedShare":
            return row.seriesCount > 0 ? formatPercent(row.oneSidedShare) : "-";
        case "bestMinShare":
            return formatPercent(row.bestMinShare);
        case "worstMinShare":
            return formatPercent(row.worstMinShare);
        case "bestLastShare":
            return formatPercent(row.bestLastShare);
        case "worstLastShare":
            return formatPercent(row.worstLastShare);
        case "rankPrecision":
            return String(row.rankPrecision);
        case "rankSpeed":
            return String(row.rankSpeed);
        case "rankStability":
            return String(row.rankStability);
        case "totalRankScore":
            return String(row.totalRankScore);
    }
}

function getExportValue(row: RowMeta, colId: ColId): string | number | null {
    if (isDynamicArgColumnId(colId)) {
        const text = getArgCellText(row, colId.slice(4));
        return text === "-" ? null : text;
    }

    switch (colId) {
        case "precision":
            return row.precisionLabel;
        case "m":
            return row.m != null ? Number(row.m) || String(row.m) : null;
        case "args":
            return row.argsSummary || null;
        case "seriesCount":
            return row.seriesCount;
        case "avgBestDeviation":
            return Number.isFinite(row.avgBestDeviation) ? row.avgBestDeviation : null;
        case "avgRelativeError":
            return Number.isFinite(row.avgRelativeError)
                ? row.avgRelativeError
                : row.avgRelativeError === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "avgOrdersGain":
            return Number.isFinite(row.avgOrdersGain)
                ? row.avgOrdersGain
                : row.avgOrdersGain === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "avgAmpAtMinN":
            return Number.isFinite(row.avgAmpAtMinN)
                ? row.avgAmpAtMinN
                : row.avgAmpAtMinN === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "notBetterThanSeriesShare":
            return Number.isFinite(row.notBetterThanSeriesShare)
                ? row.notBetterThanSeriesShare
                : null;
        case "avgMinDeviationN":
            return Number.isFinite(row.avgMinDeviationN) ? row.avgMinDeviationN : null;
        case "avgLastMinusMin":
            return Number.isFinite(row.avgLastMinusMin) ? row.avgLastMinusMin : null;
        case "avgStepsToTol":
            return Number.isFinite(row.avgStepsToTol) ? row.avgStepsToTol : null;
        case "fracReachedTol":
            return Number.isFinite(row.fracReachedTol) ? row.fracReachedTol : null;
        case "oneSidedShare":
            return Number.isFinite(row.oneSidedShare) ? row.oneSidedShare : null;
        case "bestMinShare":
            return Number.isFinite(row.bestMinShare) ? row.bestMinShare : null;
        case "worstMinShare":
            return Number.isFinite(row.worstMinShare) ? row.worstMinShare : null;
        case "bestLastShare":
            return Number.isFinite(row.bestLastShare) ? row.bestLastShare : null;
        case "worstLastShare":
            return Number.isFinite(row.worstLastShare) ? row.worstLastShare : null;
        case "rankPrecision":
            return row.rankPrecision;
        case "rankSpeed":
            return row.rankSpeed;
        case "rankStability":
            return row.rankStability;
        case "totalRankScore":
            return row.totalRankScore;
    }
}

function getExportColumnWidth(colId: ColId): number {
    if (isDynamicArgColumnId(colId)) {
        return 18;
    }

    switch (colId) {
        case "precision":
            return 14;
        case "m":
            return 10;
        case "args":
            return 28;
        case "seriesCount":
            return 10;
        case "avgBestDeviation":
        case "avgRelativeError":
        case "avgOrdersGain":
        case "avgAmpAtMinN":
        case "notBetterThanSeriesShare":
        case "avgMinDeviationN":
        case "avgLastMinusMin":
        case "avgStepsToTol":
        case "fracReachedTol":
        case "oneSidedShare":
        case "bestMinShare":
        case "worstMinShare":
        case "bestLastShare":
        case "worstLastShare":
            return 16;
        case "rankPrecision":
        case "rankSpeed":
        case "rankStability":
        case "totalRankScore":
            return 14;
    }
}

function getExportColumnFormat(colId: ColId): string | null {
    if (isDynamicArgColumnId(colId)) {
        return null;
    }

    switch (colId) {
        case "fracReachedTol":
        case "notBetterThanSeriesShare":
        case "oneSidedShare":
        case "bestMinShare":
        case "worstMinShare":
        case "bestLastShare":
        case "worstLastShare":
            return "0.0%";
        case "avgBestDeviation":
        case "avgRelativeError":
        case "avgLastMinusMin":
            return "0.000E+00";
        case "avgOrdersGain":
        case "avgAmpAtMinN":
            return "0.00";
        case "avgMinDeviationN":
        case "avgStepsToTol":
        case "seriesCount":
        case "rankPrecision":
        case "rankSpeed":
        case "rankStability":
        case "totalRankScore":
            return "0";
        default:
            return null;
    }
}

function getExportColumnName(meta: ColMeta): string {
    if (meta.argKey) {
        return `args.${meta.argKey}`;
    }

    return meta.title;
}

export const AlgoRankingTable: React.FC<AlgoRankingTableProps> = ({ experiment, className }) => {
    const viewSessionKey = experiment
        ? buildExperimentSessionStateKey(experiment.id, "view:algo-ranking")
        : undefined;
    const [viewState, setViewState] = useInMemorySessionState<AlgoRankingTableViewState>({
        key: viewSessionKey,
        initialValue: {
            epsilonExp: -6,
            precisionFilter: null,
            sortKey: "totalRankScore",
            sortDir: "asc",
        },
    });
    const { epsilonExp, precisionFilter, sortKey, sortDir } = viewState;
    const epsilon = useMemo(() => Math.pow(10, epsilonExp), [epsilonExp]);
    const sharedFilterSessionKey = experiment
        ? buildExperimentSessionStateKey(experiment.id, "matrix-filters")
        : undefined;

    const precisionsOrder = useMemo(() => {
        if (!experiment || !experiment.seriesList) return [];

        const set = new Set<string>();
        for (const series of experiment.seriesList) {
            if (series.precision) set.add(series.precision);
        }
        return Array.from(set).sort();
    }, [experiment]);

    useEffect(() => {
        if (!precisionFilter || precisionsOrder.includes(precisionFilter)) return;
        setViewState((current) =>
            current.precisionFilter === null
                ? current
                : { ...current, precisionFilter: null }
        );
    }, [precisionFilter, precisionsOrder, setViewState]);

    const totalStats = useMemo(
        () => buildAlgoStatsFromExperiment(experiment, epsilon, precisionFilter),
        [experiment, epsilon, precisionFilter]
    );

    const handleSort = useCallback((nextKey: RankingSortKey, defaultDir: SortDir) => {
        setViewState((current) => ({
            ...current,
            sortKey: nextKey,
            sortDir:
                current.sortKey === nextKey
                    ? current.sortDir === "asc"
                        ? "desc"
                        : "asc"
                    : defaultDir,
        }));
    }, [setViewState]);

    const renderColHeader: MatrixProps<RowMeta, ColMeta>["renderColHeader"] = (col) => {
        const meta = col.meta!;
        const active = meta.sortKey === sortKey;
        const icon = active ? (sortDir === "asc" ? "▲" : "▼") : "";

        return (
            <div className="group relative">
                <button
                    type="button"
                    className="w-full px-1 py-1 pr-6 text-[10px] text-left select-none"
                    onClick={() => handleSort(meta.sortKey, meta.defaultDir)}
                    title={buildColumnTooltip(meta, active, sortDir)}
                >
                    <span className="truncate">{meta.title}</span>
                    {icon ? <span className="ml-1 text-[9px] text-textDim/70">{icon}</span> : null}
                </button>

                <DocsAnchorButton
                    anchorId={getAlgoRankingColumnAnchorId(meta.docsColumnId ?? (meta.id as AlgoRankingDocsColumnKey))}
                    label={`${ALGO_RANKING_TABLE_DOCS.title}: ${meta.title}`}
                    className="absolute right-1 top-1"
                />
            </div>
        );
    };

    const renderRowHeader: MatrixProps<RowMeta, ColMeta>["renderRowHeader"] = (row) => {
        const stats = row.meta!;
        const title = [
            stats.algorithmName,
            stats.argsSummary ? `args: ${stats.argsSummary}` : "",
        ]
            .filter(Boolean)
            .join("\n");

        return (
            <div className="leading-tight" title={title}>
                <div className="flex items-baseline justify-between gap-2">
                    <div className="max-w-[230px] truncate text-textDim">{stats.baseAlgorithmName}</div>
                    <div className="font-mono text-[10px] text-textDim/70">#{stats.place}</div>
                </div>
                <div className="flex flex-wrap items-center gap-x-2 gap-y-[1px] text-[9px] text-textDim/60">
                    <span>{stats.m != null ? `m=${String(stats.m)}` : "m=-"}</span>
                    <span>prec={stats.precisionLabel}</span>
                    <span
                        className={
                            stats.variant === "filtered"
                                ? "rounded border border-border bg-panel px-1 text-[8px] text-accent"
                                : "rounded border border-border bg-surface px-1 text-[8px]"
                        }
                    >
                        {stats.variant}
                    </span>
                </div>
                {stats.filteredMethodName ? (
                    <div className="mt-[1px] max-w-[290px] truncate text-[8px] text-textDim/70">
                        {stats.filteredMethodName}
                    </div>
                ) : null}
            </div>
        );
    };

    const renderCell: MatrixProps<RowMeta, ColMeta>["renderCell"] = (row, col) => {
        const stats = row.meta!;
        const meta = col.meta!;
        const colId = meta.id;
        const text = getCellText(stats, colId);

        return (
            <div
                title={`${stats.algorithmName}\n${meta.title} = ${text}`}
                className="w-full h-full px-2 py-[2px] text-[10px] text-textDim font-mono tabular-nums"
            >
                <span className="block truncate">{text}</span>
            </div>
        );
    };

    const buildWorkbook = useCallback(
        ({
            rows,
            cols,
        }: {
            rows: MatrixAxisItem<RowMeta>[];
            cols: MatrixAxisItem<ColMeta>[];
            pager: { startIndex: number; endIndex: number };
        }): XLSX.WorkBook => {
            const workbook = createWorkbook("Algorithm ranking", "Algorithm ranking export");

            appendSheet(
                workbook,
                buildKeyValueSheet([
                    { key: "epsilon", value: epsilon },
                    { key: "epsilon exponent", value: epsilonExp },
                    { key: "precision filter", value: precisionFilter ?? "all" },
                    { key: "sort key", value: sortKey },
                    { key: "sort dir", value: sortDir },
                    { key: "rows", value: rows.length },
                    { key: "columns", value: cols.length + 2 },
                ]),
                "overview"
            );

            const header: (string | number | boolean | null)[] = ["place", "algorithm"];
            for (const col of cols) header.push(col.meta ? getExportColumnName(col.meta) : String(col.id));

            const data: (string | number | boolean | null)[][] = [header];

            for (const row of rows) {
                const stats = row.meta!;
                const line: (string | number | null)[] = [stats.place, stats.algorithmName];

                for (const col of cols) {
                    line.push(getExportValue(stats, col.meta!.id));
                }

                data.push(line);
            }

            const worksheet = buildSheetFromAoa(data, {
                cols: [
                    { wch: 8 },
                    { wch: 28 },
                    ...cols.map((col) => ({ wch: getExportColumnWidth(col.meta!.id) })),
                ],
                headerRows: 1,
                rowHeaderCols: 2,
                decorateCell: ({ rowIndex, colIndex, cell }) => {
                    if (rowIndex === 0) return;
                    if (colIndex === 0) {
                        cell.z = "0";
                        return;
                    }
                    if (colIndex === 1) return;

                    const meta = cols[colIndex - 2]?.meta;
                    if (!meta) return;
                    const format = getExportColumnFormat(meta.id);
                    if (format && typeof cell.v === "number") cell.z = format;
                },
            });

            appendSheet(workbook, worksheet, "algo_ranking");
            return workbook;
        },
        [epsilon, epsilonExp, precisionFilter, sortDir, sortKey]
    );

    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim/80">
                    No data: experiment is empty or has no seriesAccelList.
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            <div className="mb-2 flex items-center gap-4 text-xs text-textDim">
                <div className="flex flex-col gap-1">
                    <label className="font-medium text-text">Tolerance epsilon</label>
                    <div className="flex items-baseline gap-2 font-mono">
                        <span>
                            epsilon = 10<sup>{epsilonExp}</sup>
                        </span>
                        <span className="text-textDim/80">~ {epsilon.toExponential(2)}</span>
                    </div>
                </div>

                <div className="flex-1">
                    <input
                        type="range"
                        min={-1000}
                        max={-1}
                        step={1}
                        value={epsilonExp}
                        onChange={(event) =>
                            setViewState((current) => ({
                                ...current,
                                epsilonExp: parseInt(event.target.value, 10),
                            }))
                        }
                        className="w-full"
                    />
                    <div className="mt-1 flex justify-between text-[10px]">
                        <span>10^-1000</span>
                        <span>10^-500</span>
                        <span>10^-1</span>
                    </div>
                </div>

                <div className="flex items-center gap-2 text-[10px]">
                    <span>precision:</span>
                    <select
                        className="rounded border border-border bg-surface px-1 py-[1px]"
                        value={precisionFilter ?? ""}
                        onChange={(event) =>
                            setViewState((current) => ({
                                ...current,
                                precisionFilter:
                                    event.target.value === "" ? null : event.target.value,
                            }))
                        }
                    >
                        <option value="">all</option>
                        {precisionsOrder.map((precision) => (
                            <option key={precision} value={precision}>
                                {precision}
                            </option>
                        ))}
                    </select>
                </div>
            </div>

            <MatrixAlgoSeriesFilter
                accelList={experiment.accelList ?? []}
                seriesList={experiment.seriesList ?? []}
                resetKey={experiment.id}
                sessionKey={sharedFilterSessionKey}
            >
                {({ filteredAccels, filteredSeries, state }) => {
                    const filteredSeriesIds = new Set(filteredSeries.map((series) => series.id));
                    const filteredAccelIds = new Set(filteredAccels.map((accel) => accel.id));

                    const baseStats = buildAlgoStatsFromExperiment(
                        experiment,
                        epsilon,
                        precisionFilter,
                        filteredSeriesIds,
                        filteredAccelIds
                    );

                    if (!baseStats.length) {
                        return (
                            <div className="rounded-xl2 border border-border bg-panel p-3 text-[11px] text-textDim/70">
                                No rows for current filters.
                            </div>
                        );
                    }

                    const argKeys = getVisibleArgKeys(baseStats);
                    const colsAxis: MatrixAxisItem<ColMeta>[] = buildColumns(argKeys).map((col) => ({
                        id: col.id,
                        meta: col,
                    }));

                    const sortedStats = [...baseStats].sort((a, b) =>
                        compareValues(getSortValue(a, sortKey), getSortValue(b, sortKey), sortDir)
                    );

                    const rowsAxis: MatrixAxisItem<RowMeta>[] = sortedStats.map((stats, index) => ({
                        id: stats.algoKey,
                        meta: {
                            ...stats,
                            place: index + 1,
                            precisionLabel: stats.precision ?? (precisionFilter ? precisionFilter : "-"),
                        },
                    }));

                    const stateKey = buildFilterStateKey(state);

                    return (
                        <>
                            <div className="mb-2 text-[10px] text-textDim/70">
                                rows: {rowsAxis.length} / {totalStats.length} · accels:{" "}
                                {filteredAccels.length} / {(experiment.accelList ?? []).length} ·
                                series: {filteredSeries.length} / {(experiment.seriesList ?? []).length}
                            </div>

                            <MatrixPaged<RowMeta, ColMeta>
                                resetKey={`${experiment.id}::${epsilonExp}::${precisionFilter ?? "ALL"}::${sortKey}:${sortDir}::${stateKey}::args=${argKeys.join(",")}`}
                                rows={rowsAxis}
                                cols={colsAxis}
                                maxColsPerPage={0}
                                enableInnerScroll
                                maxBodyHeight="70vh"
                                stickyHeaders
                                rowWidth={320}
                                colWidth={96}
                                minCellHeightPx={46}
                                className="rounded-xl2 border border-border bg-panel shadow-panel"
                                tableClassName="border-separate border-spacing-0"
                                thClassName="bg-surface"
                                tdClassName="p-0"
                                renderTitle={() => (
                                    <span className="group inline-flex items-center gap-2">
                                        <span>Algorithm ranking</span>
                                        <DocsAnchorButton
                                            anchorId={ALGO_RANKING_TABLE_DOCS.id}
                                            label={ALGO_RANKING_TABLE_DOCS.title}
                                        />
                                    </span>
                                )}
                                renderSubtitle={() =>
                                    precisionFilter
                                        ? `epsilon=${epsilon.toExponential(2)} · precision=${precisionFilter} · N=${rowsAxis.length}`
                                        : `epsilon=${epsilon.toExponential(2)} · precision=all · N=${rowsAxis.length}`
                                }
                                renderHeaderRight={() => (
                                    <div className="whitespace-nowrap text-[10px] text-textDim/70">
                                        sort: {formatSortKey(sortKey)} ({sortDir})
                                    </div>
                                )}
                                export={{
                                    fileBaseName: "algo-ranking",
                                    enablePng: true,
                                    enableXlsx: true,
                                    buildWorkbook,
                                }}
                                renderCorner={() => <span className="text-left">Algorithm</span>}
                                renderRowHeader={renderRowHeader}
                                renderColHeader={renderColHeader}
                                renderCell={renderCell}
                                emptyFallback={
                                    <div className="rounded-xl2 border border-border bg-panel p-3 text-[11px] text-textDim/70">
                                        No data
                                    </div>
                                }
                            />
                        </>
                    );
                }}
            </MatrixAlgoSeriesFilter>
        </div>
    );
};
