import {
    type Series,
    type Accel,
    type SeriesAccel,
    type Complex,
    type SeriesArgs,
    type AccelArgs,
    type ConvergenceAnalysis,
    type SideType,
    type MonotonicityType,
} from "./types";

const EPS = 1e-15;

type Scalar = number | string | boolean | null;

export function hasFiniteNumber(v: number | null | undefined): v is number {
    return v != null && Number.isFinite(v);
}

export function errorNorm(
    value: { re: number | null; im: number | null } | null,
    limit: Complex | null
): number | null {
    if (!value || !limit) return null;

    const reV = value.re;
    const imV = value.im ?? 0;
    const reL = limit.re;
    const imL = limit.im ?? 0;

    if (!hasFiniteNumber(reV) || !hasFiniteNumber(reL)) return null;
    if (!hasFiniteNumber(imV) || !hasFiniteNumber(imL)) return null;

    const dr = reV - reL;
    const di = imV - imL;
    const e = Math.hypot(dr, di);

    return Number.isFinite(e) ? e : null;
}

export function realDiffSign(
    value: { re: number | null; im: number | null } | null,
    limit: Complex | null
): -1 | 0 | 1 | null {
    if (!value || !limit) return null;
    const reV = value.re;
    const reL = limit.re;

    if (!hasFiniteNumber(reV) || !hasFiniteNumber(reL)) return null;

    const d = reV - reL;

    if (d > EPS) return 1;
    if (d < -EPS) return -1;
    return 0;
}

export function getPointsSortedByN(
    sa: SeriesAccel
): { n: number; value: { re: number | null; im: number | null } | null }[] {
    const raw = sa.computed ?? [];
    const len = raw.length;

    if (len <= 1) {
        return raw;
    }

    let sorted = true;
    for (let i = 1; i < len; i++) {
        if (raw[i - 1].n > raw[i].n) {
            sorted = false;
            break;
        }
    }

    if (sorted) return raw;
    return [...raw].sort((a, b) => a.n - b.n);
}

function toSortableNumber(v: Scalar): number | null {
    if (typeof v === "number") {
        return Number.isFinite(v) ? v : null;
    }
    if (typeof v === "string") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    return null;
}

export function parseX(args: SeriesArgs | null): { xLabel: string; xSort: number | null } {
    const raw = args?.x as Scalar | undefined;
    if (raw == null) {
        return { xLabel: "∅", xSort: null };
    }
    return {
        xLabel: String(raw),
        xSort: toSortableNumber(raw),
    };
}

export function nonNullEntries<T extends Record<string, unknown>>(obj: T | null | undefined) {
    if (!obj) return [] as [string, unknown][];
    return Object.entries(obj).filter(([, v]) => v !== null && v !== undefined);
}

