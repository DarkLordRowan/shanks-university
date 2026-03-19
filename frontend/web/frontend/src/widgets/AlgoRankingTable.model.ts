import type {
    AccelArgs,
    Experiment,
    Series,
    SeriesAccel,
    SeriesAccelComputedPoint,
    SeriesComputedPoint,
} from "@/entities/experiment/model/experiment";
import { analyzeSeriesAccelConvergence } from "@/widgets/AlgorithmSeriesConvergenceTable/model/convergenceUtils";

export type AlgoKey = string;

export interface AlgoStats {
    algoKey: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    args: AccelArgs | null;
    arg1: string;
    arg2: string;
    arg3: string;

    precision: string | null;

    seriesCount: number;
    bestDeviations: number[];
    stepsToTol: number[];
    reachedTolCount: number;
    minDeviationNs: number[];
    relativeErrors: number[];
    ordersGains: number[];
    ampAtMinNGains: number[];
    lastMinusMinGaps: number[];
    oneSidedCount: number;

    bestMinCount: number;
    worstMinCount: number;
    bestLastCount: number;
    worstLastCount: number;

    avgBestDeviation: number;
    fracReachedTol: number;
    avgStepsToTol: number;
    avgMinDeviationN: number;
    avgRelativeError: number;
    avgOrdersGain: number;
    avgAmpAtMinN: number;
    avgLastMinusMin: number;
    oneSidedShare: number;
    bestMinShare: number;
    worstMinShare: number;
    bestLastShare: number;
    worstLastShare: number;

    rankPrecision: number;
    rankSpeed: number;
    rankStability: number;
    totalRankScore: number;
}

export type AlgoRankingSortKey =
    | "algorithmName"
    | "m"
    | "arg1"
    | "arg2"
    | "arg3"
    | "precision"
    | "seriesCount"
    | "avgBestDeviation"
    | "avgRelativeError"
    | "avgOrdersGain"
    | "avgAmpAtMinN"
    | "avgMinDeviationN"
    | "avgLastMinusMin"
    | "fracReachedTol"
    | "oneSidedShare"
    | "avgStepsToTol"
    | "bestMinShare"
    | "worstMinShare"
    | "bestLastShare"
    | "worstLastShare"
    | "rankPrecision"
    | "rankSpeed"
    | "rankStability"
    | "totalRankScore";

export const MAX_ARG_COLUMNS = 3;

interface DeviationMetrics {
    minDeviation: number;
    minDeviationN: number;
    stepsToTol: number;
    lastDeviation: number;
}

interface SeriesAlgoMetrics {
    algoKey: AlgoKey;
    minDeviation: number;
    lastDeviation: number;
}

const RANK_COMPARE_EPS = 1e-12;

function isFiniteNumber(x: number | null | undefined): x is number {
    return x != null && Number.isFinite(x);
}

function almostEqual(a: number, b: number): boolean {
    if (a === b) return true;
    if (!Number.isFinite(a) || !Number.isFinite(b)) return false;

    const scale = Math.max(1, Math.abs(a), Math.abs(b));
    return Math.abs(a - b) <= RANK_COMPARE_EPS * scale;
}

function buildArgSlots(args: AccelArgs | null | undefined): {
    summary: string;
    slots: [string, string, string];
} {
    if (!args) {
        return { summary: "", slots: ["", "", ""] };
    }

    const entries = Object.entries(args)
        .filter(([, value]) => value !== undefined && value !== null && value !== "")
        .sort(([a], [b]) => a.localeCompare(b));

    if (entries.length === 0) {
        return { summary: "", slots: ["", "", ""] };
    }

    const values = entries.slice(0, MAX_ARG_COLUMNS).map(([key, value]) => `${key}=${String(value)}`);

    return {
        summary: entries.map(([key, value]) => `${key}=${String(value)}`).join(", "),
        slots: [values[0] ?? "", values[1] ?? "", values[2] ?? ""],
    };
}

