import type { Complex, SeriesAccel } from "@/entities/experiment/model/experiment";
import type { DetailPoint, MonotonicityType, SideType } from "./types";
import { errorNorm, getPointsSortedByN, realDiffSign } from "./convergenceUtils";

const EPS = 1e-15;

export interface ConvergenceDevStats {
    count: number;
    first: number | null;
    firstN: number | null;
    min: number | null;
    minN: number | null;
    mean: number | null;
    median: number | null;
    max: number | null;
    last: number | null;
    lastN: number | null;
    lastMinusMin: number | null;
    amplitudeOrders: number | null;
    maxAmplitudeOrders: number | null;
    plateauStartN: number | null;
}

export type ConvergenceClassKind =
    | "static"
    | "fast_one_sided"
    | "last_min_one_sided"
    | "fast_two_sided"
    | "last_min_two_sided"
    | "recover_one_sided"
    | "recover_two_sided"
    | "interior_one_sided"
    | "interior_two_sided"
    | "first_one_sided"
    | "first_two_sided"
    | "unknown";

export interface ConvergenceClassInfo {
    kind: ConvergenceClassKind;
    order: number;
    symbol: string;
    label: string;
    title: string;
    description: string;
    colorToken:
        | "violet"
        | "green"
        | "greenDark"
        | "yellow"
        | "yellowDark"
        | "orange"
        | "orangeDark"
        | "red"
        | "redDark"
        | "neutral";
}

interface ErrorPoint {
    n: number;
    err: number;
}

function isFiniteNumber(value: number | null | undefined): value is number {
    return value != null && Number.isFinite(value);
}

function almostEqual(a: number, b: number): boolean {
    if (a === b) return true;
    const scale = Math.max(1, Math.abs(a), Math.abs(b));
    return Math.abs(a - b) <= EPS * scale;
}

function safeLog10(value: number): number {
    if (value === 0) return 0;
    return Math.log10(value);
}

function computeAmplitudeOrders(left: number, right: number): number {
    return safeLog10(left) - safeLog10(right);
}

function formatScalar(value: number | null | undefined): string {
    if (!isFiniteNumber(value)) return "—";
    const absValue = Math.abs(value);
    if (absValue === 0) return "0";
    if (absValue >= 1e5 || absValue < 1e-4) return value.toExponential(4);
    return value.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
}

function buildLabel(symbol: string, side: SideType): string {
    if (symbol === "→") return symbol;
    if (side === "one_sided") return `1s ${symbol}`;
    if (side === "two_sided") return `2s ${symbol}`;
    return symbol;
}

function isMonotone(monotonicity: MonotonicityType): boolean {
    return (
        monotonicity === "strict_decreasing_error" ||
        monotonicity === "non_increasing_error" ||
        monotonicity === "constant_error"
    );
}

function buildClassInfo(kind: ConvergenceClassKind, side: SideType): ConvergenceClassInfo {
    switch (kind) {
        case "static":
            return {
                kind,
                order: 0,
                symbol: "→",
                label: "→",
                title: "Не двигается",
                description: "|A_n - lim| остается постоянным.",
                colorToken: "violet",
            };
        case "fast_one_sided":
            return {
                kind,
                order: 1,
                symbol: "↓→",
                label: buildLabel("↓→", side),
                title: "Быстро сходится",
                description:
                    "Минимум |A_n - lim| достигнут до последней точки и дальше ошибка не меняется.",
                colorToken: "green",
            };
        case "last_min_one_sided":
            return {
                kind,
                order: 2,
                symbol: "↓↓",
                label: buildLabel("↓↓", side),
                title: "Сходится",
                description: "Минимум |A_n - lim| достигнут на последней точке без роста ошибки по пути.",
                colorToken: "green",
            };
        case "fast_two_sided":
            return {
                kind,
                order: 3,
                symbol: "↓→",
                label: buildLabel("↓→", side),
                title: "Быстро сходится",
                description:
                    "Двусторонний вариант: минимум достигнут до последней точки и дальше ошибка не меняется.",
                colorToken: "greenDark",
            };
        case "last_min_two_sided":
            return {
                kind,
                order: 4,
                symbol: "↓↓",
                label: buildLabel("↓↓", side),
                title: "Сходится",
                description: "Двусторонний вариант: минимум достигнут на последней точке без роста ошибки по пути.",
                colorToken: "greenDark",
            };
        case "recover_one_sided":
            return {
                kind,
                order: 5,
                symbol: "↑↓",
                label: buildLabel("↑↓", side),
                title: "Расходится и сходится",
                description: "Минимум |A_n - lim| на последней точке, но до этого были и рост, и спад.",
                colorToken: "yellow",
            };
        case "recover_two_sided":
            return {
                kind,
                order: 6,
                symbol: "↑↓",
                label: buildLabel("↑↓", side),
                title: "Расходится и сходится",
                description: "Двусторонний вариант: минимум на последней точке, но траектория была немонотонной.",
                colorToken: "yellowDark",
            };
        case "interior_one_sided":
            return {
                kind,
                order: 7,
                symbol: "↓↑",
                label: buildLabel("↓↑", side),
                title: "Сходится и расходится",
                description:
                    "Минимум |A_n - lim| достигнут внутри последовательности, не в первой и не в последней точке.",
                colorToken: "orange",
            };
        case "interior_two_sided":
            return {
                kind,
                order: 8,
                symbol: "↓↑",
                label: buildLabel("↓↑", side),
                title: "Сходится и расходится",
                description:
                    "Двусторонний вариант: минимум достигнут внутри последовательности, потом ошибка снова растет.",
                colorToken: "orangeDark",
            };
        case "first_one_sided":
            return {
                kind,
                order: 9,
                symbol: "↑↑",
                label: buildLabel("↑↑", side),
                title: "Расходится",
                description: "Минимум |A_n - lim| уже был на первой точке, дальше стало только хуже.",
                colorToken: "red",
            };
        case "first_two_sided":
            return {
                kind,
                order: 10,
                symbol: "↑↑",
                label: buildLabel("↑↑", side),
                title: "Расходится",
                description:
                    "Двусторонний вариант: лучшая точка была первой, дальше ошибка только ухудшалась.",
                colorToken: "redDark",
            };
        case "unknown":
        default:
            return {
                kind: "unknown",
                order: 99,
                symbol: "?",
                label: "?",
                title: "Недостаточно данных",
                description: "Недостаточно данных для устойчивой классификации.",
                colorToken: "neutral",
            };
    }
}

