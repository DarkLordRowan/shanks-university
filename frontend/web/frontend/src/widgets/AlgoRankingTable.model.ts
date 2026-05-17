import type {
    Accel,
    AccelArgs,
    Complex,
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
    sourceAlgorithmName: string;
    algorithmName: string;
    baseAlgorithmName: string;
    levinEnding: "" | "U" | "T" | "T~" | "V" | "V~";
    variant: "raw" | "filtered";
    filteredMethodName: string | null;
    m: number | null;
    howMuchFormula: string;
    complexityFormula: string;
    argsSummary: string;
    args: AccelArgs | null;
    arg1: string;
    arg2: string;
    arg3: string;

    precision: string | null;

    runCount: number;
    seriesCount: number;
    bestDeviations: number[];
    stepsToTol: number[];
    epsSavedSteps: number[];
    reachedTolCount: number;
    minDeviationNs: number[];
    relativeErrors: number[];
    ordersGains: number[];
    ampAtMinNGains: number[];
    avgStepSeriesAmpGains: number[];
    medianStepSeriesAmpGains: number[];
    worstStepSeriesAmpGains: number[];
    filterTriggerNs: number[];
    filterTriggerDeltaFromMinNs: number[];
    filterTriggerLossAmps: number[];
    filterTriggerLossDiffs: number[];
    divZeroFirstNs: number[];
    lastMinusMinGaps: number[];
    divZeroRunCount: number;
    divZeroFiniteNRunCount: number;
    divZeroRecoveredCount: number;
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
    avgStepSeriesAmp: number;
    medianStepSeriesAmp: number;
    worstStepSeriesAmp: number;
    avgFilterTriggerN: number;
    medianFilterTriggerN: number;
    worstFilterTriggerN: number;
    avgFilterTriggerDeltaFromMinN: number;
    medianFilterTriggerDeltaFromMinN: number;
    worstFilterTriggerDeltaFromMinN: number;
    avgFilterTriggerLossAmp: number;
    medianFilterTriggerLossAmp: number;
    worstFilterTriggerLossAmp: number;
    avgFilterTriggerLossDiff: number;
    medianFilterTriggerLossDiff: number;
    worstFilterTriggerLossDiff: number;
    avgLastMinusMin: number;
    medianLastMinusMin: number;
    worstLastMinusMin: number;
    avgEpsSavedSteps: number;
    medianEpsSavedSteps: number;
    worstEpsSavedSteps: number;
    divZeroShare: number;
    avgDivZeroFirstN: number;
    medianDivZeroFirstN: number;
    worstDivZeroFirstN: number;
    divZeroRecoveredShare: number;
    avgMinDeviationNComplexity: number;
    medianMinDeviationNComplexity: number;
    avgStepsToTolComplexity: number;
    medianStepsToTolComplexity: number;
    notBetterThanSeriesShare: number;
    oneSidedShare: number;
    bestMinShare: number;
    worstMinShare: number;
    bestLastShare: number;
    worstLastShare: number;

    rankCost: number;
    rankPrecision: number;
    rankSpeed: number;
    rankStability: number;
    totalRankScore: number;
}

export type AlgoRankingSortKey =
    | "algorithmName"
    | "levinEnding"
    | "m"
    | "howMuchFormula"
    | "complexityFormula"
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
    | "avgStepSeriesAmp"
    | "medianStepSeriesAmp"
    | "worstStepSeriesAmp"
    | "avgFilterTriggerN"
    | "medianFilterTriggerN"
    | "worstFilterTriggerN"
    | "avgFilterTriggerDeltaFromMinN"
    | "medianFilterTriggerDeltaFromMinN"
    | "worstFilterTriggerDeltaFromMinN"
    | "avgFilterTriggerLossAmp"
    | "medianFilterTriggerLossAmp"
    | "worstFilterTriggerLossAmp"
    | "avgFilterTriggerLossDiff"
    | "medianFilterTriggerLossDiff"
    | "worstFilterTriggerLossDiff"
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
    | "divZeroShare"
    | "avgDivZeroFirstN"
    | "medianDivZeroFirstN"
    | "worstDivZeroFirstN"
    | "divZeroRecoveredShare"
    | "avgMinDeviationNComplexity"
    | "medianMinDeviationNComplexity"
    | "avgStepsToTolComplexity"
    | "medianStepsToTolComplexity"
    | "bestMinShare"
    | "worstMinShare"
    | "bestLastShare"
    | "worstLastShare"
    | "rankCost"
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

function normalizeImaginaryPart(value: number | null | undefined): number {
    return isFiniteNumber(value) ? value : 0;
}