export function makeAlgoKey(
    algorithmName: string,
    m: number | null,
    args: AccelArgs | null | undefined
): AlgoKey {
    const base = `${algorithmName}|m=${m ?? "null"}`;
    if (!args) return base;

    const argEntries = Object.entries(args)
        .filter(([, v]) => v !== undefined && v !== null && v !== "")
        .sort(([a], [b]) => a.localeCompare(b));

    if (argEntries.length === 0) return base;

    const suffix = argEntries.map(([k, v]) => `${k}=${String(v)}`).join(";");
    return `${base}|${suffix}`;
}

function meanOrInfinity(values: number[]): number {
    if (values.length === 0) return Number.POSITIVE_INFINITY;

    let sum = 0;
    for (const value of values) sum += value;
    return sum / values.length;
}

function meanOrNegativeInfinity(values: number[]): number {
    if (values.length === 0) return Number.NEGATIVE_INFINITY;

    let sum = 0;
    for (const value of values) sum += value;
    return sum / values.length;
}

function meanOrValue(values: number[], fallback: number): number {
    if (values.length === 0) return fallback;

    let sum = 0;
    for (const value of values) sum += value;
    return sum / values.length;
}

function computeRelativeError(algoMinDeviation: number, seriesMinDeviation: number): number {
    if (!Number.isFinite(algoMinDeviation) || !Number.isFinite(seriesMinDeviation)) {
        return Number.POSITIVE_INFINITY;
    }

    if (seriesMinDeviation === 0) {
        return algoMinDeviation === 0 ? 1 : Number.POSITIVE_INFINITY;
    }

    return algoMinDeviation / seriesMinDeviation;
}

function computeOrdersGain(algoMinDeviation: number, seriesMinDeviation: number): number {
    if (!Number.isFinite(algoMinDeviation) || !Number.isFinite(seriesMinDeviation)) {
        return Number.NEGATIVE_INFINITY;
    }

    if (seriesMinDeviation === 0) {
        if (algoMinDeviation === 0) return 0;
        return -Math.abs(Math.log10(algoMinDeviation));
    }

    if (algoMinDeviation === 0) {
        return Math.abs(Math.log10(seriesMinDeviation));
    }

    return Math.log10(seriesMinDeviation) - Math.log10(algoMinDeviation);
}

function computeLastMinusMinGap(lastDeviation: number, minDeviation: number): number {
    if (!Number.isFinite(lastDeviation) || !Number.isFinite(minDeviation)) {
        return Number.POSITIVE_INFINITY;
    }

    if (almostEqual(lastDeviation, minDeviation)) {
        return 0;
    }

    return lastDeviation - minDeviation;
}

function collectDeviationMetrics(
    points: Array<SeriesComputedPoint | SeriesAccelComputedPoint>,
    epsilon: number
): DeviationMetrics {
    let minDeviation = Number.POSITIVE_INFINITY;
    let minDeviationN = Number.POSITIVE_INFINITY;
    let stepsToTol = Number.POSITIVE_INFINITY;
    let lastDeviation = Number.POSITIVE_INFINITY;
    let lastN = Number.NEGATIVE_INFINITY;

    for (const point of points) {
        const deviation = point.deviation;
        if (!isFiniteNumber(deviation)) continue;

        const absDeviation = Math.abs(deviation);

        if (
            absDeviation < minDeviation ||
            (almostEqual(absDeviation, minDeviation) && point.n < minDeviationN)
        ) {
            minDeviation = absDeviation;
            minDeviationN = point.n;
        }

        if (absDeviation <= epsilon && point.n < stepsToTol) {
            stepsToTol = point.n;
        }

        if (point.n > lastN) {
            lastN = point.n;
            lastDeviation = absDeviation;
        }
    }

    return {
        minDeviation,
        minDeviationN,
        stepsToTol,
        lastDeviation,
    };
}

function compareNumbers(a: number, b: number, direction: "asc" | "desc"): number {
    if (almostEqual(a, b)) return 0;

    if (!Number.isFinite(a) && !Number.isFinite(b)) return 0;
    if (!Number.isFinite(a)) return direction === "asc" ? 1 : -1;
    if (!Number.isFinite(b)) return direction === "asc" ? -1 : 1;

    if (direction === "asc") return a < b ? -1 : 1;
    return a > b ? -1 : 1;
}