function buildDevStats(finiteErrors: ErrorPoint[]): ConvergenceDevStats {
    if (finiteErrors.length === 0) {
        return {
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
        };
    }

    let min = finiteErrors[0].err;
    let minN = finiteErrors[0].n;
    let max = finiteErrors[0].err;
    let sum = 0;

    for (const point of finiteErrors) {
        if (point.err < min || (almostEqual(point.err, min) && point.n < minN)) {
            min = point.err;
            minN = point.n;
        }
        if (point.err > max) max = point.err;
        sum += point.err;
    }

    const sortedErrors = finiteErrors.map((point) => point.err).sort((a, b) => a - b);
    const mid = Math.floor(sortedErrors.length / 2);
    const median =
        sortedErrors.length % 2 === 1
            ? sortedErrors[mid]
            : 0.5 * (sortedErrors[mid - 1] + sortedErrors[mid]);

    const firstPoint = finiteErrors[0];
    const lastPoint = finiteErrors[finiteErrors.length - 1];

    let plateauStartN: number | null = null;
    for (let i = 0; i < finiteErrors.length; i++) {
        if (!almostEqual(finiteErrors[i].err, min)) continue;

        let plateau = true;
        for (let j = i; j < finiteErrors.length; j++) {
            if (!almostEqual(finiteErrors[j].err, min)) {
                plateau = false;
                break;
            }
        }

        if (plateau) {
            plateauStartN = finiteErrors[i].n;
            break;
        }
    }

    const amplitudeOrders = computeAmplitudeOrders(lastPoint.err, min);
    const maxAmplitudeOrders = computeAmplitudeOrders(max, min);

    return {
        count: finiteErrors.length,
        first: firstPoint.err,
        firstN: firstPoint.n,
        min,
        minN,
        mean: sum / finiteErrors.length,
        median,
        max,
        last: lastPoint.err,
        lastN: lastPoint.n,
        lastMinusMin: lastPoint.err - min,
        amplitudeOrders,
        maxAmplitudeOrders,
        plateauStartN,
    };
}

export function computeSeriesAlgoAmplitudeOrders(
    seriesMin: number | null,
    algoMin: number | null
): number | null {
    if (!isFiniteNumber(seriesMin) || !isFiniteNumber(algoMin)) return null;

    if (seriesMin === 0) {
        if (algoMin === 0) return 0;
        return -Math.abs(Math.log10(algoMin));
    }

    if (algoMin === 0) {
        return Math.abs(Math.log10(seriesMin));
    }

    return Math.log10(seriesMin) - Math.log10(algoMin);
}

export function buildConvergenceDetailPoints(
    sa: SeriesAccel | null | undefined,
    limit: Complex | null
): DetailPoint[] {
    if (!sa) return [];

    let prevValue: { re: number | null; im: number | null } | null = null;
    const points: DetailPoint[] = [];

    for (const point of getPointsSortedByN(sa)) {
        const valueRe = point.value?.re ?? null;
        const valueImRaw = point.value?.im ?? null;
        const valueIm = valueImRaw ?? 0;
        const err = errorNorm(point.value, limit);
        const sign = realDiffSign(point.value, limit);

        let diffRe: number | null = null;
        let diffIm: number | null = null;
        let diffNorm: number | null = null;

        if (prevValue && isFiniteNumber(valueRe) && isFiniteNumber(prevValue.re)) {
            const prevIm = prevValue.im ?? 0;
            const dRe = valueRe - prevValue.re;
            const dIm = valueIm - prevIm;
            const dn = Math.hypot(dRe, dIm);
            diffRe = dRe;
            diffIm = dIm;
            diffNorm = Number.isFinite(dn) ? dn : null;
        }

        prevValue = { re: valueRe, im: valueImRaw };

        points.push({
            n: point.n,
            valueRe,
            valueIm: valueImRaw,
            err,
            sign,
            diffRe,
            diffIm,
            diffNorm,
        });
    }

    return points;
}

