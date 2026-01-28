import {
    type SeriesAccel,
    type Profiling,
    type ProfilingAnalysis,
    type OpsTrendType,
    type SeriesArgs,
    type AccelArgs,
} from "./types";

type Scalar = number | string | boolean | null;

export function hasFiniteNumber(v: number | null | undefined): v is number {
    return v != null && Number.isFinite(v);
}

export function nonNullEntries<T extends Record<string, unknown>>(obj: T | null | undefined) {
    if (!obj) return [] as [string, unknown][];
    return Object.entries(obj).filter(([, v]) => v !== null && v !== undefined);
}

function toSortableNumber(v: Scalar): number | null {
    if (typeof v === "number") return Number.isFinite(v) ? v : null;
    if (typeof v === "string") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    return null;
}

export function parseX(args: SeriesArgs | null): { xLabel: string; xSort: number | null } {
    const raw = (args?.x as Scalar | undefined) ?? null;
    if (raw == null) return { xLabel: "∅", xSort: null };
    return { xLabel: String(raw), xSort: toSortableNumber(raw) };
}

export function buildArgsSummary(args: AccelArgs | null): string {
    const entries = nonNullEntries(args);
    if (entries.length === 0) return "";
    entries.sort(([a], [b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

export function getProfilingCellDomId(accelId: string, seriesId: string): string {
    return `profiling-cell-${accelId}::${seriesId}`;
}

export function formatOps(v: number | null): string {
    if (!hasFiniteNumber(v)) return "∅";
    const a = Math.abs(v);
    if (a >= 1e9) return `${trimZeros((v / 1e9).toFixed(2))}G`;
    if (a >= 1e6) return `${trimZeros((v / 1e6).toFixed(2))}M`;
    if (a >= 1e3) return `${trimZeros((v / 1e3).toFixed(2))}K`;
    return String(Math.trunc(v));
}

function trimZeros(s: string): string {
    return s.replace(/0+$/g, "").replace(/\.$/, "");
}

export function formatTrendShort(t: OpsTrendType): string {
    switch (t) {
        case "decreasing_error":
            return "↓↓";
        case "constant":
            return "=";
        case "increasing":
            return "↑↑";
        case "non_decreasing":
            return "↑";
        default:
            return "?";
    }
}


function profilingToKinds(pr: Profiling | null | undefined): Record<string, number | null> {
    if (!pr) return {};
    return {
        add: hasFiniteNumber(pr.add) ? pr.add : null,
        mul: hasFiniteNumber(pr.mul) ? pr.mul : null,
        div: hasFiniteNumber(pr.div) ? pr.div : null,
        special: hasFiniteNumber(pr.special) ? pr.special : null,
    };
}

function sumKinds(kinds: Record<string, number | null>): number | null {
    const vals = Object.values(kinds).filter((v): v is number => typeof v === "number" && Number.isFinite(v));
    if (vals.length === 0) return null;
    return vals.reduce((a, b) => a + b, 0);
}

function trendFromDeltas(deltas: number[]): OpsTrendType {
    if (deltas.length === 0) return "unknown";
    if (deltas.some((d) => d < 0)) return "decreasing_error";
    if (deltas.every((d) => d === 0)) return "constant";
    if (deltas.every((d) => d > 0)) return "increasing";
    return "non_decreasing";
}

function getComputedPointsSortedByN(sa: SeriesAccel) {
    const raw = sa.computed ?? [];
    if (raw.length <= 1) return raw;

    let sorted = true;
    for (let i = 1; i < raw.length; i++) {
        if (raw[i - 1].n > raw[i].n) {
            sorted = false;
            break;
        }
    }
    return sorted ? raw : [...raw].sort((a, b) => a.n - b.n);
}

export function analyzeSeriesAccelProfiling(sa: SeriesAccel): ProfilingAnalysis {
    const pts = getComputedPointsSortedByN(sa);

    let prevTotal: number | null = null;
    const prevByKind: Record<string, number | null> = {};

    const deltasTotal: number[] = [];
    const decreaseNsTotal: number[] = [];

    let maxTotalOps: number | null = null;
    let maxTotalOpsN: number | null = null;

    const maxByKind: Record<string, { max: number; n: number }> = {};
    const decreaseNsByKind: Record<string, number[]> = {};

    let stepsAnalyzed = 0;

    for (const p of pts) {
        const n = p.n;
        const kinds = profilingToKinds(p.profiling);
        const total = sumKinds(kinds);

        if (total != null) {
            if (maxTotalOps == null || total > maxTotalOps) {
                maxTotalOps = total;
                maxTotalOpsN = n;
            }
        }

        for (const [k, v] of Object.entries(kinds)) {
            if (!hasFiniteNumber(v)) continue;

            const m = maxByKind[k];
            if (!m || v > m.max) maxByKind[k] = { max: v, n };

            const pv = prevByKind[k];
            if (hasFiniteNumber(pv)) {
                const d = v - pv;
                if (d < 0) (decreaseNsByKind[k] ??= []).push(n);
            }
            prevByKind[k] = v;
        }

        if (hasFiniteNumber(prevTotal) && hasFiniteNumber(total)) {
            const d = total - prevTotal;
            deltasTotal.push(d);
            stepsAnalyzed++;
            if (d < 0) decreaseNsTotal.push(n);
        }

        prevTotal = hasFiniteNumber(total) ? total : null;
    }

    const trendTotal = trendFromDeltas(deltasTotal);

    const topKinds = Object.entries(maxByKind)
        .sort((a, b) => b[1].max - a[1].max)
        .slice(0, 8)
        .map(([k]) => k);

    return {
        seriesId: sa.series_id,
        accelId: sa.accel_id,

        stepsAnalyzed,

        trendTotal,
        maxTotalOps,
        maxTotalOpsN,
        decreaseNsTotal,

        maxByKind,
        decreaseNsByKind,
        topKinds,
    };
}
