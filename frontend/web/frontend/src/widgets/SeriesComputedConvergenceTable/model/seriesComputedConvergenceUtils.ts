import type { Complex, Series, SeriesArgs } from "@/entities/experiment/model/experiment";
import type {
    DetailPoint,
    MonotonicityType,
    SideType,
    SeriesComputedConvergenceAnalysis,
} from "./types";

const EPS = 1e-15;

export function hasFiniteNumber(v: number | null | undefined): v is number {
    return v != null && Number.isFinite(v);
}

export function normalizeIm(x: number | null | undefined): number {
    return hasFiniteNumber(x) ? x : 0;
}

export function errorNorm(value: Complex | null, limit: Complex | null): number | null {
    if (!value || !limit) return null;

    const vr = value.re;
    const lr = limit.re;

    if (!hasFiniteNumber(vr) || !hasFiniteNumber(lr)) return null;

    const vi = normalizeIm(value.im);
    const li = normalizeIm(limit.im);

    const dr = vr - lr;
    const di = vi - li;

    const n = Math.hypot(dr, di);
    return Number.isFinite(n) ? n : null;
}

export function realDiffSign(value: Complex | null, limit: Complex | null): -1 | 0 | 1 | null {
    if (!value || !limit) return null;

    const vr = value.re;
    const lr = limit.re;

    if (!hasFiniteNumber(vr) || !hasFiniteNumber(lr)) return null;

    const d = vr - lr;

    if (Math.abs(d) <= EPS) return 0;
    return d > 0 ? 1 : -1;
}

export interface SeriesComputedPointForError {
    n: number;
    value: Complex | null;
    deviation?: number | null;
}

export function errorNormFromPoint(
    point: SeriesComputedPointForError,
    limit: Complex | null
): number | null {
    if (hasFiniteNumber(point.deviation)) return Math.abs(point.deviation);
    return errorNorm(point.value, limit);
}

export function realDiffSignFromPoint(
    point: SeriesComputedPointForError,
    limit: Complex | null
): -1 | 0 | 1 | null {
    if (hasFiniteNumber(point.deviation)) {
        if (point.deviation === 0) return 0;
        return point.deviation > 0 ? 1 : -1;
    }
    return realDiffSign(point.value, limit);
}

export function getSeriesComputedSorted(
    series: Series
): SeriesComputedPointForError[] {
    const raw = (series.computed ?? []).map((p) => ({
        n: p.n,
        value: p.value ?? null,
        deviation: p.deviation ?? null,
    }));

    const len = raw.length;
    if (len <= 1) return raw;

    let sorted = true;
    for (let i = 1; i < len; i++) {
        if (raw[i - 1].n > raw[i].n) {
            sorted = false;
            break;
        }
    }
    return sorted ? raw : [...raw].sort((a, b) => a.n - b.n);
}

export function analyzeSeriesComputedConvergence(
    series: Series
): SeriesComputedConvergenceAnalysis {
    const limit = series.limit ?? null;
    const pts = getSeriesComputedSorted(series);

    let prevErr: number | null = null;
    let prevSign: -1 | 0 | 1 | null = null;

    let stepsAnalyzed = 0;

    let signChangesCount = 0;
    const signChangeNs: number[] = [];

    let incCount = 0;
    let decCount = 0;
    let eqCount = 0;

    const violationsNs: number[] = [];

    let anyFiniteSign = false;

    for (const p of pts) {
        const err = errorNormFromPoint(p, limit);
        const sgn = realDiffSignFromPoint(p, limit);

        // sign changes of Re(S_n - S)
        if (sgn != null && sgn !== 0) {
            anyFiniteSign = true;

            if (prevSign == null) {
                prevSign = sgn;
            } else if (prevSign !== sgn) {
                signChangesCount += 1;
                signChangeNs.push(p.n);
                prevSign = sgn;
            }
        }

        // error monotonicity
        if (err != null) {
            if (prevErr != null) {
                stepsAnalyzed += 1;

                if (err > prevErr + EPS) {
                    incCount += 1;
                    violationsNs.push(p.n);
                } else if (err < prevErr - EPS) {
                    decCount += 1;
                } else {
                    eqCount += 1;
                }
            }
            prevErr = err;
        }
    }

    const sideRaw: SideType =
        stepsAnalyzed === 0 && !anyFiniteSign
            ? "unknown"
            : signChangesCount === 0
              ? "one_sided"
              : "two_sided";

    const monotonicityRaw: MonotonicityType = (() => {
        if (stepsAnalyzed === 0) return "unknown";

        const hasInc = incCount > 0;
        const hasDec = decCount > 0;
        const hasEq = eqCount > 0;

        if (hasDec && !hasInc && !hasEq) return "strict_decreasing_error";
        if (!hasDec && !hasInc && hasEq) return "constant_error";
        if (hasDec && !hasInc && hasEq) return "non_increasing_error";

        if (hasInc && !hasDec && !hasEq) return "strict_increasing_error";
        if (hasInc && !hasDec && hasEq) return "non_decreasing_error";

        if (hasInc && hasDec) return "random_error";

        // случаи “только eq” уже обработаны, остальное считаем случайным
        return "random_error";
    })();

    return {
        seriesId: series.id,

        sideRaw,
        monotonicityRaw,

        stepsAnalyzed,

        signChangesCount,
        signChangeNs,

        incCount,
        decCount,
        eqCount,

        violationsNs,
    };
}