function assignDenseRanks<T>(
    items: T[],
    getValue: (item: T) => number,
    direction: "asc" | "desc",
    applyRank: (item: T, rank: number) => void
) {
    const sorted = [...items].sort((a, b) => compareNumbers(getValue(a), getValue(b), direction));

    let rank = 0;
    let previousValue: number | null = null;

    sorted.forEach((item) => {
        const currentValue = getValue(item);

        if (previousValue === null || !almostEqual(currentValue, previousValue)) {
            rank += 1;
            previousValue = currentValue;
        }

        applyRank(item, rank);
    });
}

function buildRankMap<T extends { algoKey: string }>(
    items: T[],
    getValue: (item: T) => number,
    direction: "asc" | "desc"
): Map<string, number> {
    const ranks = new Map<string, number>();

    assignDenseRanks(items, getValue, direction, (item, rank) => {
        ranks.set(item.algoKey, rank);
    });

    return ranks;
}

function getSeriesMinDeviationMap(
    seriesList: Series[] | undefined,
    precisionFilter: string | null,
    allowedSeriesIds?: Set<string> | null
): Map<string, number> {
    const result = new Map<string, number>();

    for (const series of seriesList ?? []) {
        if (allowedSeriesIds && !allowedSeriesIds.has(series.id)) continue;
        if (precisionFilter && series.precision !== precisionFilter) continue;

        const { minDeviation } = collectDeviationMetrics(series.computed ?? [], Number.NEGATIVE_INFINITY);
        result.set(series.id, minDeviation);
    }

    return result;
}

function getSeriesDeviationByNMaps(
    seriesList: Series[] | undefined,
    precisionFilter: string | null,
    allowedSeriesIds?: Set<string> | null
): Map<string, Map<number, number>> {
    const result = new Map<string, Map<number, number>>();

    for (const series of seriesList ?? []) {
        if (allowedSeriesIds && !allowedSeriesIds.has(series.id)) continue;
        if (precisionFilter && series.precision !== precisionFilter) continue;

        const byN = new Map<number, number>();

        for (const point of series.computed ?? []) {
            const deviation = point.deviation;
            if (!isFiniteNumber(deviation)) continue;

            const absDeviation = Math.abs(deviation);
            const prev = byN.get(point.n);
            if (prev == null || absDeviation < prev) {
                byN.set(point.n, absDeviation);
            }
        }

        result.set(series.id, byN);
    }

    return result;
}

export function getVisibleArgColumnCount(stats: AlgoStats[]): number {
    let count = 0;

    for (const stat of stats) {
        if (stat.arg1) count = Math.max(count, 1);
        if (stat.arg2) count = Math.max(count, 2);
        if (stat.arg3) count = Math.max(count, 3);
    }

    return count;
}

