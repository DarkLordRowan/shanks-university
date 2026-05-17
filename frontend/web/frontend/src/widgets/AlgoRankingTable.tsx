import React, { useCallback, useEffect, useMemo, useState } from "react";
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
    getVisibleArgKeys,
    type AlgoRankingSortKey as BaseSortKey,
    type AlgoStats,
} from "./AlgoRankingTable.model";
import { useAlgoRankingStats } from "./AlgoRankingTable.useAlgoRankingStats";
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
const EPSILON_EXP_MIN = -1000;
const EPSILON_EXP_MAX = -1;

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
        id: "levinEnding",
        title: "Levin end",
        description: "Levin-family ending extracted from the algorithm name or remainder args: U, T, T~, V, or V~.",
        sortKey: "levinEnding",
        defaultDir: "asc",
    },
    {
        id: "howMuchFormula",
        title: "how_much formula",
        description:
            "Formula used to map algorithm step n to the source sequence index S(how_much(n, order, args)).",
        sortKey: "howMuchFormula",
        defaultDir: "asc",
    },
    {
        id: "complexityFormula",
        title: "complexity formula",
        description:
            "Formula used for complexity score: how_much(n) plus the algorithm O(...) evaluated at current m and n.",
        sortKey: "complexityFormula",
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
        id: "medianBestDeviation",
        title: "med min |dev|",
        description: "Median over series of min |A_n - lim| for the algorithm. Lower is better.",
        sortKey: "medianBestDeviation",
        defaultDir: "asc",
    },
    {
        id: "worstBestDeviation",
        title: "worst min |dev|",
        description: "Worst over series of min |A_n - lim| for the algorithm. Lower is better.",
        sortKey: "worstBestDeviation",
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
        id: "medianRelativeError",
        title: "med rel error",
        description: "Median relative error min |A_n-lim| / min |S_n-lim|. Lower is better.",
        sortKey: "medianRelativeError",
        defaultDir: "asc",
    },
    {
        id: "worstRelativeError",
        title: "worst rel error",
        description: "Worst relative error min |A_n-lim| / min |S_n-lim|. Lower is better.",
        sortKey: "worstRelativeError",
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
        id: "medianOrdersGain",
        title: "med series/algo amp",
        description: "Median order gain log10(min series) - log10(min algorithm). Higher is better.",
        sortKey: "medianOrdersGain",
        defaultDir: "desc",
    },
    {
        id: "worstOrdersGain",
        title: "worst series/algo amp",
        description: "Worst order gain log10(min series) - log10(min algorithm). Higher is better.",
        sortKey: "worstOrdersGain",
        defaultDir: "desc",
    },
    {
        id: "avgAmpAtMinN",
        title: "avg series@min n/algo amp",
        description:
            "Средний выигрыш по порядкам на первом n, где алгоритм достиг своего min |A_n - lim|, с рядом в точке S(how_much(n)). Больше лучше.",
        sortKey: "avgAmpAtMinN",
        defaultDir: "desc",
    },
    {
        id: "medianAmpAtMinN",
        title: "med series@min n/algo amp",
        description: "Median order gain against S(how_much(n_min)). Higher is better.",
        sortKey: "medianAmpAtMinN",
        defaultDir: "desc",
    },
    {
        id: "worstAmpAtMinN",
        title: "worst series@min n/algo amp",
        description: "Worst order gain against S(how_much(n_min)). Higher is better.",
        sortKey: "worstAmpAtMinN",
        defaultDir: "desc",
    },
    {
        id: "avgStepSeriesAmp",
        title: "avg step S*/A-S amp",
        description:
            "Average per-series gain over accelerated steps: log10(|S_how_much(n)-S|) - log10(|A_n - S_how_much(n)|). Higher is better.",
        sortKey: "avgStepSeriesAmp",
        defaultDir: "desc",
    },
    {
        id: "medianStepSeriesAmp",
        title: "med step S*/A-S amp",
        description:
            "Median per-series gain over accelerated steps against |A_n - S_how_much(n)|. Higher is better.",
        sortKey: "medianStepSeriesAmp",
        defaultDir: "desc",
    },
    {
        id: "worstStepSeriesAmp",
        title: "worst step S*/A-S amp",
        description:
            "Worst per-series gain over accelerated steps against |A_n - S_how_much(n)|. Higher is better.",
        sortKey: "worstStepSeriesAmp",
        defaultDir: "desc",
    },
    {
        id: "avgFilterTriggerN",
        title: "avg filter n",
        description: "Average first event n where filters were triggered. Lower is better.",
        sortKey: "avgFilterTriggerN",
        defaultDir: "asc",
    },
    {
        id: "medianFilterTriggerN",
        title: "med filter n",
        description: "Median first event n where filters were triggered. Lower is better.",
        sortKey: "medianFilterTriggerN",
        defaultDir: "asc",
    },
    {
        id: "worstFilterTriggerN",
        title: "worst filter n",
        description: "Worst first event n where filters were triggered. Lower is better.",
        sortKey: "worstFilterTriggerN",
        defaultDir: "asc",
    },
    {
        id: "avgFilterTriggerDeltaFromMinN",
        title: "avg filter-min n",
        description: "Average filter trigger n minus min deviation n. Lower is better.",
        sortKey: "avgFilterTriggerDeltaFromMinN",
        defaultDir: "asc",
    },
    {
        id: "medianFilterTriggerDeltaFromMinN",
        title: "med filter-min n",
        description: "Median filter trigger n minus min deviation n. Lower is better.",
        sortKey: "medianFilterTriggerDeltaFromMinN",
        defaultDir: "asc",
    },
    {
        id: "worstFilterTriggerDeltaFromMinN",
        title: "worst filter-min n",
        description: "Worst filter trigger n minus min deviation n. Lower is better.",
        sortKey: "worstFilterTriggerDeltaFromMinN",
        defaultDir: "asc",
    },
    {
        id: "avgFilterTriggerLossAmp",
        title: "avg filter loss amp",
        description:
            "Average log10(|A_filter_n-S|) - log10(min |A_n-S|). It shows how many orders were lost when filters triggered. Lower is better.",
        sortKey: "avgFilterTriggerLossAmp",
        defaultDir: "asc",
    },
    {
        id: "medianFilterTriggerLossAmp",
        title: "med filter loss amp",
        description:
            "Median log10(|A_filter_n-S|) - log10(min |A_n-S|). Lower is better.",
        sortKey: "medianFilterTriggerLossAmp",
        defaultDir: "asc",
    },
    {
        id: "worstFilterTriggerLossAmp",
        title: "worst filter loss amp",
        description:
            "Worst log10(|A_filter_n-S|) - log10(min |A_n-S|). Lower is better.",
        sortKey: "worstFilterTriggerLossAmp",
        defaultDir: "asc",
    },
    {
        id: "avgFilterTriggerLossDiff",
        title: "avg filter loss diff",
        description: "Average |A_filter_n-S| - min |A_n-S| when filters triggered. Lower is better.",
        sortKey: "avgFilterTriggerLossDiff",
        defaultDir: "asc",
    },
    {
        id: "medianFilterTriggerLossDiff",
        title: "med filter loss diff",
        description: "Median |A_filter_n-S| - min |A_n-S| when filters triggered. Lower is better.",
        sortKey: "medianFilterTriggerLossDiff",
        defaultDir: "asc",
    },
    {
        id: "worstFilterTriggerLossDiff",
        title: "worst filter loss diff",
        description: "Worst |A_filter_n-S| - min |A_n-S| when filters triggered. Lower is better.",
        sortKey: "worstFilterTriggerLossDiff",
        defaultDir: "asc",
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
        id: "medianMinDeviationN",
        title: "med min dev n",
        description: "Median n where the algorithm reaches min |A_n-lim|. Lower is better.",
        sortKey: "medianMinDeviationN",
        defaultDir: "asc",
    },
    {
        id: "worstMinDeviationN",
        title: "worst min dev n",
        description: "Worst n where the algorithm reaches min |A_n-lim|. Lower is better.",
        sortKey: "worstMinDeviationN",
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
        id: "medianLastMinusMin",
        title: "med last-min",
        description: "Median last-min gap. Lower is better.",
        sortKey: "medianLastMinusMin",
        defaultDir: "asc",
    },
    {
        id: "worstLastMinusMin",
        title: "worst last-min",
        description: "Worst last-min gap. Lower is better.",
        sortKey: "worstLastMinusMin",
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
        id: "medianStepsToTol",
        title: "med steps to eps",
        description: "Median first n where |A_n-lim| <= epsilon. Lower is better.",
        sortKey: "medianStepsToTol",
        defaultDir: "asc",
    },
    {
        id: "worstStepsToTol",
        title: "worst steps to eps",
        description: "Worst first n where |A_n-lim| <= epsilon. Lower is better.",
        sortKey: "worstStepsToTol",
        defaultDir: "asc",
    },
    {
        id: "avgEpsSavedSteps",
        title: "avg eps saved steps",
        description:
            "Average n_series_eps - how_much(n_algo_eps, order, args). Higher means the algorithm saves more source sequence terms.",
        sortKey: "avgEpsSavedSteps",
        defaultDir: "desc",
    },
    {
        id: "medianEpsSavedSteps",
        title: "med eps saved steps",
        description:
            "Median n_series_eps - how_much(n_algo_eps, order, args). Higher is better.",
        sortKey: "medianEpsSavedSteps",
        defaultDir: "desc",
    },
    {
        id: "worstEpsSavedSteps",
        title: "worst eps saved steps",
        description:
            "Worst n_series_eps - how_much(n_algo_eps, order, args). Higher is better.",
        sortKey: "worstEpsSavedSteps",
        defaultDir: "desc",
    },
    {
        id: "avgMinDeviationNComplexity",
        title: "avg min dev complexity",
        description: "complexity(avg min dev n) = how_much(n) + evaluated O(k,n). Lower is better.",
        sortKey: "avgMinDeviationNComplexity",
        defaultDir: "asc",
    },
    {
        id: "medianMinDeviationNComplexity",
        title: "med min dev complexity",
        description: "complexity(med min dev n) = how_much(n) + evaluated O(k,n). Lower is better.",
        sortKey: "medianMinDeviationNComplexity",
        defaultDir: "asc",
    },
    {
        id: "avgStepsToTolComplexity",
        title: "avg eps complexity",
        description: "complexity(avg steps to eps) = how_much(n) + evaluated O(k,n). Lower is better.",
        sortKey: "avgStepsToTolComplexity",
        defaultDir: "asc",
    },
    {
        id: "medianStepsToTolComplexity",
        title: "med eps complexity",
        description: "complexity(med steps to eps) = how_much(n) + evaluated O(k,n). Lower is better.",
        sortKey: "medianStepsToTolComplexity",
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
        id: "divZeroShare",
        title: "div0, %",
        description: "Share of processed runs that reported a division-by-zero error. Lower is better.",
        sortKey: "divZeroShare",
        defaultDir: "asc",
    },
    {
        id: "avgDivZeroFirstN",
        title: "avg div0 first n",
        description: "Average first finite n where a division-by-zero error appeared. Higher means later failure.",
        sortKey: "avgDivZeroFirstN",
        defaultDir: "desc",
    },
    {
        id: "medianDivZeroFirstN",
        title: "med div0 first n",
        description: "Median first finite n where a division-by-zero error appeared. Higher means later failure.",
        sortKey: "medianDivZeroFirstN",
        defaultDir: "desc",
    },
    {
        id: "worstDivZeroFirstN",
        title: "worst div0 first n",
        description: "Worst first finite n for division-by-zero errors across runs. Higher means later failure.",
        sortKey: "worstDivZeroFirstN",
        defaultDir: "desc",
    },
    {
        id: "divZeroRecoveredShare",
        title: "div0 recovered, %",
        description: "Share of finite-n div0 runs that later produced a successful computed point. Higher is better.",
        sortKey: "divZeroRecoveredShare",
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
        id: "rankCost",
        title: "rank cost",
        description:
            "Standalone cost rank built from dense ranks of the four complexity columns. Lower is better and it is not included in total rank.",
        sortKey: "rankCost",
        defaultDir: "asc",
    },
    {
        id: "rankPrecision",
        title: "rank precision",
        description:
            "Итоговый rank по точности: median/worst min |dev|, rel error, series/algo amp, step S*/A-S amp, plus min-div shares. Меньше лучше.",
        sortKey: "rankPrecision",
        defaultDir: "asc",
    },
    {
        id: "rankSpeed",
        title: "rank speed",
        description:
            "Итоговый rank по скорости: median/worst steps to eps, min dev n, eps saved steps, series@min n/algo amp, filter n, filter-min n, and filter loss amp. Меньше лучше.",
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
        BASE_COLUMNS[2],
        ...argKeys.map(buildArgColumn),
        ...BASE_COLUMNS.slice(3),
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
        case "levinEnding":
            return row.levinEnding || "-";
        case "howMuchFormula":
            return row.howMuchFormula;
        case "complexityFormula":
            return row.complexityFormula;
        case "args":
            return row.argsSummary || "-";
        case "seriesCount":
            return String(row.seriesCount);
        case "avgBestDeviation":
            return formatNumber(row.avgBestDeviation);
        case "medianBestDeviation":
            return formatNumber(row.medianBestDeviation);
        case "worstBestDeviation":
            return formatNumber(row.worstBestDeviation);
        case "avgRelativeError":
            return formatNumber(row.avgRelativeError);
        case "medianRelativeError":
            return formatNumber(row.medianRelativeError);
        case "worstRelativeError":
            return formatNumber(row.worstRelativeError);
        case "avgOrdersGain":
            return formatNumber(row.avgOrdersGain);
        case "medianOrdersGain":
            return formatNumber(row.medianOrdersGain);
        case "worstOrdersGain":
            return formatNumber(row.worstOrdersGain);
        case "avgAmpAtMinN":
            return formatNumber(row.avgAmpAtMinN);
        case "medianAmpAtMinN":
            return formatNumber(row.medianAmpAtMinN);
        case "worstAmpAtMinN":
            return formatNumber(row.worstAmpAtMinN);
        case "avgStepSeriesAmp":
            return formatNumber(row.avgStepSeriesAmp);
        case "medianStepSeriesAmp":
            return formatNumber(row.medianStepSeriesAmp);
        case "worstStepSeriesAmp":
            return formatNumber(row.worstStepSeriesAmp);
        case "avgFilterTriggerN":
            return formatSteps(row.avgFilterTriggerN);
        case "medianFilterTriggerN":
            return formatSteps(row.medianFilterTriggerN);
        case "worstFilterTriggerN":
            return formatSteps(row.worstFilterTriggerN);
        case "avgFilterTriggerDeltaFromMinN":
            return formatSteps(row.avgFilterTriggerDeltaFromMinN);
        case "medianFilterTriggerDeltaFromMinN":
            return formatSteps(row.medianFilterTriggerDeltaFromMinN);
        case "worstFilterTriggerDeltaFromMinN":
            return formatSteps(row.worstFilterTriggerDeltaFromMinN);
        case "avgFilterTriggerLossAmp":
            return formatNumber(row.avgFilterTriggerLossAmp);
        case "medianFilterTriggerLossAmp":
            return formatNumber(row.medianFilterTriggerLossAmp);
        case "worstFilterTriggerLossAmp":
            return formatNumber(row.worstFilterTriggerLossAmp);
        case "avgFilterTriggerLossDiff":
            return formatNumber(row.avgFilterTriggerLossDiff);
        case "medianFilterTriggerLossDiff":
            return formatNumber(row.medianFilterTriggerLossDiff);
        case "worstFilterTriggerLossDiff":
            return formatNumber(row.worstFilterTriggerLossDiff);
        case "notBetterThanSeriesShare":
            return row.seriesCount > 0 ? formatPercent(row.notBetterThanSeriesShare) : "-";
        case "avgMinDeviationN":
            return formatSteps(row.avgMinDeviationN);
        case "medianMinDeviationN":
            return formatSteps(row.medianMinDeviationN);
        case "worstMinDeviationN":
            return formatSteps(row.worstMinDeviationN);
        case "avgLastMinusMin":
            return formatNumber(row.avgLastMinusMin);
        case "medianLastMinusMin":
            return formatNumber(row.medianLastMinusMin);
        case "worstLastMinusMin":
            return formatNumber(row.worstLastMinusMin);
        case "avgStepsToTol":
            return formatSteps(row.avgStepsToTol);
        case "medianStepsToTol":
            return formatSteps(row.medianStepsToTol);
        case "worstStepsToTol":
            return formatSteps(row.worstStepsToTol);
        case "avgEpsSavedSteps":
            return formatSteps(row.avgEpsSavedSteps);
        case "medianEpsSavedSteps":
            return formatSteps(row.medianEpsSavedSteps);
        case "worstEpsSavedSteps":
            return formatSteps(row.worstEpsSavedSteps);
        case "avgMinDeviationNComplexity":
            return formatNumber(row.avgMinDeviationNComplexity);
        case "medianMinDeviationNComplexity":
            return formatNumber(row.medianMinDeviationNComplexity);
        case "avgStepsToTolComplexity":
            return formatNumber(row.avgStepsToTolComplexity);
        case "medianStepsToTolComplexity":
            return formatNumber(row.medianStepsToTolComplexity);
        case "fracReachedTol":
            return row.seriesCount > 0 ? formatPercent(row.fracReachedTol) : "-";
        case "divZeroShare":
            return row.runCount > 0 ? formatPercent(row.divZeroShare) : "-";
        case "avgDivZeroFirstN":
            return formatSteps(row.avgDivZeroFirstN);
        case "medianDivZeroFirstN":
            return formatSteps(row.medianDivZeroFirstN);
        case "worstDivZeroFirstN":
            return formatSteps(row.worstDivZeroFirstN);
        case "divZeroRecoveredShare":
            return row.divZeroFiniteNRunCount > 0 ? formatPercent(row.divZeroRecoveredShare) : "-";
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
        case "rankCost":
            return String(row.rankCost);
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
        case "levinEnding":
            return row.levinEnding || null;
        case "howMuchFormula":
            return row.howMuchFormula;
        case "complexityFormula":
            return row.complexityFormula;
        case "args":
            return row.argsSummary || null;
        case "seriesCount":
            return row.seriesCount;
        case "avgBestDeviation":
            return Number.isFinite(row.avgBestDeviation) ? row.avgBestDeviation : null;
        case "medianBestDeviation":
            return Number.isFinite(row.medianBestDeviation) ? row.medianBestDeviation : null;
        case "worstBestDeviation":
            return Number.isFinite(row.worstBestDeviation) ? row.worstBestDeviation : null;
        case "avgRelativeError":
            return Number.isFinite(row.avgRelativeError)
                ? row.avgRelativeError
                : row.avgRelativeError === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "medianRelativeError":
            return Number.isFinite(row.medianRelativeError)
                ? row.medianRelativeError
                : row.medianRelativeError === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "worstRelativeError":
            return Number.isFinite(row.worstRelativeError)
                ? row.worstRelativeError
                : row.worstRelativeError === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "avgOrdersGain":
            return Number.isFinite(row.avgOrdersGain)
                ? row.avgOrdersGain
                : row.avgOrdersGain === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "medianOrdersGain":
            return Number.isFinite(row.medianOrdersGain)
                ? row.medianOrdersGain
                : row.medianOrdersGain === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "worstOrdersGain":
            return Number.isFinite(row.worstOrdersGain)
                ? row.worstOrdersGain
                : row.worstOrdersGain === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "avgAmpAtMinN":
            return Number.isFinite(row.avgAmpAtMinN)
                ? row.avgAmpAtMinN
                : row.avgAmpAtMinN === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "medianAmpAtMinN":
            return Number.isFinite(row.medianAmpAtMinN)
                ? row.medianAmpAtMinN
                : row.medianAmpAtMinN === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "worstAmpAtMinN":
            return Number.isFinite(row.worstAmpAtMinN)
                ? row.worstAmpAtMinN
                : row.worstAmpAtMinN === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "avgStepSeriesAmp":
            return Number.isFinite(row.avgStepSeriesAmp) ? row.avgStepSeriesAmp : null;
        case "medianStepSeriesAmp":
            return Number.isFinite(row.medianStepSeriesAmp) ? row.medianStepSeriesAmp : null;
        case "worstStepSeriesAmp":
            return Number.isFinite(row.worstStepSeriesAmp) ? row.worstStepSeriesAmp : null;
        case "avgFilterTriggerN":
            return Number.isFinite(row.avgFilterTriggerN) ? row.avgFilterTriggerN : null;
        case "medianFilterTriggerN":
            return Number.isFinite(row.medianFilterTriggerN) ? row.medianFilterTriggerN : null;
        case "worstFilterTriggerN":
            return Number.isFinite(row.worstFilterTriggerN) ? row.worstFilterTriggerN : null;
        case "avgFilterTriggerDeltaFromMinN":
            return Number.isFinite(row.avgFilterTriggerDeltaFromMinN)
                ? row.avgFilterTriggerDeltaFromMinN
                : null;
        case "medianFilterTriggerDeltaFromMinN":
            return Number.isFinite(row.medianFilterTriggerDeltaFromMinN)
                ? row.medianFilterTriggerDeltaFromMinN
                : null;
        case "worstFilterTriggerDeltaFromMinN":
            return Number.isFinite(row.worstFilterTriggerDeltaFromMinN)
                ? row.worstFilterTriggerDeltaFromMinN
                : null;
        case "avgFilterTriggerLossAmp":
            return Number.isFinite(row.avgFilterTriggerLossAmp)
                ? row.avgFilterTriggerLossAmp
                : row.avgFilterTriggerLossAmp === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "medianFilterTriggerLossAmp":
            return Number.isFinite(row.medianFilterTriggerLossAmp)
                ? row.medianFilterTriggerLossAmp
                : row.medianFilterTriggerLossAmp === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "worstFilterTriggerLossAmp":
            return Number.isFinite(row.worstFilterTriggerLossAmp)
                ? row.worstFilterTriggerLossAmp
                : row.worstFilterTriggerLossAmp === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "avgFilterTriggerLossDiff":
            return Number.isFinite(row.avgFilterTriggerLossDiff)
                ? row.avgFilterTriggerLossDiff
                : row.avgFilterTriggerLossDiff === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "medianFilterTriggerLossDiff":
            return Number.isFinite(row.medianFilterTriggerLossDiff)
                ? row.medianFilterTriggerLossDiff
                : row.medianFilterTriggerLossDiff === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "worstFilterTriggerLossDiff":
            return Number.isFinite(row.worstFilterTriggerLossDiff)
                ? row.worstFilterTriggerLossDiff
                : row.worstFilterTriggerLossDiff === Number.POSITIVE_INFINITY
                  ? "∞"
                  : null;
        case "notBetterThanSeriesShare":
            return Number.isFinite(row.notBetterThanSeriesShare)
                ? row.notBetterThanSeriesShare
                : null;
        case "avgMinDeviationN":
            return Number.isFinite(row.avgMinDeviationN) ? row.avgMinDeviationN : null;
        case "medianMinDeviationN":
            return Number.isFinite(row.medianMinDeviationN) ? row.medianMinDeviationN : null;
        case "worstMinDeviationN":
            return Number.isFinite(row.worstMinDeviationN) ? row.worstMinDeviationN : null;
        case "avgLastMinusMin":
            return Number.isFinite(row.avgLastMinusMin) ? row.avgLastMinusMin : null;
        case "medianLastMinusMin":
            return Number.isFinite(row.medianLastMinusMin) ? row.medianLastMinusMin : null;
        case "worstLastMinusMin":
            return Number.isFinite(row.worstLastMinusMin) ? row.worstLastMinusMin : null;
        case "avgStepsToTol":
            return Number.isFinite(row.avgStepsToTol) ? row.avgStepsToTol : null;
        case "medianStepsToTol":
            return Number.isFinite(row.medianStepsToTol) ? row.medianStepsToTol : null;
        case "worstStepsToTol":
            return Number.isFinite(row.worstStepsToTol) ? row.worstStepsToTol : null;
        case "avgEpsSavedSteps":
            return Number.isFinite(row.avgEpsSavedSteps)
                ? row.avgEpsSavedSteps
                : row.avgEpsSavedSteps === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "medianEpsSavedSteps":
            return Number.isFinite(row.medianEpsSavedSteps)
                ? row.medianEpsSavedSteps
                : row.medianEpsSavedSteps === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "worstEpsSavedSteps":
            return Number.isFinite(row.worstEpsSavedSteps)
                ? row.worstEpsSavedSteps
                : row.worstEpsSavedSteps === Number.NEGATIVE_INFINITY
                  ? "-∞"
                  : null;
        case "avgMinDeviationNComplexity":
            return Number.isFinite(row.avgMinDeviationNComplexity)
                ? row.avgMinDeviationNComplexity
                : null;
        case "medianMinDeviationNComplexity":
            return Number.isFinite(row.medianMinDeviationNComplexity)
                ? row.medianMinDeviationNComplexity
                : null;
        case "avgStepsToTolComplexity":
            return Number.isFinite(row.avgStepsToTolComplexity)
                ? row.avgStepsToTolComplexity
                : null;
        case "medianStepsToTolComplexity":
            return Number.isFinite(row.medianStepsToTolComplexity)
                ? row.medianStepsToTolComplexity
                : null;
        case "fracReachedTol":
            return Number.isFinite(row.fracReachedTol) ? row.fracReachedTol : null;
        case "divZeroShare":
            return Number.isFinite(row.divZeroShare) ? row.divZeroShare : null;
        case "avgDivZeroFirstN":
            return Number.isFinite(row.avgDivZeroFirstN) ? row.avgDivZeroFirstN : null;
        case "medianDivZeroFirstN":
            return Number.isFinite(row.medianDivZeroFirstN) ? row.medianDivZeroFirstN : null;
        case "worstDivZeroFirstN":
            return Number.isFinite(row.worstDivZeroFirstN) ? row.worstDivZeroFirstN : null;
        case "divZeroRecoveredShare":
            return Number.isFinite(row.divZeroRecoveredShare) ? row.divZeroRecoveredShare : null;
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
        case "rankCost":
            return row.rankCost;
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
        case "levinEnding":
            return 12;
        case "howMuchFormula":
            return 34;
        case "complexityFormula":
            return 34;
        case "args":
            return 28;
        case "seriesCount":
            return 10;
        case "avgBestDeviation":
        case "medianBestDeviation":
        case "worstBestDeviation":
        case "avgRelativeError":
        case "medianRelativeError":
        case "worstRelativeError":
        case "avgOrdersGain":
        case "medianOrdersGain":
        case "worstOrdersGain":
        case "avgAmpAtMinN":
        case "medianAmpAtMinN":
        case "worstAmpAtMinN":
        case "avgStepSeriesAmp":
        case "medianStepSeriesAmp":
        case "worstStepSeriesAmp":
        case "avgFilterTriggerN":
        case "medianFilterTriggerN":
        case "worstFilterTriggerN":
        case "avgFilterTriggerDeltaFromMinN":
        case "medianFilterTriggerDeltaFromMinN":
        case "worstFilterTriggerDeltaFromMinN":
        case "avgFilterTriggerLossAmp":
        case "medianFilterTriggerLossAmp":
        case "worstFilterTriggerLossAmp":
        case "avgFilterTriggerLossDiff":
        case "medianFilterTriggerLossDiff":
        case "worstFilterTriggerLossDiff":
        case "notBetterThanSeriesShare":
        case "avgMinDeviationN":
        case "medianMinDeviationN":
        case "worstMinDeviationN":
        case "avgLastMinusMin":
        case "medianLastMinusMin":
        case "worstLastMinusMin":
        case "avgStepsToTol":
        case "medianStepsToTol":
        case "worstStepsToTol":
        case "avgEpsSavedSteps":
        case "medianEpsSavedSteps":
        case "worstEpsSavedSteps":
        case "avgMinDeviationNComplexity":
        case "medianMinDeviationNComplexity":
        case "avgStepsToTolComplexity":
        case "medianStepsToTolComplexity":
        case "fracReachedTol":
        case "divZeroShare":
        case "avgDivZeroFirstN":
        case "medianDivZeroFirstN":
        case "worstDivZeroFirstN":
        case "divZeroRecoveredShare":
        case "oneSidedShare":
        case "bestMinShare":
        case "worstMinShare":
        case "bestLastShare":
        case "worstLastShare":
            return 16;
        case "rankPrecision":
        case "rankCost":
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
        case "divZeroShare":
        case "divZeroRecoveredShare":
        case "oneSidedShare":
        case "bestMinShare":
        case "worstMinShare":
        case "bestLastShare":
        case "worstLastShare":
            return "0.0%";
        case "avgBestDeviation":
        case "medianBestDeviation":
        case "worstBestDeviation":
        case "avgRelativeError":
        case "medianRelativeError":
        case "worstRelativeError":
        case "avgLastMinusMin":
        case "medianLastMinusMin":
        case "worstLastMinusMin":
        case "avgFilterTriggerLossDiff":
        case "medianFilterTriggerLossDiff":
        case "worstFilterTriggerLossDiff":
            return "0.000E+00";
        case "avgOrdersGain":
        case "medianOrdersGain":
        case "worstOrdersGain":
        case "avgAmpAtMinN":
        case "medianAmpAtMinN":
        case "worstAmpAtMinN":
        case "avgStepSeriesAmp":
        case "medianStepSeriesAmp":
        case "worstStepSeriesAmp":
        case "avgFilterTriggerLossAmp":
        case "medianFilterTriggerLossAmp":
        case "worstFilterTriggerLossAmp":
        case "avgMinDeviationNComplexity":
        case "medianMinDeviationNComplexity":
        case "avgStepsToTolComplexity":
        case "medianStepsToTolComplexity":
            return "0.00";
        case "avgMinDeviationN":
        case "medianMinDeviationN":
        case "worstMinDeviationN":
        case "avgStepsToTol":
        case "medianStepsToTol":
        case "worstStepsToTol":
        case "avgEpsSavedSteps":
        case "medianEpsSavedSteps":
        case "worstEpsSavedSteps":
        case "avgDivZeroFirstN":
        case "medianDivZeroFirstN":
        case "worstDivZeroFirstN":
        case "avgFilterTriggerN":
        case "medianFilterTriggerN":
        case "worstFilterTriggerN":
        case "avgFilterTriggerDeltaFromMinN":
        case "medianFilterTriggerDeltaFromMinN":
        case "worstFilterTriggerDeltaFromMinN":
        case "seriesCount":
        case "rankPrecision":
        case "rankCost":
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

function clampEpsilonExp(value: number): number {
    return Math.min(EPSILON_EXP_MAX, Math.max(EPSILON_EXP_MIN, value));
}

function parseEpsilonExpInput(value: string): number | null {
    const trimmed = value.trim();
    if (!/^-?\d+$/.test(trimmed)) return null;

    const parsed = Number.parseInt(trimmed, 10);
    if (!Number.isFinite(parsed)) return null;
    return clampEpsilonExp(parsed);
}

function RankingProgressCard({
    current,
    total,
}: {
    current: number;
    total: number;
}) {
    const pct = total > 0 ? Math.max(0, Math.min(100, Math.round((current / total) * 100))) : 0;

    return (
        <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
            <div className="mb-2 flex items-center justify-between text-sm text-textDim">
                <span>Computing algorithm ranking...</span>
                <span>
                    {current} / {total} ({pct}%)
                </span>
            </div>
            <div className="h-2 w-full overflow-hidden rounded bg-surface/60">
                <div className="h-2 bg-primary transition-[width]" style={{ width: `${pct}%` }} />
            </div>
        </div>
    );
}

interface AlgoRankingTableBodyProps {
    experiment: Experiment;
    epsilon: number;
    epsilonExp: number;
    precisionFilter: string | null;
    sortKey: RankingSortKey;
    sortDir: SortDir;
    filteredAccels: Experiment["accelList"];
    filteredSeries: Experiment["seriesList"];
    state: MatrixAlgoSeriesFilterState;
    renderRowHeader: MatrixProps<RowMeta, ColMeta>["renderRowHeader"];
    renderColHeader: MatrixProps<RowMeta, ColMeta>["renderColHeader"];
    renderCell: MatrixProps<RowMeta, ColMeta>["renderCell"];
    buildWorkbook: (args: {
        rows: MatrixAxisItem<RowMeta>[];
        cols: MatrixAxisItem<ColMeta>[];
        pager: { startIndex: number; endIndex: number };
    }) => XLSX.WorkBook;
}

function AlgoRankingTableBody({
    experiment,
    epsilon,
    epsilonExp,
    precisionFilter,
    sortKey,
    sortDir,
    filteredAccels,
    filteredSeries,
    state,
    renderRowHeader,
    renderColHeader,
    renderCell,
    buildWorkbook,
}: AlgoRankingTableBodyProps) {
    const filteredSeriesIds = useMemo(
        () => new Set(filteredSeries.map((series) => series.id)),
        [filteredSeries]
    );
    const filteredAccelIds = useMemo(
        () => new Set(filteredAccels.map((accel) => accel.id)),
        [filteredAccels]
    );
    const { stats, progress } = useAlgoRankingStats({
        experiment,
        epsilon,
        precisionFilter,
        allowedSeriesIds: filteredSeriesIds,
        allowedAccelIds: filteredAccelIds,
    });

    if (!stats || progress.running) {
        return <RankingProgressCard current={progress.current} total={progress.total} />;
    }

    if (!stats.length) {
        return (
            <div className="rounded-xl2 border border-border bg-panel p-3 text-[11px] text-textDim/70">
                No rows for current filters.
            </div>
        );
    }

    const argKeys = getVisibleArgKeys(stats);
    const colsAxis: MatrixAxisItem<ColMeta>[] = buildColumns(argKeys).map((col) => ({
        id: col.id,
        meta: col,
    }));

    const sortedStats = [...stats].sort((a, b) =>
        compareValues(getSortValue(a, sortKey), getSortValue(b, sortKey), sortDir)
    );

    const rowsAxis: MatrixAxisItem<RowMeta>[] = sortedStats.map((stat, index) => ({
        id: stat.algoKey,
        meta: {
            ...stat,
            place: index + 1,
            precisionLabel: stat.precision ?? (precisionFilter ? precisionFilter : "-"),
        },
    }));

    const stateKey = buildFilterStateKey(state);

    return (
        <>
            <div className="mb-2 text-[10px] text-textDim/70">
                rows: {rowsAxis.length} · accels: {filteredAccels.length} /{" "}
                {(experiment.accelList ?? []).length} · series: {filteredSeries.length} /{" "}
                {(experiment.seriesList ?? []).length}
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

    const [epsilonExpDraft, setEpsilonExpDraft] = useState(String(epsilonExp));

    useEffect(() => {
        const clamped = clampEpsilonExp(epsilonExp);
        if (clamped === epsilonExp) return;
        setViewState((current) => ({ ...current, epsilonExp: clamped }));
    }, [epsilonExp, setViewState]);

    useEffect(() => {
        setEpsilonExpDraft(String(epsilonExp));
    }, [epsilonExp]);

    const setEpsilonExp = useCallback(
        (nextExp: number) => {
            const clamped = clampEpsilonExp(nextExp);
            setEpsilonExpDraft(String(clamped));
            setViewState((current) => ({ ...current, epsilonExp: clamped }));
        },
        [setViewState]
    );

    const commitEpsilonExpDraft = useCallback(() => {
        const nextExp = parseEpsilonExpInput(epsilonExpDraft);
        if (nextExp === null) {
            setEpsilonExpDraft(String(epsilonExp));
            return;
        }
        setEpsilonExp(nextExp);
    }, [epsilonExp, epsilonExpDraft, setEpsilonExp]);

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
                        min={EPSILON_EXP_MIN}
                        max={EPSILON_EXP_MAX}
                        step={1}
                        value={epsilonExp}
                        onChange={(event) =>
                            setEpsilonExp(Number.parseInt(event.target.value, 10))
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
                    <label htmlFor="algo-ranking-epsilon-exp">exp:</label>
                    <input
                        id="algo-ranking-epsilon-exp"
                        type="number"
                        min={EPSILON_EXP_MIN}
                        max={EPSILON_EXP_MAX}
                        step={1}
                        value={epsilonExpDraft}
                        onChange={(event) => setEpsilonExpDraft(event.target.value)}
                        onBlur={commitEpsilonExpDraft}
                        onKeyDown={(event) => {
                            if (event.key === "Enter") {
                                event.currentTarget.blur();
                            } else if (event.key === "Escape") {
                                setEpsilonExpDraft(String(epsilonExp));
                                event.currentTarget.blur();
                            }
                        }}
                        className="w-20 rounded border border-border bg-surface px-1 py-[1px] font-mono outline-none"
                    />
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
                    return (
                        <AlgoRankingTableBody
                            experiment={experiment}
                            epsilon={epsilon}
                            epsilonExp={epsilonExp}
                            precisionFilter={precisionFilter}
                            sortKey={sortKey}
                            sortDir={sortDir}
                            filteredAccels={filteredAccels}
                            filteredSeries={filteredSeries}
                            state={state}
                            renderRowHeader={renderRowHeader}
                            renderColHeader={renderColHeader}
                            renderCell={renderCell}
                            buildWorkbook={buildWorkbook}
                        />
                    );
                }}
            </MatrixAlgoSeriesFilter>
        </div>
    );
};