export function computeConvergenceDevStats(points: DetailPoint[]): ConvergenceDevStats {
    const finiteErrors = points
        .filter((point): point is DetailPoint & { err: number } => isFiniteNumber(point.err))
        .map((point) => ({ n: point.n, err: point.err }));

    return buildDevStats(finiteErrors);
}

export function computeConvergenceDevStatsFromSeriesAccel(
    sa: SeriesAccel | null | undefined,
    limit: Complex | null
): ConvergenceDevStats {
    if (!sa) return buildDevStats([]);

    const finiteErrors: ErrorPoint[] = [];
    for (const point of getPointsSortedByN(sa)) {
        const err = errorNorm(point.value, limit);
        if (!isFiniteNumber(err)) continue;
        finiteErrors.push({ n: point.n, err });
    }

    return buildDevStats(finiteErrors);
}

export function getConvergenceClassInfo(
    side: SideType,
    monotonicity: MonotonicityType,
    dev: ConvergenceDevStats
): ConvergenceClassInfo {
    if (dev.count === 0 || side === "unknown" || monotonicity === "unknown") {
        return buildClassInfo("unknown", side);
    }

    if (
        monotonicity === "constant_error" ||
        (isFiniteNumber(dev.min) && isFiniteNumber(dev.max) && almostEqual(dev.min, dev.max))
    ) {
        return buildClassInfo("static", side);
    }

    const monotone = isMonotone(monotonicity);
    const minAtFirst =
        isFiniteNumber(dev.firstN) && isFiniteNumber(dev.minN) && dev.firstN === dev.minN;
    const minAtLast =
        isFiniteNumber(dev.lastN) && isFiniteNumber(dev.minN) && dev.lastN === dev.minN;
    const plateauBeforeLast =
        isFiniteNumber(dev.plateauStartN) &&
        isFiniteNumber(dev.lastN) &&
        dev.plateauStartN < dev.lastN;

    if (monotone && plateauBeforeLast) {
        return buildClassInfo(side === "one_sided" ? "fast_one_sided" : "fast_two_sided", side);
    }

    if (monotone && minAtLast) {
        return buildClassInfo(
            side === "one_sided" ? "last_min_one_sided" : "last_min_two_sided",
            side
        );
    }

    if (minAtLast) {
        return buildClassInfo(
            side === "one_sided" ? "recover_one_sided" : "recover_two_sided",
            side
        );
    }

    if (minAtFirst) {
        return buildClassInfo(
            side === "one_sided" ? "first_one_sided" : "first_two_sided",
            side
        );
    }

    if (isFiniteNumber(dev.minN)) {
        return buildClassInfo(
            side === "one_sided" ? "interior_one_sided" : "interior_two_sided",
            side
        );
    }

    return buildClassInfo("unknown", side);
}

export function buildConvergenceClassLegendTitle(
    maxSignChangesForOneSided: number,
    maxViolationsForMonotone: number
): string {
    return [
        "Класс: порядок идет от лучших классов к худшим.",
        `1s = односторонний алгоритм (с учетом max sign changes = ${maxSignChangesForOneSided}).`,
        `2s = двусторонний алгоритм (с учетом max sign changes = ${maxSignChangesForOneSided}).`,
        `Почти монотонность учитывает max violations = ${maxViolationsForMonotone}.`,
        "",
        "→  Не двигается: |A_n - lim| = const.",
        "↓→ Быстро сходится: минимум достигнут раньше последней точки и дальше ошибка не меняется.",
        "↓↓ Сходится: минимум достигнут на последней точке без роста ошибки.",
        "↑↓ Расходится и сходится: минимум на последней точке, но по пути были и рост, и спад.",
        "↓↑ Сходится и расходится: минимум внутри последовательности, не в первой и не в последней точке.",
        "↑↑ Расходится: минимум уже на первой точке, дальше стало только хуже.",
        "",
        "Порядок классов: violet, green, dark-green, yellow, dark-yellow, orange, dark-orange, red, dark-red.",
    ].join("\n");
}

export function formatComplexValue(value: Complex | null): string {
    if (!value) return "∅";

    const re = formatScalar(value.re);
    const im = formatScalar(value.im ?? 0);
    return `(${re}, ${im})`;
}

export function formatDeviationValue(value: number | null): string {
    return formatScalar(value);
}

export function formatAmplitudeOrders(value: number | null): string {
    if (!isFiniteNumber(value)) {
        return value === Number.POSITIVE_INFINITY ? "∞" : "—";
    }
    return value.toFixed(2).replace(/0+$/g, "").replace(/\.$/, "");
}
