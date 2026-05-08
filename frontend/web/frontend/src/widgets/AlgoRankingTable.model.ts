import type {
    Accel,
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
    baseAlgorithmName: string;
    variant: "raw" | "filtered";
    filteredMethodName: string | null;
    m: number | null;
    howMuchFormula: string;
    argsSummary: string;
    args: AccelArgs | null;
    arg1: string;
    arg2: string;
    arg3: string;

    precision: string | null;

    seriesCount: number;
    bestDeviations: number[];
    stepsToTol: number[];
    epsSavedSteps: number[];
    reachedTolCount: number;
    minDeviationNs: number[];
    relativeErrors: number[];
    ordersGains: number[];
    ampAtMinNGains: number[];
    lastMinusMinGaps: number[];
    comparableSeriesMinCount: number;
    notBetterThanSeriesCount: number;
    oneSidedCount: number;

    bestMinCount: number;
    worstMinCount: number;
    bestLastCount: number;
    worstLastCount: number;

    avgBestDeviation: number;
    medianBestDeviation: number;
    worstBestDeviation: number;
    fracReachedTol: number;
    avgStepsToTol: number;
    medianStepsToTol: number;
    worstStepsToTol: number;
    avgMinDeviationN: number;
    medianMinDeviationN: number;
    worstMinDeviationN: number;
    avgRelativeError: number;
    medianRelativeError: number;
    worstRelativeError: number;
    avgOrdersGain: number;
    medianOrdersGain: number;
    worstOrdersGain: number;
    avgAmpAtMinN: number;
    medianAmpAtMinN: number;
    worstAmpAtMinN: number;
    avgLastMinusMin: number;
    medianLastMinusMin: number;
    worstLastMinusMin: number;
    avgEpsSavedSteps: number;
    medianEpsSavedSteps: number;
    worstEpsSavedSteps: number;
    notBetterThanSeriesShare: number;
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
    | "howMuchFormula"
    | "arg1"
    | "arg2"
    | "arg3"
    | "precision"
    | "seriesCount"
    | "avgBestDeviation"
    | "medianBestDeviation"
    | "worstBestDeviation"
    | "avgRelativeError"
    | "medianRelativeError"
    | "worstRelativeError"
    | "avgOrdersGain"
    | "medianOrdersGain"
    | "worstOrdersGain"
    | "avgAmpAtMinN"
    | "medianAmpAtMinN"
    | "worstAmpAtMinN"
    | "notBetterThanSeriesShare"
    | "avgMinDeviationN"
    | "medianMinDeviationN"
    | "worstMinDeviationN"
    | "avgLastMinusMin"
    | "medianLastMinusMin"
    | "worstLastMinusMin"
    | "fracReachedTol"
    | "oneSidedShare"
    | "avgStepsToTol"
    | "medianStepsToTol"
    | "worstStepsToTol"
    | "avgEpsSavedSteps"
    | "medianEpsSavedSteps"
    | "worstEpsSavedSteps"
    | "bestMinShare"
    | "worstMinShare"
    | "bestLastShare"
    | "worstLastShare"
    | "rankPrecision"
    | "rankSpeed"
    | "rankStability"
    | "totalRankScore";

export const MAX_ARG_COLUMNS = 3;

const ARG_KEY_PRIORITY = [
    "filter_method",
    "filter_window_length",
    "filter_degree",
    "filter_polyorder",
    "filter_delta",
    "filter_derive",
];

const HIDDEN_DYNAMIC_ARG_KEYS = new Set(["filtered"]);

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

function isDisplayableArgValue(value: unknown): boolean {
    return value !== undefined && value !== null && value !== "";
}

function compareArgKeys(a: string, b: string): number {
    const aPriority = ARG_KEY_PRIORITY.indexOf(a);
    const bPriority = ARG_KEY_PRIORITY.indexOf(b);

    if (aPriority !== -1 || bPriority !== -1) {
        if (aPriority === -1) return 1;
        if (bPriority === -1) return -1;
        return aPriority - bPriority;
    }

    return a.localeCompare(b);
}

export function getVisibleArgKeys(stats: AlgoStats[]): string[] {
    const keys = new Set<string>();

    for (const stat of stats) {
        for (const [key, value] of Object.entries(stat.args ?? {})) {
            if (HIDDEN_DYNAMIC_ARG_KEYS.has(key)) continue;
            if (!isDisplayableArgValue(value)) continue;
            keys.add(key);
        }
    }

    return Array.from(keys).sort(compareArgKeys);
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

function medianOrValue(values: number[], fallback: number): number {
    if (values.length === 0) return fallback;

    const sorted = [...values].sort((a, b) => a - b);
    const mid = Math.floor(sorted.length / 2);
    return sorted.length % 2 === 1
        ? sorted[mid]
        : 0.5 * (sorted[mid - 1] + sorted[mid]);
}

function worstOrValue(
    values: number[],
    direction: "lower-is-better" | "higher-is-better",
    fallback: number
): number {
    if (values.length === 0) return fallback;
    return direction === "lower-is-better" ? Math.max(...values) : Math.min(...values);
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

function normalizeToken(value: unknown): string {
    return String(value ?? "")
        .toLowerCase()
        .replace(/[^a-z0-9]+/g, "");
}

function getArgValue(args: AccelArgs | null | undefined, aliases: string[]): unknown {
    if (!args) return null;

    const normalizedAliases = new Set(aliases.map(normalizeToken));
    for (const [key, value] of Object.entries(args)) {
        if (normalizedAliases.has(normalizeToken(key))) return value;
    }

    return null;
}

function getOrder(accel: Pick<Accel, "m"> | null | undefined): number {
    const order = accel?.m;
    if (!isFiniteNumber(order)) return 0;
    return Math.max(0, Math.trunc(order));
}

function getRemainderDeltaFromValue(value: unknown): number {
    const rawValue = String(value ?? "").trim().toLowerCase();
    const token = normalizeToken(value);

    if (
        ["v~", "ṽ", "v_tilde", "v-tilde"].includes(rawValue) ||
        rawValue.endsWith("v~") ||
        rawValue.endsWith("ṽ")
    ) return 2;
    if (
        ["vwavetype", "vwave", "tildev", "vtilde"].includes(token) ||
        token.endsWith("vwavetype") ||
        token.endsWith("vwave") ||
        token.endsWith("tildev") ||
        token.endsWith("vtilde")
    ) return 2;
    if (
        ["t~", "t̃", "t_tilde", "t-tilde"].includes(rawValue) ||
        rawValue.endsWith("t~") ||
        rawValue.endsWith("t̃")
    ) return 1;
    if (
        ["twave", "t", "vtype", "v"].includes(token) ||
        token.endsWith("twave") ||
        token.endsWith("vtype")
    ) return 1;
    if (rawValue.endsWith("t") || rawValue.endsWith("v")) return 1;
    return 0;
}

function getRemainderDelta(args: AccelArgs | null | undefined, sourceName?: string | null): number {
    const value = getArgValue(args, ["type", "remainder", "remainder_type"]);
    const argDelta = getRemainderDeltaFromValue(value);
    return argDelta > 0 ? argDelta : getRemainderDeltaFromValue(sourceName);
}

function isRhoType(args: AccelArgs | null | undefined): boolean {
    const value = getArgValue(args, ["type", "numerator", "numerator_type", "rho", "rho_type"]);
    if (value === true) return true;
    if (value === false || value == null) return false;
    if (typeof value === "number") return value !== 0;

    const rawValue = String(value).trim().toLowerCase();
    const numericValue = Number(rawValue);
    if (Number.isFinite(numericValue)) return numericValue !== 0;

    const token = normalizeToken(value);
    return token === "rhotype" || token === "rho" || token === "1" || token === "true";
}

function normalizeHowMuch(value: number, fallbackN: number): number {
    if (!Number.isFinite(value)) return fallbackN;
    return Math.max(0, Math.trunc(value));
}

export function computeHowMuch(accel: Pick<Accel, "name" | "m" | "args"> | null | undefined, n: number): number {
    const normalizedN = normalizeHowMuch(n, n);
    const order = getOrder(accel);
    const name = normalizeToken(accel?.name);
    const args = accel?.args ?? null;

    if (
        name.includes("anderson") ||
        name.includes("changwynn") ||
        name.includes("richardson")
    ) {
        return normalizedN + 1;
    }

    if (
        name.includes("fordsidi2") ||
        name.includes("fordsidialgorithm2") ||
        name.includes("fordsidisecond")
    ) {
        return normalizedN + 2;
    }

    if (
        name.includes("fordsidi3") ||
        name.includes("fordsidialgorithm3") ||
        name.includes("fordsidithird")
    ) {
        return normalizedN;
    }

    if (name.includes("brezinski")) {
        return normalizedN + Math.floor((3 * order) / 2) + 1;
    }

    if (
        name.includes("wynnrho") ||
        name.includes("whynnrho")
    ) {
        return normalizedN + order + 1 + (isRhoType(args) ? order : 0);
    }

    if (
        name.includes("wynnepsilon") ||
        (name.includes("shanks") && !name.includes("alternating")) ||
        name.includes("overholt")
    ) {
        return normalizedN + 2 * order + 1;
    }

    if (name.includes("lubkin")) {
        return normalizedN + 3 * order + 1;
    }

    if (
        name.includes("weniger") ||
        name.includes("alternatingshanks") ||
        name.includes("shanksalternating") ||
        name.includes("jtransformation")
    ) {
        return normalizedN + order + 1;
    }

    if (
        name === "pj" ||
        name.includes("pjalgorithm") ||
        name.includes("pjtransformation") ||
        name.includes("levin") ||
        name.includes("drummond") ||
        name === "falgorithm" ||
        name === "halgorithm" ||
        name.includes("falgorithm") ||
        name.includes("halgorithm")
    ) {
        return normalizedN + order + 1 + getRemainderDelta(args, accel?.name);
    }

    return normalizedN;
}

export function computeHowMuchFormula(
    accel: Pick<Accel, "name" | "m" | "args"> | null | undefined
): string {
    const name = normalizeToken(accel?.name);
    const args = accel?.args ?? null;

    if (
        name.includes("anderson") ||
        name.includes("changwynn") ||
        name.includes("richardson")
    ) {
        return "n + 1";
    }

    if (
        name.includes("fordsidi2") ||
        name.includes("fordsidialgorithm2") ||
        name.includes("fordsidisecond")
    ) {
        return "n + 2";
    }

    if (
        name.includes("fordsidi3") ||
        name.includes("fordsidialgorithm3") ||
        name.includes("fordsidithird")
    ) {
        return "n";
    }

    if (name.includes("brezinski")) {
        return "n + floor(3*order/2) + 1";
    }

    if (
        name.includes("wynnrho") ||
        name.includes("whynnrho")
    ) {
        return isRhoType(args)
            ? "n + 2*order + 1 (rho_type)"
            : "n + order + 1 (non-rho)";
    }

    if (
        name.includes("wynnepsilon") ||
        (name.includes("shanks") && !name.includes("alternating")) ||
        name.includes("overholt")
    ) {
        return "n + 2*order + 1";
    }

    if (name.includes("lubkin")) {
        return "n + 3*order + 1";
    }

    if (
        name.includes("weniger") ||
        name.includes("alternatingshanks") ||
        name.includes("shanksalternating") ||
        name.includes("jtransformation")
    ) {
        return "n + order + 1";
    }

    if (
        name === "pj" ||
        name.includes("pjalgorithm") ||
        name.includes("pjtransformation") ||
        name.includes("levin") ||
        name.includes("drummond") ||
        name === "falgorithm" ||
        name === "halgorithm" ||
        name.includes("falgorithm") ||
        name.includes("halgorithm")
    ) {
        const delta = getRemainderDelta(args, accel?.name);
        return delta > 0
            ? `n + order + 1 + delta, delta=${delta}`
            : "n + order + 1";
    }

    return "n";
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

export interface AlgoRankingStatsAccumulatorOptions {
    experiment: Experiment | null;
    epsilon: number;
    precisionFilter: string | null;
    allowedSeriesIds?: Set<string> | null;
    allowedAccelIds?: Set<string> | null;
}

export interface AlgoRankingStatsAccumulator {
    processSeriesAccel: (seriesAccel: SeriesAccel) => void;
    finalize: () => AlgoStats[];
}

function createEmptyAccumulator(): AlgoRankingStatsAccumulator {
    return {
        processSeriesAccel: () => {},
        finalize: () => [],
    };
}

function createInitialAlgoStats(params: {
    algoKey: AlgoKey;
    algorithmName: string;
    baseAlgorithmName: string;
    variant: "raw" | "filtered";
    filteredMethodName: string | null;
    m: number | null;
    argsSummary: string;
    args: AccelArgs | null;
    slots: [string, string, string];
    precision: string | null;
}): AlgoStats {
    return {
        algoKey: params.algoKey,
        algorithmName: params.algorithmName,
        baseAlgorithmName: params.baseAlgorithmName,
        variant: params.variant,
        filteredMethodName: params.filteredMethodName,
        m: params.m,
        howMuchFormula: computeHowMuchFormula({
            name: params.algorithmName,
            m: params.m,
            args: params.args,
        }),
        argsSummary: params.argsSummary,
        args: params.args,
        arg1: params.slots[0],
        arg2: params.slots[1],
        arg3: params.slots[2],

        precision: params.precision,

        seriesCount: 0,
        bestDeviations: [],
        stepsToTol: [],
        epsSavedSteps: [],
        reachedTolCount: 0,
        minDeviationNs: [],
        relativeErrors: [],
        ordersGains: [],
        ampAtMinNGains: [],
        lastMinusMinGaps: [],
        comparableSeriesMinCount: 0,
        notBetterThanSeriesCount: 0,
        oneSidedCount: 0,

        bestMinCount: 0,
        worstMinCount: 0,
        bestLastCount: 0,
        worstLastCount: 0,

        avgBestDeviation: Number.POSITIVE_INFINITY,
        medianBestDeviation: Number.POSITIVE_INFINITY,
        worstBestDeviation: Number.POSITIVE_INFINITY,
        fracReachedTol: 0,
        avgStepsToTol: Number.POSITIVE_INFINITY,
        medianStepsToTol: Number.POSITIVE_INFINITY,
        worstStepsToTol: Number.POSITIVE_INFINITY,
        avgMinDeviationN: Number.POSITIVE_INFINITY,
        medianMinDeviationN: Number.POSITIVE_INFINITY,
        worstMinDeviationN: Number.POSITIVE_INFINITY,
        avgRelativeError: Number.POSITIVE_INFINITY,
        medianRelativeError: Number.POSITIVE_INFINITY,
        worstRelativeError: Number.POSITIVE_INFINITY,
        avgOrdersGain: Number.NEGATIVE_INFINITY,
        medianOrdersGain: Number.NEGATIVE_INFINITY,
        worstOrdersGain: Number.NEGATIVE_INFINITY,
        avgAmpAtMinN: Number.NEGATIVE_INFINITY,
        medianAmpAtMinN: Number.NEGATIVE_INFINITY,
        worstAmpAtMinN: Number.NEGATIVE_INFINITY,
        avgLastMinusMin: Number.POSITIVE_INFINITY,
        medianLastMinusMin: Number.POSITIVE_INFINITY,
        worstLastMinusMin: Number.POSITIVE_INFINITY,
        avgEpsSavedSteps: Number.NEGATIVE_INFINITY,
        medianEpsSavedSteps: Number.NEGATIVE_INFINITY,
        worstEpsSavedSteps: Number.NEGATIVE_INFINITY,
        notBetterThanSeriesShare: 0,
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
}

export function createAlgoRankingStatsAccumulator(
    options: AlgoRankingStatsAccumulatorOptions
): AlgoRankingStatsAccumulator {
    const { experiment, epsilon, precisionFilter, allowedSeriesIds, allowedAccelIds } = options;

    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return createEmptyAccumulator();
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
    let finalizedStats: AlgoStats[] | null = null;

    return {
        processSeriesAccel: (seriesAccel: SeriesAccel) => {
            if (finalizedStats) return;
            if (allowedSeriesIds && !allowedSeriesIds.has(seriesAccel.series_id)) return;
            if (allowedAccelIds && !allowedAccelIds.has(seriesAccel.accel_id)) return;

            const series = seriesById.get(seriesAccel.series_id);
            if (!series) return;

            const seriesPrecision = series.precision ?? null;
            if (precisionFilter && seriesPrecision !== precisionFilter) return;

            const accel = accelById.get(seriesAccel.accel_id);
            const algorithmName = accel?.name ?? seriesAccel.accel_id;
            const baseAccel = accel?.baseAccelId ? accelById.get(accel.baseAccelId) : null;
            const baseAlgorithmName = baseAccel?.name ?? algorithmName;
            const variant = accel?.variant ?? "raw";
            const filteredMethodName = accel?.filteredMethodName ?? null;
            const m = accel?.m ?? null;
            const args = accel?.args ?? null;
            const { summary: argsSummary, slots } = buildArgSlots(args);

            const algoKey = makeAlgoKey(algorithmName, m, args);

            let stats = byAlgo.get(algoKey);
            if (!stats) {
                stats = createInitialAlgoStats({
                    algoKey,
                    algorithmName,
                    baseAlgorithmName,
                    variant,
                    filteredMethodName,
                    m,
                    argsSummary,
                    args,
                    slots,
                    precision: seriesPrecision,
                });
                byAlgo.set(algoKey, stats);
            } else if (stats.precision !== seriesPrecision) {
                stats.precision = null;
            }

            const metrics = collectDeviationMetrics(seriesAccel.computed ?? [], epsilon);
            if (!Number.isFinite(metrics.minDeviation)) return;
            const convergenceAnalysis = analyzeSeriesAccelConvergence(
                series,
                accel ?? null,
                seriesAccel
            );

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

            const seriesDeviationMetrics = collectDeviationMetrics(series.computed ?? [], epsilon);
            const effectiveStepsToTol = computeHowMuch(accel ?? null, metrics.stepsToTol);
            if (
                Number.isFinite(seriesDeviationMetrics.stepsToTol) &&
                Number.isFinite(effectiveStepsToTol)
            ) {
                stats.epsSavedSteps.push(
                    seriesDeviationMetrics.stepsToTol - effectiveStepsToTol
                );
            }

            const seriesMinDeviation =
                seriesMinDeviationById.get(series.id) ?? Number.POSITIVE_INFINITY;
            if (Number.isFinite(seriesMinDeviation)) {
                stats.comparableSeriesMinCount += 1;
                if (
                    metrics.minDeviation > seriesMinDeviation ||
                    almostEqual(metrics.minDeviation, seriesMinDeviation)
                ) {
                    stats.notBetterThanSeriesCount += 1;
                }
                if (seriesMinDeviation > 0) {
                    stats.relativeErrors.push(
                        computeRelativeError(metrics.minDeviation, seriesMinDeviation)
                    );
                }
                stats.ordersGains.push(computeOrdersGain(metrics.minDeviation, seriesMinDeviation));
            }
            const effectiveMinDeviationN = computeHowMuch(accel ?? null, metrics.minDeviationN);
            const seriesDeviationAtAlgoMinN =
                seriesDeviationByNById.get(series.id)?.get(effectiveMinDeviationN) ??
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
        },
        finalize: () => {
            if (!finalizedStats) {
                finalizedStats = finalizeAlgoStats(byAlgo, seriesMetrics);
            }
            return finalizedStats;
        },
    };
}

function finalizeAlgoStats(
    byAlgo: Map<AlgoKey, AlgoStats>,
    seriesMetrics: Map<string, SeriesAlgoMetrics[]>
): AlgoStats[] {
    const statsList: AlgoStats[] = [];
    for (const stats of byAlgo.values()) {
        if (stats.seriesCount === 0) continue;

        const finiteSteps = stats.stepsToTol.filter((value) => Number.isFinite(value));
        const finiteEpsSavedSteps = stats.epsSavedSteps.filter((value) =>
            Number.isFinite(value)
        );

        stats.avgBestDeviation = meanOrInfinity(stats.bestDeviations);
        stats.medianBestDeviation = medianOrValue(
            stats.bestDeviations,
            Number.POSITIVE_INFINITY
        );
        stats.worstBestDeviation = worstOrValue(
            stats.bestDeviations,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.avgMinDeviationN = meanOrInfinity(stats.minDeviationNs);
        stats.medianMinDeviationN = medianOrValue(
            stats.minDeviationNs,
            Number.POSITIVE_INFINITY
        );
        stats.worstMinDeviationN = worstOrValue(
            stats.minDeviationNs,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.fracReachedTol =
            stats.seriesCount > 0 ? stats.reachedTolCount / stats.seriesCount : 0;
        stats.avgStepsToTol = meanOrInfinity(finiteSteps);
        stats.medianStepsToTol = medianOrValue(finiteSteps, Number.POSITIVE_INFINITY);
        stats.worstStepsToTol = worstOrValue(
            finiteSteps,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.avgRelativeError = meanOrValue(stats.relativeErrors, 1);
        stats.medianRelativeError = medianOrValue(stats.relativeErrors, 1);
        stats.worstRelativeError = worstOrValue(
            stats.relativeErrors,
            "lower-is-better",
            1
        );
        stats.avgOrdersGain = meanOrNegativeInfinity(stats.ordersGains);
        stats.medianOrdersGain = medianOrValue(stats.ordersGains, Number.NEGATIVE_INFINITY);
        stats.worstOrdersGain = worstOrValue(
            stats.ordersGains,
            "higher-is-better",
            Number.NEGATIVE_INFINITY
        );
        stats.avgAmpAtMinN = meanOrNegativeInfinity(stats.ampAtMinNGains);
        stats.medianAmpAtMinN = medianOrValue(
            stats.ampAtMinNGains,
            Number.NEGATIVE_INFINITY
        );
        stats.worstAmpAtMinN = worstOrValue(
            stats.ampAtMinNGains,
            "higher-is-better",
            Number.NEGATIVE_INFINITY
        );
        stats.avgLastMinusMin = meanOrInfinity(stats.lastMinusMinGaps);
        stats.medianLastMinusMin = medianOrValue(
            stats.lastMinusMinGaps,
            Number.POSITIVE_INFINITY
        );
        stats.worstLastMinusMin = worstOrValue(
            stats.lastMinusMinGaps,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.avgEpsSavedSteps = meanOrNegativeInfinity(finiteEpsSavedSteps);
        stats.medianEpsSavedSteps = medianOrValue(
            finiteEpsSavedSteps,
            Number.NEGATIVE_INFINITY
        );
        stats.worstEpsSavedSteps = worstOrValue(
            finiteEpsSavedSteps,
            "higher-is-better",
            Number.NEGATIVE_INFINITY
        );
        stats.notBetterThanSeriesShare =
            stats.comparableSeriesMinCount > 0
                ? stats.notBetterThanSeriesCount / stats.comparableSeriesMinCount
                : 0;
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
    const notBetterThanSeriesRanks = buildRankMap(
        statsList,
        (stats) => stats.notBetterThanSeriesShare,
        "asc"
    );
    const bestMinShareRanks = buildRankMap(statsList, (stats) => stats.bestMinShare, "desc");
    const worstMinShareRanks = buildRankMap(statsList, (stats) => stats.worstMinShare, "asc");
    const avgStepsToTolRanks = buildRankMap(statsList, (stats) => stats.avgStepsToTol, "asc");
    const avgMinDeviationNRanks = buildRankMap(statsList, (stats) => stats.avgMinDeviationN, "asc");
    const avgEpsSavedStepsRanks = buildRankMap(
        statsList,
        (stats) => stats.avgEpsSavedSteps,
        "desc"
    );
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
                (notBetterThanSeriesRanks.get(stats.algoKey) ?? 0) +
                (bestMinShareRanks.get(stats.algoKey) ?? 0) +
                (worstMinShareRanks.get(stats.algoKey) ?? 0)
        );

        speedScores.set(
            stats.algoKey,
            (avgStepsToTolRanks.get(stats.algoKey) ?? 0) +
                (avgMinDeviationNRanks.get(stats.algoKey) ?? 0) +
                (avgEpsSavedStepsRanks.get(stats.algoKey) ?? 0)
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

export function buildAlgoStatsFromExperiment(
    experiment: Experiment | null,
    epsilon: number,
    precisionFilter: string | null,
    allowedSeriesIds?: Set<string> | null,
    allowedAccelIds?: Set<string> | null
): AlgoStats[] {
    const accumulator = createAlgoRankingStatsAccumulator({
        experiment,
        epsilon,
        precisionFilter,
        allowedSeriesIds,
        allowedAccelIds,
    });

    for (const seriesAccel of experiment?.seriesAccelList ?? []) {
        accumulator.processSeriesAccel(seriesAccel);
    }

    return accumulator.finalize();
}