function computeComplexDistance(
    a: Pick<Complex, "re" | "im"> | null | undefined,
    b: Pick<Complex, "re" | "im"> | null | undefined
): number {
    if (!a || !b) return Number.POSITIVE_INFINITY;
    if (!isFiniteNumber(a.re) || !isFiniteNumber(b.re)) return Number.POSITIVE_INFINITY;

    const re = a.re - b.re;
    const im = normalizeImaginaryPart(a.im) - normalizeImaginaryPart(b.im);
    const distance = Math.hypot(re, im);
    return Number.isFinite(distance) ? distance : Number.POSITIVE_INFINITY;
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

function computeFilterTriggerLossAmp(filterDeviation: number, minDeviation: number): number {
    if (!Number.isFinite(filterDeviation) || !Number.isFinite(minDeviation)) {
        return Number.POSITIVE_INFINITY;
    }

    if (almostEqual(filterDeviation, minDeviation)) return 0;
    if (minDeviation === 0) return filterDeviation === 0 ? 0 : Number.POSITIVE_INFINITY;
    if (filterDeviation === 0) return 0;

    const loss = Math.log10(filterDeviation) - Math.log10(minDeviation);
    return Number.isFinite(loss) ? Math.max(0, loss) : Number.POSITIVE_INFINITY;
}

function computeFilterTriggerLossDiff(filterDeviation: number, minDeviation: number): number {
    if (!Number.isFinite(filterDeviation) || !Number.isFinite(minDeviation)) {
        return Number.POSITIVE_INFINITY;
    }

    if (almostEqual(filterDeviation, minDeviation)) return 0;
    return Math.max(0, filterDeviation - minDeviation);
}

type LevinEnding = "" | "U" | "T" | "T~" | "V" | "V~";

function parseLevinEndingValue(value: unknown): LevinEnding {
    const rawValue = String(value ?? "").trim().toLowerCase();
    const token = normalizeToken(value);

    if (!rawValue && !token) return "";

    if (
        ["v~", "v_tilde", "v-tilde"].includes(rawValue) ||
        rawValue.endsWith("v~") ||
        ["vwavetype", "vwave", "tildev", "vtilde"].includes(token) ||
        token.endsWith("vwavetype") ||
        token.endsWith("vwave") ||
        token.endsWith("vtilde") ||
        token.endsWith("tildev")
    ) return "V~";

    if (
        ["t~", "t_tilde", "t-tilde"].includes(rawValue) ||
        rawValue.endsWith("t~") ||
        ["twavetype", "twave", "tildet", "ttilde"].includes(token) ||
        token.endsWith("twavetype") ||
        token.endsWith("twave") ||
        token.endsWith("ttilde") ||
        token.endsWith("tildet")
    ) return "T~";

    if (
        ["vtype", "v"].includes(token) ||
        token.endsWith("vtype") ||
        token.endsWith("algorithmv") ||
        rawValue.endsWith(" v")
    ) {
        return "V";
    }

    if (
        ["ttype", "t"].includes(token) ||
        token.endsWith("ttype") ||
        token.endsWith("algorithmt") ||
        rawValue.endsWith(" t")
    ) {
        return "T";
    }

    if (
        ["utype", "u"].includes(token) ||
        token.endsWith("utype") ||
        token.endsWith("algorithmu") ||
        rawValue.endsWith(" u")
    ) {
        return "U";
    }

    return "";
}

function isLevinLikeName(name: string): boolean {
    return normalizeToken(name).includes("levin");
}

function stripLevinEndingSuffix(name: string, ending: LevinEnding): string {
    if (!ending) return name;

    const stripped = name
        .replace(/(?:[\s_-]*(?:v[_\s-]*(?:wave|tilde)|v~|vwavetype|vwave|vtilde|vtype|v))$/i, "")
        .replace(/(?:[\s_-]*(?:t[_\s-]*(?:wave|tilde)|t~|twavetype|twave|ttilde|ttype|t))$/i, "")
        .replace(/(?:[\s_-]*(?:utype|u))$/i, "")
        .trim();

    return stripped || name;
}

function getLevinNameParts(
    sourceName: string,
    args: AccelArgs | null | undefined
): { displayName: string; ending: LevinEnding } {
    const isLevin = isLevinLikeName(sourceName);
    const argEnding = parseLevinEndingValue(getArgValue(args, ["type", "remainder", "remainder_type"]));
    const nameEnding = parseLevinEndingValue(sourceName);
    const ending = isLevin ? (argEnding || nameEnding) : "";

    return {
        displayName: ending ? stripLevinEndingSuffix(sourceName, ending) : sourceName,
        ending,
    };
}

interface DivZeroRunMetrics {
    hasDivZero: boolean;
    firstN: number | null;
    recovered: boolean;
}

function isDivZeroErrorMessage(message: string): boolean {
    const compact = message.toLowerCase().replace(/[^a-z0-9\u0430-\u044f]+/g, "");

    return (
        compact.includes("divisionbyzero") ||
        compact.includes("dividebyzero") ||
        compact.includes("divbyzero") ||
        compact.includes("zerodivision") ||
        compact.includes("\u0434\u0435\u043b\u0435\u043d\u0438\u0435\u043d\u0430\u043d\u043e\u043b\u044c") ||
        compact.includes("\u0434\u0435\u043b\u0435\u043d\u0438\u044f\u043d\u0430\u043d\u043e\u043b\u044c") ||
        compact.includes("\u0434\u0435\u043b\u0435\u043d\u0438\u0435\u043d\u04300") ||
        compact.includes("\u0434\u0435\u043b\u0435\u043d\u0438\u044f\u043d\u04300") ||
        compact.includes("\u0434\u0435\u043b\u0438\u0442\u044c\u043d\u0430\u043d\u043e\u043b\u044c")
    );
}

function isSuccessfulComputedPoint(point: SeriesAccelComputedPoint): boolean {
    if (isFiniteNumber(point.deviation)) return true;
    if (!point.value) return false;
    return isFiniteNumber(point.value.re) || isFiniteNumber(point.value.im);
}

function collectDivZeroRunMetrics(seriesAccel: SeriesAccel): DivZeroRunMetrics {
    const ns: number[] = [];
    let hasDivZero = false;

    for (const error of seriesAccel.errors ?? []) {
        const message = typeof error.message === "string" ? error.message : String(error.message ?? "");
        if (!isDivZeroErrorMessage(message)) continue;

        hasDivZero = true;
        if (isFiniteNumber(error.n)) ns.push(error.n);
    }

    if (ns.length === 0) {
        return { hasDivZero, firstN: null, recovered: false };
    }

    const firstN = Math.min(...ns);
    const lastN = Math.max(...ns);
    const recovered = (seriesAccel.computed ?? []).some(
        (point) => point.n > lastN && isSuccessfulComputedPoint(point)
    );

    return { hasDivZero, firstN, recovered };
}

function getComplexityFormula(
    accel: Pick<Accel, "name" | "m" | "args"> | null | undefined
): string {
    return `how_much(n) + ${getComplexityOFormula(accel)}`;
}

function getArgNumber(args: AccelArgs | null | undefined, aliases: string[]): number | null {
    const value = getArgValue(args, aliases);
    if (typeof value === "number") return Number.isFinite(value) ? value : null;
    if (typeof value === "string") {
        const parsed = Number(value.trim());
        return Number.isFinite(parsed) ? parsed : null;
    }
    return null;
}

function isAndersonVariableOrderName(name: string): boolean {
    return (
        name.includes("andersonmn") ||
        name.includes("andersonmton") ||
        name.includes("andersonmvar") ||
        name.includes("andersonvariable") ||
        name.includes("andersonnonconstant")
    );
}

function getFAuxiliaryTerm(
    accel: Pick<Accel, "args"> | null | undefined,
    n: number
): number {
    return (
        getArgNumber(accel?.args, [
            "T_aux",
            "t_aux",
            "tAux",
            "taux",
            "aux",
            "t_auxiliary",
            "auxiliary_terms",
            "auxiliaryTerms",
        ]) ?? n
    );
}

function getComplexityOFormula(
    accel: Pick<Accel, "name" | "m" | "args"> | null | undefined
): string {
    const name = normalizeToken(accel?.name);

    if (name.includes("pjalgorithm") || name.includes("pjtransformation") || name === "pj") {
        return getArgNumber(accel?.args, ["p"]) === 2 ? "O(k^2)" : "O(k^3)";
    }
    if (name.includes("anderson")) return isAndersonVariableOrderName(name) ? "O(n^3)" : "O(1)";
    if (name.includes("changwynn")) return "O(n^2)";
    if (name.includes("richardson")) return "O(n^2)";
    if (name.includes("fordsidi3") || name.includes("fordsidialgorithm3")) return "O(n^3)";
    if (name.includes("fordsidi2") || name.includes("fordsidialgorithm2")) return "O(n)";
    if (name.includes("brezinskitheta") || name.includes("brezinski")) return "O(k*n+k^2)";
    if (name.includes("wynnrho") || name.includes("whynnrho") || name.includes("wynnepsilon")) {
        return "O(k*n+k^2)";
    }
    if (name.includes("alternatingshanks") || name.includes("shanksalternating")) {
        return "O(k*n+k^2)";
    }
    if (name.includes("jtransformation") || name === "jalgorithm") return "O(k*n+k^2)";
    if (name === "f" || name === "falgorithm" || name.includes("falgorithm")) {
        return "O(k^2+k*T_aux)";
    }
    if (name === "h" || name === "halgorithm" || name.includes("halgorithm")) return "O(k^2)";
    if (name.includes("recdrummondd")) return "O(k^2)";
    if (name.includes("levinsidim")) return "O(k*n)";
    if (name.includes("levinsidis")) return "O(k^2)";
    if (name.includes("reclevin")) return "O(k^2)";
    if (name.includes("drummondd")) return "O(k)";
    if (name.includes("levin")) return "O(k)";
    if (name.includes("lubkinw") || name.includes("lubkin")) return "O(k*n+k^2)";
    if (name.includes("overholt")) return "O(k^2)";
    if (name.includes("shanks") && !name.includes("alternating")) return "O(k^3)";
    if (name.includes("weniger")) return "O(k)";

    return "O(n)";
}

function evaluateComplexityO(
    accel: Pick<Accel, "name" | "m" | "args"> | null | undefined,
    n: number
): number {
    if (!Number.isFinite(n)) return Number.POSITIVE_INFINITY;

    const m = getOrder(accel);
    const name = normalizeToken(accel?.name);

    if (name.includes("pjalgorithm") || name.includes("pjtransformation") || name === "pj") {
        return getArgNumber(accel?.args, ["p"]) === 2 ? m ** 2 : m ** 3;
    }
    if (name.includes("anderson")) return isAndersonVariableOrderName(name) ? n ** 3 : 1;
    if (name.includes("changwynn")) return n ** 2;
    if (name.includes("richardson")) return n ** 2;
    if (name.includes("fordsidi3") || name.includes("fordsidialgorithm3")) return n ** 3;
    if (name.includes("fordsidi2") || name.includes("fordsidialgorithm2")) return n;
    if (name.includes("brezinskitheta") || name.includes("brezinski")) return m * n + m ** 2;
    if (name.includes("wynnrho") || name.includes("whynnrho") || name.includes("wynnepsilon")) {
        return m * n + m ** 2;
    }
    if (name.includes("alternatingshanks") || name.includes("shanksalternating")) {
        return m * n + m ** 2;
    }
    if (name.includes("jtransformation") || name === "jalgorithm") return m * n + m ** 2;
    if (name === "f" || name === "falgorithm" || name.includes("falgorithm")) {
        return m ** 2 + m * getFAuxiliaryTerm(accel, n);
    }
    if (name === "h" || name === "halgorithm" || name.includes("halgorithm")) return m ** 2;
    if (name.includes("recdrummondd")) return m ** 2;
    if (name.includes("levinsidim")) return m * n;
    if (name.includes("levinsidis")) return m ** 2;
    if (name.includes("reclevin")) return m ** 2;
    if (name.includes("drummondd")) return m;
    if (name.includes("levin")) return m;
    if (name.includes("lubkinw") || name.includes("lubkin")) return m * n + m ** 2;
    if (name.includes("overholt")) return m ** 2;
    if (name.includes("shanks") && !name.includes("alternating")) return m ** 3;
    if (name.includes("weniger")) return m;

    return n;
}

function computeComplexityScore(
    accel: Pick<Accel, "name" | "m" | "args"> | null | undefined,
    n: number
): number {
    if (!Number.isFinite(n)) return Number.POSITIVE_INFINITY;

    const requiredTerms = computeHowMuch(accel, n);
    const operationScore = evaluateComplexityO(accel, n);
    return requiredTerms + operationScore;
}

const FILTER_TRIGGER_NEEDLE = "filters triggered due to";

function getFirstFilterTriggerN(seriesAccel: SeriesAccel): number | null {
    let firstN: number | null = null;

    for (const event of seriesAccel.events ?? []) {
        const text = `${event.name ?? ""} ${event.description ?? ""}`.toLowerCase();
        if (!text.includes(FILTER_TRIGGER_NEEDLE)) continue;
        if (!isFiniteNumber(event.n)) continue;
        if (firstN === null || event.n < firstN) firstN = event.n;
    }

    return firstN;
}

function getComputedDeviationAtN(
    points: SeriesAccelComputedPoint[] | undefined,
    n: number
): number | null {
    let result: number | null = null;

    for (const point of points ?? []) {
        if (point.n !== n) continue;
        const deviation = point.deviation;
        if (!isFiniteNumber(deviation)) continue;

        const absDeviation = Math.abs(deviation);
        if (result === null || absDeviation < result) result = absDeviation;
    }

    return result;
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

interface SeriesPointByNMetric {
    absDeviation: number;
    value: Complex | null;
}

function getSeriesPointByNMaps(
    seriesList: Series[] | undefined,
    precisionFilter: string | null,
    allowedSeriesIds?: Set<string> | null
): Map<string, Map<number, SeriesPointByNMetric>> {
    const result = new Map<string, Map<number, SeriesPointByNMetric>>();

    for (const series of seriesList ?? []) {
        if (allowedSeriesIds && !allowedSeriesIds.has(series.id)) continue;
        if (precisionFilter && series.precision !== precisionFilter) continue;

        const byN = new Map<number, SeriesPointByNMetric>();

        for (const point of series.computed ?? []) {
            const deviation = point.deviation;
            if (!isFiniteNumber(deviation)) continue;

            const absDeviation = Math.abs(deviation);
            const prev = byN.get(point.n);
            if (prev == null || absDeviation < prev.absDeviation) {
                byN.set(point.n, {
                    absDeviation,
                    value: point.value ?? null,
                });
            }
        }

        result.set(series.id, byN);
    }

    return result;
}

function collectStepSeriesAmpGains(
    computed: SeriesAccelComputedPoint[] | undefined,
    seriesPointByN: Map<number, SeriesPointByNMetric> | undefined,
    accel: Pick<Accel, "name" | "m" | "args"> | null | undefined
): number[] {
    if (!seriesPointByN || seriesPointByN.size === 0) return [];

    const gains: number[] = [];
    for (const point of computed ?? []) {
        const effectiveN = computeHowMuch(accel, point.n);
        const seriesPoint = seriesPointByN.get(effectiveN);
        if (!seriesPoint || !Number.isFinite(seriesPoint.absDeviation)) continue;

        const algoMinusSeries = computeComplexDistance(point.value, seriesPoint.value);
        if (!Number.isFinite(algoMinusSeries)) continue;

        const gain = computeOrdersGain(algoMinusSeries, seriesPoint.absDeviation);
        if (Number.isFinite(gain)) gains.push(gain);
    }

    return gains;
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
    sourceAlgorithmName: string;
    algorithmName: string;
    baseAlgorithmName: string;
    levinEnding: LevinEnding;
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
        sourceAlgorithmName: params.sourceAlgorithmName,
        algorithmName: params.algorithmName,
        baseAlgorithmName: params.baseAlgorithmName,
        levinEnding: params.levinEnding,
        variant: params.variant,
        filteredMethodName: params.filteredMethodName,
        m: params.m,
        howMuchFormula: computeHowMuchFormula({
            name: params.sourceAlgorithmName,
            m: params.m,
            args: params.args,
        }),
        complexityFormula: getComplexityFormula({
            name: params.sourceAlgorithmName,
            m: params.m,
            args: params.args,
        }),
        argsSummary: params.argsSummary,
        args: params.args,
        arg1: params.slots[0],
        arg2: params.slots[1],
        arg3: params.slots[2],

        precision: params.precision,

        runCount: 0,
        seriesCount: 0,
        bestDeviations: [],
        stepsToTol: [],
        epsSavedSteps: [],
        reachedTolCount: 0,
        minDeviationNs: [],
        relativeErrors: [],
        ordersGains: [],
        ampAtMinNGains: [],
        avgStepSeriesAmpGains: [],
        medianStepSeriesAmpGains: [],
        worstStepSeriesAmpGains: [],
        filterTriggerNs: [],
        filterTriggerDeltaFromMinNs: [],
        filterTriggerLossAmps: [],
        filterTriggerLossDiffs: [],
        divZeroFirstNs: [],
        lastMinusMinGaps: [],
        divZeroRunCount: 0,
        divZeroFiniteNRunCount: 0,
        divZeroRecoveredCount: 0,
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
        avgStepSeriesAmp: Number.NEGATIVE_INFINITY,
        medianStepSeriesAmp: Number.NEGATIVE_INFINITY,
        worstStepSeriesAmp: Number.NEGATIVE_INFINITY,
        avgFilterTriggerN: Number.POSITIVE_INFINITY,
        medianFilterTriggerN: Number.POSITIVE_INFINITY,
        worstFilterTriggerN: Number.POSITIVE_INFINITY,
        avgFilterTriggerDeltaFromMinN: Number.POSITIVE_INFINITY,
        medianFilterTriggerDeltaFromMinN: Number.POSITIVE_INFINITY,
        worstFilterTriggerDeltaFromMinN: Number.POSITIVE_INFINITY,
        avgFilterTriggerLossAmp: Number.POSITIVE_INFINITY,
        medianFilterTriggerLossAmp: Number.POSITIVE_INFINITY,
        worstFilterTriggerLossAmp: Number.POSITIVE_INFINITY,
        avgFilterTriggerLossDiff: Number.POSITIVE_INFINITY,
        medianFilterTriggerLossDiff: Number.POSITIVE_INFINITY,
        worstFilterTriggerLossDiff: Number.POSITIVE_INFINITY,
        avgLastMinusMin: Number.POSITIVE_INFINITY,
        medianLastMinusMin: Number.POSITIVE_INFINITY,
        worstLastMinusMin: Number.POSITIVE_INFINITY,
        avgEpsSavedSteps: Number.NEGATIVE_INFINITY,
        medianEpsSavedSteps: Number.NEGATIVE_INFINITY,
        worstEpsSavedSteps: Number.NEGATIVE_INFINITY,
        divZeroShare: 0,
        avgDivZeroFirstN: Number.POSITIVE_INFINITY,
        medianDivZeroFirstN: Number.POSITIVE_INFINITY,
        worstDivZeroFirstN: Number.POSITIVE_INFINITY,
        divZeroRecoveredShare: 0,
        avgMinDeviationNComplexity: Number.POSITIVE_INFINITY,
        medianMinDeviationNComplexity: Number.POSITIVE_INFINITY,
        avgStepsToTolComplexity: Number.POSITIVE_INFINITY,
        medianStepsToTolComplexity: Number.POSITIVE_INFINITY,
        notBetterThanSeriesShare: 0,
        oneSidedShare: 0,
        bestMinShare: 0,
        worstMinShare: 0,
        bestLastShare: 0,
        worstLastShare: 0,

        rankCost: 0,
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
    const seriesPointByNById = getSeriesPointByNMaps(
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
            const sourceAlgorithmName = accel?.name ?? seriesAccel.accel_id;
            const baseAccel = accel?.baseAccelId ? accelById.get(accel.baseAccelId) : null;
            const sourceBaseAlgorithmName = baseAccel?.name ?? sourceAlgorithmName;
            const variant = accel?.variant ?? "raw";
            const filteredMethodName = accel?.filteredMethodName ?? null;
            const m = accel?.m ?? null;
            const args = accel?.args ?? null;
            const { summary: argsSummary, slots } = buildArgSlots(args);
            const { displayName: algorithmName, ending: levinEnding } = getLevinNameParts(
                sourceAlgorithmName,
                args
            );
            const { displayName: baseAlgorithmName } = getLevinNameParts(
                sourceBaseAlgorithmName,
                baseAccel?.args ?? args
            );

            const algoKey = makeAlgoKey(sourceAlgorithmName, m, args);

            let stats = byAlgo.get(algoKey);
            if (!stats) {
                stats = createInitialAlgoStats({
                    algoKey,
                    sourceAlgorithmName,
                    algorithmName,
                    baseAlgorithmName,
                    levinEnding,
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

            stats.runCount += 1;
            const divZeroMetrics = collectDivZeroRunMetrics(seriesAccel);
            if (divZeroMetrics.hasDivZero) {
                stats.divZeroRunCount += 1;
                if (divZeroMetrics.firstN !== null) {
                    stats.divZeroFiniteNRunCount += 1;
                    stats.divZeroFirstNs.push(divZeroMetrics.firstN);
                    if (divZeroMetrics.recovered) {
                        stats.divZeroRecoveredCount += 1;
                    }
                }
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
            const filterTriggerN = getFirstFilterTriggerN(seriesAccel);
            if (isFiniteNumber(filterTriggerN)) {
                stats.filterTriggerNs.push(filterTriggerN);
                stats.filterTriggerDeltaFromMinNs.push(filterTriggerN - metrics.minDeviationN);
                const filterTriggerDeviation = getComputedDeviationAtN(
                    seriesAccel.computed,
                    filterTriggerN
                );
                if (filterTriggerDeviation !== null) {
                    stats.filterTriggerLossAmps.push(
                        computeFilterTriggerLossAmp(filterTriggerDeviation, metrics.minDeviation)
                    );
                    stats.filterTriggerLossDiffs.push(
                        computeFilterTriggerLossDiff(filterTriggerDeviation, metrics.minDeviation)
                    );
                }
            }
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
                seriesPointByNById.get(series.id)?.get(effectiveMinDeviationN)?.absDeviation ??
                Number.POSITIVE_INFINITY;
            if (Number.isFinite(seriesDeviationAtAlgoMinN)) {
                stats.ampAtMinNGains.push(
                    computeOrdersGain(metrics.minDeviation, seriesDeviationAtAlgoMinN)
                );
            }
            const stepSeriesAmpGains = collectStepSeriesAmpGains(
                seriesAccel.computed,
                seriesPointByNById.get(series.id),
                accel ?? null
            );
            if (stepSeriesAmpGains.length > 0) {
                stats.avgStepSeriesAmpGains.push(meanOrNegativeInfinity(stepSeriesAmpGains));
                stats.medianStepSeriesAmpGains.push(
                    medianOrValue(stepSeriesAmpGains, Number.NEGATIVE_INFINITY)
                );
                stats.worstStepSeriesAmpGains.push(
                    worstOrValue(
                        stepSeriesAmpGains,
                        "higher-is-better",
                        Number.NEGATIVE_INFINITY
                    )
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
        if (stats.seriesCount === 0 && stats.divZeroRunCount === 0) continue;

        const finiteSteps = stats.stepsToTol.filter((value) => Number.isFinite(value));
        const finiteEpsSavedSteps = stats.epsSavedSteps.filter((value) =>
            Number.isFinite(value)
        );
        const accelForStats = {
            name: stats.sourceAlgorithmName,
            m: stats.m,
            args: stats.args,
        };

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
        stats.avgStepSeriesAmp = meanOrNegativeInfinity(stats.avgStepSeriesAmpGains);
        stats.medianStepSeriesAmp = medianOrValue(
            stats.medianStepSeriesAmpGains,
            Number.NEGATIVE_INFINITY
        );
        stats.worstStepSeriesAmp = worstOrValue(
            stats.worstStepSeriesAmpGains,
            "higher-is-better",
            Number.NEGATIVE_INFINITY
        );
        stats.avgFilterTriggerN = meanOrInfinity(stats.filterTriggerNs);
        stats.medianFilterTriggerN = medianOrValue(
            stats.filterTriggerNs,
            Number.POSITIVE_INFINITY
        );
        stats.worstFilterTriggerN = worstOrValue(
            stats.filterTriggerNs,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.avgFilterTriggerDeltaFromMinN = meanOrInfinity(
            stats.filterTriggerDeltaFromMinNs
        );
        stats.medianFilterTriggerDeltaFromMinN = medianOrValue(
            stats.filterTriggerDeltaFromMinNs,
            Number.POSITIVE_INFINITY
        );
        stats.worstFilterTriggerDeltaFromMinN = worstOrValue(
            stats.filterTriggerDeltaFromMinNs,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.avgFilterTriggerLossAmp = meanOrInfinity(stats.filterTriggerLossAmps);
        stats.medianFilterTriggerLossAmp = medianOrValue(
            stats.filterTriggerLossAmps,
            Number.POSITIVE_INFINITY
        );
        stats.worstFilterTriggerLossAmp = worstOrValue(
            stats.filterTriggerLossAmps,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.avgFilterTriggerLossDiff = meanOrInfinity(stats.filterTriggerLossDiffs);
        stats.medianFilterTriggerLossDiff = medianOrValue(
            stats.filterTriggerLossDiffs,
            Number.POSITIVE_INFINITY
        );
        stats.worstFilterTriggerLossDiff = worstOrValue(
            stats.filterTriggerLossDiffs,
            "lower-is-better",
            Number.POSITIVE_INFINITY
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
        stats.divZeroShare =
            stats.runCount > 0 ? stats.divZeroRunCount / stats.runCount : 0;
        stats.avgDivZeroFirstN = meanOrInfinity(stats.divZeroFirstNs);
        stats.medianDivZeroFirstN = medianOrValue(
            stats.divZeroFirstNs,
            Number.POSITIVE_INFINITY
        );
        stats.worstDivZeroFirstN = worstOrValue(
            stats.divZeroFirstNs,
            "lower-is-better",
            Number.POSITIVE_INFINITY
        );
        stats.divZeroRecoveredShare =
            stats.divZeroFiniteNRunCount > 0
                ? stats.divZeroRecoveredCount / stats.divZeroFiniteNRunCount
                : 0;
        stats.avgMinDeviationNComplexity = computeComplexityScore(
            accelForStats,
            stats.avgMinDeviationN
        );
        stats.medianMinDeviationNComplexity = computeComplexityScore(
            accelForStats,
            stats.medianMinDeviationN
        );
        stats.avgStepsToTolComplexity = computeComplexityScore(
            accelForStats,
            stats.avgStepsToTol
        );
        stats.medianStepsToTolComplexity = computeComplexityScore(
            accelForStats,
            stats.medianStepsToTol
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

    const medianBestDeviationRanks = buildRankMap(
        statsList,
        (stats) => stats.medianBestDeviation,
        "asc"
    );
    const worstBestDeviationRanks = buildRankMap(
        statsList,
        (stats) => stats.worstBestDeviation,
        "asc"
    );
    const medianRelativeErrorRanks = buildRankMap(
        statsList,
        (stats) => stats.medianRelativeError,
        "asc"
    );
    const worstRelativeErrorRanks = buildRankMap(
        statsList,
        (stats) => stats.worstRelativeError,
        "asc"
    );
    const medianOrdersGainRanks = buildRankMap(
        statsList,
        (stats) => stats.medianOrdersGain,
        "desc"
    );
    const worstOrdersGainRanks = buildRankMap(
        statsList,
        (stats) => stats.worstOrdersGain,
        "desc"
    );
    const notBetterThanSeriesRanks = buildRankMap(
        statsList,
        (stats) => stats.notBetterThanSeriesShare,
        "asc"
    );
    const bestMinShareRanks = buildRankMap(statsList, (stats) => stats.bestMinShare, "desc");
    const worstMinShareRanks = buildRankMap(statsList, (stats) => stats.worstMinShare, "asc");
    const medianStepsToTolRanks = buildRankMap(
        statsList,
        (stats) => stats.medianStepsToTol,
        "asc"
    );
    const worstStepsToTolRanks = buildRankMap(
        statsList,
        (stats) => stats.worstStepsToTol,
        "asc"
    );
    const medianMinDeviationNRanks = buildRankMap(
        statsList,
        (stats) => stats.medianMinDeviationN,
        "asc"
    );
    const worstMinDeviationNRanks = buildRankMap(
        statsList,
        (stats) => stats.worstMinDeviationN,
        "asc"
    );
    const medianEpsSavedStepsRanks = buildRankMap(
        statsList,
        (stats) => stats.medianEpsSavedSteps,
        "desc"
    );
    const worstEpsSavedStepsRanks = buildRankMap(
        statsList,
        (stats) => stats.worstEpsSavedSteps,
        "desc"
    );
    const medianAmpAtMinNRanks = buildRankMap(
        statsList,
        (stats) => stats.medianAmpAtMinN,
        "desc"
    );
    const worstAmpAtMinNRanks = buildRankMap(
        statsList,
        (stats) => stats.worstAmpAtMinN,
        "desc"
    );
    const medianStepSeriesAmpRanks = buildRankMap(
        statsList,
        (stats) => stats.medianStepSeriesAmp,
        "desc"
    );
    const worstStepSeriesAmpRanks = buildRankMap(
        statsList,
        (stats) => stats.worstStepSeriesAmp,
        "desc"
    );
    const medianFilterTriggerNRanks = buildRankMap(
        statsList,
        (stats) => stats.medianFilterTriggerN,
        "asc"
    );
    const worstFilterTriggerNRanks = buildRankMap(
        statsList,
        (stats) => stats.worstFilterTriggerN,
        "asc"
    );
    const medianFilterTriggerDeltaFromMinNRanks = buildRankMap(
        statsList,
        (stats) => stats.medianFilterTriggerDeltaFromMinN,
        "asc"
    );
    const worstFilterTriggerDeltaFromMinNRanks = buildRankMap(
        statsList,
        (stats) => stats.worstFilterTriggerDeltaFromMinN,
        "asc"
    );
    const medianFilterTriggerLossAmpRanks = buildRankMap(
        statsList,
        (stats) => stats.medianFilterTriggerLossAmp,
        "asc"
    );
    const worstFilterTriggerLossAmpRanks = buildRankMap(
        statsList,
        (stats) => stats.worstFilterTriggerLossAmp,
        "asc"
    );
    const medianLastMinusMinRanks = buildRankMap(
        statsList,
        (stats) => stats.medianLastMinusMin,
        "asc"
    );
    const worstLastMinusMinRanks = buildRankMap(
        statsList,
        (stats) => stats.worstLastMinusMin,
        "asc"
    );
    const reachedTolRanks = buildRankMap(statsList, (stats) => stats.fracReachedTol, "desc");
    const oneSidedShareRanks = buildRankMap(statsList, (stats) => stats.oneSidedShare, "desc");
    const bestLastShareRanks = buildRankMap(statsList, (stats) => stats.bestLastShare, "desc");
    const worstLastShareRanks = buildRankMap(statsList, (stats) => stats.worstLastShare, "asc");
    const avgMinDeviationNComplexityRanks = buildRankMap(
        statsList,
        (stats) => stats.avgMinDeviationNComplexity,
        "asc"
    );
    const medianMinDeviationNComplexityRanks = buildRankMap(
        statsList,
        (stats) => stats.medianMinDeviationNComplexity,
        "asc"
    );
    const avgStepsToTolComplexityRanks = buildRankMap(
        statsList,
        (stats) => stats.avgStepsToTolComplexity,
        "asc"
    );
    const medianStepsToTolComplexityRanks = buildRankMap(
        statsList,
        (stats) => stats.medianStepsToTolComplexity,
        "asc"
    );

    const precisionScores = new Map<string, number>();
    const speedScores = new Map<string, number>();
    const stabilityScores = new Map<string, number>();
    const costScores = new Map<string, number>();

    for (const stats of statsList) {
        precisionScores.set(
            stats.algoKey,
            (medianBestDeviationRanks.get(stats.algoKey) ?? 0) +
                (worstBestDeviationRanks.get(stats.algoKey) ?? 0) +
                (medianRelativeErrorRanks.get(stats.algoKey) ?? 0) +
                (worstRelativeErrorRanks.get(stats.algoKey) ?? 0) +
                (medianOrdersGainRanks.get(stats.algoKey) ?? 0) +
                (worstOrdersGainRanks.get(stats.algoKey) ?? 0) +
                (medianStepSeriesAmpRanks.get(stats.algoKey) ?? 0) +
                (worstStepSeriesAmpRanks.get(stats.algoKey) ?? 0) +
                (notBetterThanSeriesRanks.get(stats.algoKey) ?? 0) +
                (bestMinShareRanks.get(stats.algoKey) ?? 0) +
                (worstMinShareRanks.get(stats.algoKey) ?? 0)
        );

        speedScores.set(
            stats.algoKey,
            (medianStepsToTolRanks.get(stats.algoKey) ?? 0) +
                (worstStepsToTolRanks.get(stats.algoKey) ?? 0) +
                (medianMinDeviationNRanks.get(stats.algoKey) ?? 0) +
                (worstMinDeviationNRanks.get(stats.algoKey) ?? 0) +
                (medianEpsSavedStepsRanks.get(stats.algoKey) ?? 0) +
                (worstEpsSavedStepsRanks.get(stats.algoKey) ?? 0) +
                (medianAmpAtMinNRanks.get(stats.algoKey) ?? 0) +
                (worstAmpAtMinNRanks.get(stats.algoKey) ?? 0) +
                (medianFilterTriggerNRanks.get(stats.algoKey) ?? 0) +
                (worstFilterTriggerNRanks.get(stats.algoKey) ?? 0) +
                (medianFilterTriggerDeltaFromMinNRanks.get(stats.algoKey) ?? 0) +
                (worstFilterTriggerDeltaFromMinNRanks.get(stats.algoKey) ?? 0) +
                (medianFilterTriggerLossAmpRanks.get(stats.algoKey) ?? 0) +
                (worstFilterTriggerLossAmpRanks.get(stats.algoKey) ?? 0)
        );

        stabilityScores.set(
            stats.algoKey,
            (reachedTolRanks.get(stats.algoKey) ?? 0) +
                (oneSidedShareRanks.get(stats.algoKey) ?? 0) +
                (medianLastMinusMinRanks.get(stats.algoKey) ?? 0) +
                (worstLastMinusMinRanks.get(stats.algoKey) ?? 0) +
                (bestLastShareRanks.get(stats.algoKey) ?? 0) +
                (worstLastShareRanks.get(stats.algoKey) ?? 0)
        );

        costScores.set(
            stats.algoKey,
            (avgMinDeviationNComplexityRanks.get(stats.algoKey) ?? 0) +
                (medianMinDeviationNComplexityRanks.get(stats.algoKey) ?? 0) +
                (avgStepsToTolComplexityRanks.get(stats.algoKey) ?? 0) +
                (medianStepsToTolComplexityRanks.get(stats.algoKey) ?? 0)
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

    assignDenseRanks(
        statsList,
        (stats) => costScores.get(stats.algoKey) ?? Number.POSITIVE_INFINITY,
        "asc",
        (stats, rank) => {
            stats.rankCost = rank;
        }
    );

    for (const stats of statsList) {
        stats.totalRankScore =
            stats.rankCost + stats.rankPrecision + stats.rankSpeed + stats.rankStability;
    }

    statsList.sort((a, b) => {
        const rankDiff = compareNumbers(a.totalRankScore, b.totalRankScore, "asc");
        if (rankDiff !== 0) return rankDiff;

        const devDiff = compareNumbers(a.medianBestDeviation, b.medianBestDeviation, "asc");
        if (devDiff !== 0) return devDiff;

        const worstDevDiff = compareNumbers(a.worstBestDeviation, b.worstBestDeviation, "asc");
        if (worstDevDiff !== 0) return worstDevDiff;

        const gainDiff = compareNumbers(a.medianOrdersGain, b.medianOrdersGain, "desc");
        if (gainDiff !== 0) return gainDiff;

        const worstGainDiff = compareNumbers(a.worstOrdersGain, b.worstOrdersGain, "desc");
        if (worstGainDiff !== 0) return worstGainDiff;

        const gainAtMinNDiff = compareNumbers(a.medianAmpAtMinN, b.medianAmpAtMinN, "desc");
        if (gainAtMinNDiff !== 0) return gainAtMinNDiff;

        const worstGainAtMinNDiff = compareNumbers(a.worstAmpAtMinN, b.worstAmpAtMinN, "desc");
        if (worstGainAtMinNDiff !== 0) return worstGainAtMinNDiff;

        const stepSeriesAmpDiff = compareNumbers(
            a.medianStepSeriesAmp,
            b.medianStepSeriesAmp,
            "desc"
        );
        if (stepSeriesAmpDiff !== 0) return stepSeriesAmpDiff;

        const worstStepSeriesAmpDiff = compareNumbers(
            a.worstStepSeriesAmp,
            b.worstStepSeriesAmp,
            "desc"
        );
        if (worstStepSeriesAmpDiff !== 0) return worstStepSeriesAmpDiff;

        const filterLossDiff = compareNumbers(
            a.medianFilterTriggerLossAmp,
            b.medianFilterTriggerLossAmp,
            "asc"
        );
        if (filterLossDiff !== 0) return filterLossDiff;

        const worstFilterLossDiff = compareNumbers(
            a.worstFilterTriggerLossAmp,
            b.worstFilterTriggerLossAmp,
            "asc"
        );
        if (worstFilterLossDiff !== 0) return worstFilterLossDiff;

        const relativeDiff = compareNumbers(a.medianRelativeError, b.medianRelativeError, "asc");
        if (relativeDiff !== 0) return relativeDiff;

        return compareNumbers(a.worstRelativeError, b.worstRelativeError, "asc");
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