export function buildArgsSummary(args: AccelArgs | null): string {
    const entries = nonNullEntries(args);
    if (entries.length === 0) return "";
    entries.sort(([a], [b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

export function analyzeSeriesAccelConvergence(
    series: Series | undefined,
    _accel: Accel | undefined,
    sa: SeriesAccel
): ConvergenceAnalysis {
    const limit = series?.limit ?? null;

    if (!limit || !hasFiniteNumber(limit.re ?? null)) {
        return {
            seriesId: sa.series_id,
            accelId: sa.accel_id,
            side: "no_limit",
            monotonicity: "no_limit",
            signChangesCount: 0,
            firstSignChangeN: null,
            firstGrowthN: null,
            growthViolationsCount: 0,
            stepsAnalyzed: 0,
            signChangeNs: [],
            growthNs: [],
        };
    }

    const points = getPointsSortedByN(sa);

    let prevErr: number | null = null;
    let prevSign: -1 | 0 | 1 | null = null;

    let signChangesCount = 0;
    let firstSignChangeN: number | null = null;

    let hasGrowth = false;
    let firstGrowthN: number | null = null;

    let hasStrictDecrease = false;
    let hasEqual = false;

    let stepsAnalyzed = 0;
    let growthViolationsCount = 0;

    const signChangeNs: number[] = [];
    const growthNs: number[] = [];

    for (const p of points) {
        const value = p.value;
        const err = errorNorm(value, limit);
        const sgn = realDiffSign(value, limit);

        // анализ знака
        if (sgn !== null && sgn !== 0) {
            if (prevSign === null) {
                prevSign = sgn;
            } else if (prevSign !== 0 && prevSign !== sgn) {
                signChangesCount += 1;
                signChangeNs.push(p.n);
                if (firstSignChangeN === null) {
                    firstSignChangeN = p.n;
                }
                prevSign = sgn;
            }
        }

        // анализ монотонности ошибки
        if (err !== null) {
            if (prevErr !== null) {
                stepsAnalyzed += 1;

                if (err > prevErr + EPS) {
                    growthViolationsCount += 1;
                    growthNs.push(p.n);

                    if (!hasGrowth) {
                        hasGrowth = true;
                        firstGrowthN = p.n;
                    }
                } else if (err < prevErr - EPS) {
                    hasStrictDecrease = true;
                } else {
                    hasEqual = true;
                }
            }
            prevErr = err;
        }
    }

    let side: SideType;
    if (prevSign === null) {
        side = "unknown";
    } else if (signChangesCount === 0) {
        side = "one_sided";
    } else {
        side = "two_sided";
    }

    let monotonicity: MonotonicityType;
    if (stepsAnalyzed === 0) {
        monotonicity = "not_enough_data";
    } else if (hasGrowth) {
        monotonicity = "has_growth";
    } else if (hasStrictDecrease && !hasEqual) {
        monotonicity = "strict_decreasing_error";
    } else if (!hasStrictDecrease && hasEqual) {
        monotonicity = "constant_error";
    } else if (hasStrictDecrease && hasEqual) {
        monotonicity = "non_increasing_error";
    } else {
        monotonicity = "not_enough_data";
    }

    return {
        seriesId: sa.series_id,
        accelId: sa.accel_id,
        side,
        monotonicity,
        signChangesCount,
        firstSignChangeN,
        firstGrowthN,
        growthViolationsCount,
        stepsAnalyzed,
        signChangeNs,
        growthNs,
    };
}

export function formatSideShort(side: SideType): string {
    switch (side) {
        case "one_sided":
            return "1ст";
        case "two_sided":
            return "2ст";
        case "unknown":
            return "?";
        case "no_limit":
            return "∅";
    }
}

export function formatMonotonicityShort(mon: MonotonicityType): string {
    switch (mon) {
        case "strict_decreasing_error":
            return "↓";
        case "non_increasing_error":
            return "⇘";
        case "constant_error":
            return "=";
        case "has_growth":
            return "↗";
        case "not_enough_data":
        case "no_limit":
            return "?";
    }
}

export function formatSideDescription(side: SideType): string {
    switch (side) {
        case "one_sided":
            return "Одностороннее приближение к пределу (знак A_k - lim не меняется).";
        case "two_sided":
            return "Двустороннее приближение к пределу (знак A_k - lim меняется).";
        case "unknown":
            return "Недостаточно данных для определения односторонности.";
        case "no_limit":
            return "Нет значения предела для ряда (series.limit = null).";
    }
}

export function formatMonotonicityDescription(mon: MonotonicityType): string {
    switch (mon) {
        case "strict_decreasing_error":
            return "|A_k - lim| строго убывает на всех шагах (монотонно сходится).";
        case "non_increasing_error":
            return "|A_k - lim| не возрастает (есть убывающие и равные шаги).";
        case "constant_error":
            return "|A_k - lim| одинаково на всех шагах.";
        case "has_growth":
            return "На некоторых шагах |A_k - lim| возрастает (есть рост ошибки).";
        case "not_enough_data":
            return "Недостаточно точек для анализа монотонности.";
        case "no_limit":
            return "Нет значения предела, анализ монотонности не выполняется.";
    }
}