export function applySideThreshold(
    sideRaw: SideType,
    signChangesCount: number,
    maxSignChangesForOneSided: number
): SideType {
    if (sideRaw === "unknown") return "unknown";
    return signChangesCount <= maxSignChangesForOneSided ? "one_sided" : "two_sided";
}

export function applyMonotonicityThreshold(
    monotonicityRaw: MonotonicityType,
    incCount: number,
    decCount: number,
    eqCount: number,
    maxViolationsForMonotone: number
): MonotonicityType {
    if (monotonicityRaw !== "random_error") return monotonicityRaw;

    // если увеличений мало, принимаем как "почти невозрастающая"
    if (incCount <= maxViolationsForMonotone) return "non_increasing_error";

    // симметрично: если уменьшений мало, принимаем как "почти неубывающая"
    if (decCount <= maxViolationsForMonotone) return "non_decreasing_error";

    // иначе случайная
    return "random_error";
}

export function buildDetailPoints(series: Series): DetailPoint[] {
    const limit = series.limit ?? null;
    const sorted = getSeriesComputedSorted(series);

    let prevVal: Complex | null = null;

    const res: DetailPoint[] = [];

    for (const p of sorted) {
        const v = p.value ?? null;

        const valueRe = v?.re ?? null;
        const valueImRaw = v?.im ?? null;

        const err = errorNormFromPoint(p, limit);
        const sign = realDiffSignFromPoint(p, limit);

        let diffRe: number | null = null;
        let diffIm: number | null = null;
        let diffNorm: number | null = null;

        if (prevVal && v && hasFiniteNumber(prevVal.re) && hasFiniteNumber(v.re)) {
            const prevIm = normalizeIm(prevVal.im);
            const curIm = normalizeIm(v.im);

            const dRe = v.re - prevVal.re;
            const dIm = curIm - prevIm;

            const dn = Math.hypot(dRe, dIm);

            diffRe = Number.isFinite(dRe) ? dRe : null;
            diffIm = Number.isFinite(dIm) ? dIm : null;
            diffNorm = Number.isFinite(dn) ? dn : null;
        }

        prevVal = v;

        res.push({
            n: p.n,
            valueRe,
            valueIm: valueImRaw,
            err,
            sign,
            diffRe,
            diffIm,
            diffNorm,
        });
    }

    return res;
}

export function formatSideShort(side: SideType): string {
    if (side === "one_sided") return "1s";
    if (side === "two_sided") return "2s";
    return "?";
}

export function formatMonotonicityShort(mon: MonotonicityType): string {
    switch (mon) {
        case "strict_decreasing_error":
            return "↓↓";
        case "non_increasing_error":
            return "↓";
        case "constant_error":
            return "=";
        case "strict_increasing_error":
            return "↑↑";
        case "non_decreasing_error":
            return "↑";
        case "random_error":
            return "~";
        default:
            return "?";
    }
}

export function describeClass(side: SideType, mon: MonotonicityType): string {
    const mono =
        mon === "strict_decreasing_error" ||
        mon === "non_increasing_error" ||
        mon === "constant_error";

    if (side === "one_sided" && mono) return "односторонний и монотонный";
    if (side === "one_sided" && !mono) return "односторонний и немонотонный";
    if (side === "two_sided" && mono) return "двусторонний и монотонный";
    if (side === "two_sided" && !mono) return "двусторонний и немонотонный";
    return "недостаточно данных";
}

export function nonNullEntries<T extends Record<string, unknown>>(obj: T | null | undefined) {
    if (!obj) return [] as [string, unknown][];
    return Object.entries(obj).filter(([, v]) => v !== null && v !== undefined);
}

export function buildArgsSummary(args: SeriesArgs | null): string {
    const entries = nonNullEntries(args);
    if (entries.length === 0) return "";
    entries.sort(([a], [b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${String(v)}`).join(", ");
}

export function formatIntervals(ns: number[], maxRanges = 5): string {
    if (!ns.length) return "—";

    const sorted = Array.from(new Set(ns)).sort((a, b) => a - b);

    const ranges: Array<{ start: number; end: number }> = [];
    let start = sorted[0];
    let prev = sorted[0];

    for (let i = 1; i < sorted.length; i++) {
        const x = sorted[i];
        if (x === prev + 1) {
            prev = x;
            continue;
        }
        ranges.push({ start, end: prev });
        start = x;
        prev = x;
    }
    ranges.push({ start, end: prev });

    const parts = ranges
        .slice(0, maxRanges)
        .map((r) => (r.start === r.end ? `${r.start}` : `${r.start}–${r.end}`));

    if (ranges.length > maxRanges) parts.push("…");
    return parts.join(", ");
}

export function getSeriesRowDomId(seriesId: string): string {
    return `series-computed-row-${seriesId}`;
}
