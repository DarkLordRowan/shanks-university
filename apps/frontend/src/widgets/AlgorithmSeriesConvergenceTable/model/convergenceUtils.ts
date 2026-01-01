import {
    type Series,
    type Accel,
    type SeriesAccel,
    type Complex,
    type ConvergenceAnalysis,
    type SideType,
    type MonotonicityType,
} from "./types";

const EPS = 1e-10;

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

export function analyzeSeriesAccelConvergence(
    series: Series | undefined,
    _accel: Accel | undefined,
    sa: SeriesAccel,
    maxSignChangesForOneSided: number = 0,
    maxViolationsForMonotone: number = 0
): ConvergenceAnalysis {
    const limit = series?.limit ?? null;

    if (!limit || !hasFiniteNumber(limit.re ?? null)) {
        return {
            seriesId: sa.series_id,
            accelId: sa.accel_id,
            side: "unknown",
            monotonicity: "unknown",
            stepsAnalyzed: 0,
            signChangesCount: 0,
            violationsCount: 0,
            signChangeNs: [],
            violationsNs: [],
        };
    }

    const points = getPointsSortedByN(sa);

    let prevErr: number | null = null;
    let prevSign: -1 | 0 | 1 | null = null;

    let signChangesCount = 0;
    let firstSignChangeN: number | null = null;

    let strictDecreaseFound = false;
    let equalityFound = false;
    let strictIncreaseFound = false;

    let stepsAnalyzed = 0;
    let violationsCount = 0;

    const signChangeNs: number[] = [];
    const violationsNs: number[] = [];

    for (const p of points) {
        const value = p.value;
        const err = errorNorm(value, limit);
        const sgn = realDiffSign(value, limit);

        // анализ знака
        if (sgn !== null && sgn !== 0) {
            if (prevSign === null) {
                prevSign = sgn;
            } else if (prevSign !== sgn) {
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
                    // Нарушение: ошибка увеличилась
                    strictIncreaseFound = true;
                    violationsCount += 1;
                    violationsNs.push(p.n);
                } else if (err < prevErr - EPS) {
                    strictDecreaseFound = true;
                } else {
                    equalityFound = true;
                }
            }
            prevErr = err;
        }
    }

    let side: SideType;
    if (prevSign === null) {
        if (stepsAnalyzed === 0) {
            side = "unknown";
        } else {
            side = "one_sided";
        }
    } else {
        if (signChangesCount <= maxSignChangesForOneSided) {
            side = "one_sided";
        } else {
            side = "two_sided";
        }
    }

    let monotonicity: MonotonicityType;
    if (stepsAnalyzed === 0) {
        monotonicity = "unknown";
    } else if (strictDecreaseFound && !strictIncreaseFound && !equalityFound) {
        monotonicity = "strict_decreasing_error";
    } else if (!strictDecreaseFound && !strictIncreaseFound && equalityFound) {
        monotonicity = "constant_error";
    } else if (strictDecreaseFound && !strictIncreaseFound && equalityFound) {
        monotonicity = "non_increasing_error";
    } else if (!strictDecreaseFound && strictIncreaseFound && !equalityFound) {
        monotonicity = "strict_increasing_error";
    } else if (!strictDecreaseFound && strictIncreaseFound && equalityFound) {
        monotonicity = "non_decreasing_error";
    } else if (strictDecreaseFound && strictIncreaseFound) {
        if (violationsCount <= maxViolationsForMonotone) {
            monotonicity = "non_increasing_error";
        } else if (stepsAnalyzed - violationsCount <= maxViolationsForMonotone) {
            monotonicity = "non_decreasing_error";
        } else {
            monotonicity = "random_error";
        }
    } else {
        monotonicity = "random_error";
    }

    return {
        seriesId: sa.series_id,
        accelId: sa.accel_id,
        side: side,
        monotonicity: monotonicity,
        stepsAnalyzed: stepsAnalyzed,
        signChangesCount: signChangesCount,
        violationsCount: violationsCount,
        signChangeNs: signChangeNs,
        violationsNs: violationsNs,
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
        case "strict_increasing_error":
            return "↑";
        case "non_decreasing_error":
            return "⇗";
        case "random_error":
            return "↕";
        case "unknown":
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
        case "strict_increasing_error":
            return "|A_k - lim| строго возрастает на всех шагах.";
        case "non_decreasing_error":
            return "|A_k - lim| не убывает (есть растущие и равные шаги).";
        case "random_error":
            return "|A_k - lim| меняется случайным образом (нет явной монотонности).";
        case "unknown":
            return "Недостаточно точек для анализа монотонности.";
    }
}

export function getConvergenceCellDomId(accelId: string, seriesId: string): string {
    return `conv-cell-${accelId}::${seriesId}`;
}