export function buildAlgoStatsFromExperiment(
    experiment: Experiment | null,
    epsilon: number,
    precisionFilter: string | null,
    allowedSeriesIds?: Set<string> | null,
    allowedAccelIds?: Set<string> | null
): AlgoStats[] {
    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return [];
    }

    const seriesById = new Map((experiment.seriesList ?? []).map((series) => [series.id, series]));
    const accelById = new Map((experiment.accelList ?? []).map((accel) => [accel.id, accel]));
    const seriesMinDeviationById = getSeriesMinDeviationMap(
        experiment.seriesList,
        precisionFilter,
        allowedSeriesIds
    );
    const seriesDeviationByNById = getSeriesDeviationByNMaps(
        experiment.seriesList,
        precisionFilter,
        allowedSeriesIds
    );

    const byAlgo = new Map<AlgoKey, AlgoStats>();
    const seriesMetrics = new Map<string, SeriesAlgoMetrics[]>();

    for (const seriesAccel of experiment.seriesAccelList) {
        if (allowedSeriesIds && !allowedSeriesIds.has(seriesAccel.series_id)) continue;
        if (allowedAccelIds && !allowedAccelIds.has(seriesAccel.accel_id)) continue;

        const series = seriesById.get(seriesAccel.series_id);
        if (!series) continue;

        const seriesPrecision = series.precision ?? null;
        if (precisionFilter && seriesPrecision !== precisionFilter) continue;

        const accel = accelById.get(seriesAccel.accel_id);
        const algorithmName = accel?.name ?? seriesAccel.accel_id;
        const m = accel?.m ?? null;
        const args = accel?.args ?? null;
        const { summary: argsSummary, slots } = buildArgSlots(args);

        const algoKey = makeAlgoKey(algorithmName, m, args);

        let stats = byAlgo.get(algoKey);
        if (!stats) {
            stats = {
                algoKey,
                algorithmName,
                m,
                argsSummary,
                args,
                arg1: slots[0],
                arg2: slots[1],
                arg3: slots[2],

                precision: seriesPrecision,

                seriesCount: 0,
                bestDeviations: [],
                stepsToTol: [],
                reachedTolCount: 0,
                minDeviationNs: [],
                relativeErrors: [],
                ordersGains: [],
                ampAtMinNGains: [],
                lastMinusMinGaps: [],
                oneSidedCount: 0,

                bestMinCount: 0,
                worstMinCount: 0,
                bestLastCount: 0,
                worstLastCount: 0,

                avgBestDeviation: Number.POSITIVE_INFINITY,
                fracReachedTol: 0,
                avgStepsToTol: Number.POSITIVE_INFINITY,
                avgMinDeviationN: Number.POSITIVE_INFINITY,
                avgRelativeError: Number.POSITIVE_INFINITY,
                avgOrdersGain: Number.NEGATIVE_INFINITY,
                avgAmpAtMinN: Number.NEGATIVE_INFINITY,
                avgLastMinusMin: Number.POSITIVE_INFINITY,
                oneSidedShare: 0,
                bestMinShare: 0,
                worstMinShare: 0,
                bestLastShare: 0,
                worstLastShare: 0,

                rankPrecision: 0,
                rankSpeed: 0,
                rankStability: 0,
                totalRankScore: 0,
            };
            byAlgo.set(algoKey, stats);
        } else if (stats.precision !== seriesPrecision) {
            stats.precision = null;
        }

        const metrics = collectDeviationMetrics(seriesAccel.computed ?? [], epsilon);
        if (!Number.isFinite(metrics.minDeviation)) continue;
        const convergenceAnalysis = analyzeSeriesAccelConvergence(series, accel ?? null, seriesAccel);

        stats.seriesCount += 1;
        stats.bestDeviations.push(metrics.minDeviation);
        stats.minDeviationNs.push(metrics.minDeviationN);
        if (convergenceAnalysis.side === "one_sided") {
            stats.oneSidedCount += 1;
        }

        if (Number.isFinite(metrics.stepsToTol)) {
            stats.stepsToTol.push(metrics.stepsToTol);
            stats.reachedTolCount += 1;
        } else {
            stats.stepsToTol.push(Number.POSITIVE_INFINITY);
        }

        const seriesMinDeviation = seriesMinDeviationById.get(series.id) ?? Number.POSITIVE_INFINITY;
        if (Number.isFinite(seriesMinDeviation)) {
            if (seriesMinDeviation > 0) {
                stats.relativeErrors.push(
                    computeRelativeError(metrics.minDeviation, seriesMinDeviation)
                );
            }
            stats.ordersGains.push(computeOrdersGain(metrics.minDeviation, seriesMinDeviation));
        }
        const seriesDeviationAtAlgoMinN =
            seriesDeviationByNById.get(series.id)?.get(metrics.minDeviationN) ??
            Number.POSITIVE_INFINITY;
        if (Number.isFinite(seriesDeviationAtAlgoMinN)) {
            stats.ampAtMinNGains.push(
                computeOrdersGain(metrics.minDeviation, seriesDeviationAtAlgoMinN)
            );
        }
        stats.lastMinusMinGaps.push(
            computeLastMinusMinGap(metrics.lastDeviation, metrics.minDeviation)
        );

        const items = seriesMetrics.get(series.id) ?? [];
        items.push({
            algoKey,
            minDeviation: metrics.minDeviation,
            lastDeviation: metrics.lastDeviation,
        });
        seriesMetrics.set(series.id, items);
    }

    const statsList: AlgoStats[] = [];
    for (const stats of byAlgo.values()) {
        if (stats.seriesCount === 0) continue;

        const finiteSteps = stats.stepsToTol.filter((value) => Number.isFinite(value));

        stats.avgBestDeviation = meanOrInfinity(stats.bestDeviations);
        stats.avgMinDeviationN = meanOrInfinity(stats.minDeviationNs);
        stats.fracReachedTol =
            stats.seriesCount > 0 ? stats.reachedTolCount / stats.seriesCount : 0;
        stats.avgStepsToTol = meanOrInfinity(finiteSteps);
        stats.avgRelativeError = meanOrValue(stats.relativeErrors, 1);
        stats.avgOrdersGain = meanOrNegativeInfinity(stats.ordersGains);
        stats.avgAmpAtMinN = meanOrNegativeInfinity(stats.ampAtMinNGains);
        stats.avgLastMinusMin = meanOrInfinity(stats.lastMinusMinGaps);
        stats.oneSidedShare = stats.seriesCount > 0 ? stats.oneSidedCount / stats.seriesCount : 0;

        statsList.push(stats);
    }

    if (statsList.length === 0) return statsList;

    let comparableSeriesForMin = 0;
    let comparableSeriesForLast = 0;

    for (const entries of seriesMetrics.values()) {
        const finiteMinEntries = entries.filter((entry) => Number.isFinite(entry.minDeviation));
        if (finiteMinEntries.length > 0) {
            comparableSeriesForMin += 1;

            let bestMin = finiteMinEntries[0].minDeviation;
            let worstMin = finiteMinEntries[0].minDeviation;

            for (const entry of finiteMinEntries) {
                if (entry.minDeviation < bestMin) bestMin = entry.minDeviation;
                if (entry.minDeviation > worstMin) worstMin = entry.minDeviation;
            }

            for (const entry of finiteMinEntries) {
                const stats = byAlgo.get(entry.algoKey);
                if (!stats) continue;

                if (almostEqual(entry.minDeviation, bestMin)) stats.bestMinCount += 1;
                if (almostEqual(entry.minDeviation, worstMin)) stats.worstMinCount += 1;
            }
        }

        const finiteLastEntries = entries.filter((entry) => Number.isFinite(entry.lastDeviation));
        if (finiteLastEntries.length > 0) {
            comparableSeriesForLast += 1;

            let bestLast = finiteLastEntries[0].lastDeviation;
            let worstLast = finiteLastEntries[0].lastDeviation;

            for (const entry of finiteLastEntries) {
                if (entry.lastDeviation < bestLast) bestLast = entry.lastDeviation;
                if (entry.lastDeviation > worstLast) worstLast = entry.lastDeviation;
            }

            for (const entry of finiteLastEntries) {
                const stats = byAlgo.get(entry.algoKey);
                if (!stats) continue;

                if (almostEqual(entry.lastDeviation, bestLast)) stats.bestLastCount += 1;
                if (almostEqual(entry.lastDeviation, worstLast)) stats.worstLastCount += 1;
            }
        }
    }

    for (const stats of statsList) {
        stats.bestMinShare =
            comparableSeriesForMin > 0 ? stats.bestMinCount / comparableSeriesForMin : 0;
        stats.worstMinShare =
            comparableSeriesForMin > 0 ? stats.worstMinCount / comparableSeriesForMin : 0;
        stats.bestLastShare =
            comparableSeriesForLast > 0 ? stats.bestLastCount / comparableSeriesForLast : 0;
        stats.worstLastShare =
            comparableSeriesForLast > 0 ? stats.worstLastCount / comparableSeriesForLast : 0;
    }

    const avgBestDeviationRanks = buildRankMap(statsList, (stats) => stats.avgBestDeviation, "asc");
    const avgRelativeErrorRanks = buildRankMap(statsList, (stats) => stats.avgRelativeError, "asc");
    const avgOrdersGainRanks = buildRankMap(statsList, (stats) => stats.avgOrdersGain, "desc");
    const bestMinShareRanks = buildRankMap(statsList, (stats) => stats.bestMinShare, "desc");
    const worstMinShareRanks = buildRankMap(statsList, (stats) => stats.worstMinShare, "asc");
    const avgStepsToTolRanks = buildRankMap(statsList, (stats) => stats.avgStepsToTol, "asc");
    const avgMinDeviationNRanks = buildRankMap(statsList, (stats) => stats.avgMinDeviationN, "asc");
    const avgLastMinusMinRanks = buildRankMap(statsList, (stats) => stats.avgLastMinusMin, "asc");
    const reachedTolRanks = buildRankMap(statsList, (stats) => stats.fracReachedTol, "desc");
    const oneSidedShareRanks = buildRankMap(statsList, (stats) => stats.oneSidedShare, "desc");
    const bestLastShareRanks = buildRankMap(statsList, (stats) => stats.bestLastShare, "desc");
    const worstLastShareRanks = buildRankMap(statsList, (stats) => stats.worstLastShare, "asc");

    const precisionScores = new Map<string, number>();
    const speedScores = new Map<string, number>();
    const stabilityScores = new Map<string, number>();

    for (const stats of statsList) {
        precisionScores.set(
            stats.algoKey,
            (avgBestDeviationRanks.get(stats.algoKey) ?? 0) +
                (avgRelativeErrorRanks.get(stats.algoKey) ?? 0) +
                (avgOrdersGainRanks.get(stats.algoKey) ?? 0) +
                (bestMinShareRanks.get(stats.algoKey) ?? 0) +
                (worstMinShareRanks.get(stats.algoKey) ?? 0)
        );

        speedScores.set(
            stats.algoKey,
            (avgStepsToTolRanks.get(stats.algoKey) ?? 0) +
                (avgMinDeviationNRanks.get(stats.algoKey) ?? 0)
        );

        stabilityScores.set(
            stats.algoKey,
            (reachedTolRanks.get(stats.algoKey) ?? 0) +
                (oneSidedShareRanks.get(stats.algoKey) ?? 0) +
                (avgLastMinusMinRanks.get(stats.algoKey) ?? 0) +
                (bestLastShareRanks.get(stats.algoKey) ?? 0) +
                (worstLastShareRanks.get(stats.algoKey) ?? 0)
        );
    }

    assignDenseRanks(
        statsList,
        (stats) => precisionScores.get(stats.algoKey) ?? Number.POSITIVE_INFINITY,
        "asc",
        (stats, rank) => {
            stats.rankPrecision = rank;
        }
    );

    assignDenseRanks(
        statsList,
        (stats) => speedScores.get(stats.algoKey) ?? Number.POSITIVE_INFINITY,
        "asc",
        (stats, rank) => {
            stats.rankSpeed = rank;
        }
    );

    assignDenseRanks(
        statsList,
        (stats) => stabilityScores.get(stats.algoKey) ?? Number.POSITIVE_INFINITY,
        "asc",
        (stats, rank) => {
            stats.rankStability = rank;
        }
    );

    for (const stats of statsList) {
        stats.totalRankScore = stats.rankPrecision + stats.rankSpeed + stats.rankStability;
    }

    statsList.sort((a, b) => {
        const rankDiff = compareNumbers(a.totalRankScore, b.totalRankScore, "asc");
        if (rankDiff !== 0) return rankDiff;

        const devDiff = compareNumbers(a.avgBestDeviation, b.avgBestDeviation, "asc");
        if (devDiff !== 0) return devDiff;

        const gainDiff = compareNumbers(a.avgOrdersGain, b.avgOrdersGain, "desc");
        if (gainDiff !== 0) return gainDiff;

        const gainAtMinNDiff = compareNumbers(a.avgAmpAtMinN, b.avgAmpAtMinN, "desc");
        if (gainAtMinNDiff !== 0) return gainAtMinNDiff;

        return compareNumbers(a.avgRelativeError, b.avgRelativeError, "asc");
    });

    return statsList;
}
