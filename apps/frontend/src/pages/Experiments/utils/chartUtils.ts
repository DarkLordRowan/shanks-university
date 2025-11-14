import type { ApiJsonResult, ChartGroup, XY } from "../types";

export const toNum = (v: any): number | null => {
    if (typeof v === "number" && Number.isFinite(v)) return v;
    if (v == null) return null;
    const n = Number(String(v).replace(",", "."));
    return Number.isFinite(n) ? n : null;
};

export const fmt = (v: number) => (v >= 1e4 || v < 1e-3 ? v.toExponential(2) : v.toFixed(2));

export const niceTicks = (min: number, max: number, count = 5) => {
    if (min === max) return [min];
    const span = max - min;
    const step = Math.pow(10, Math.floor(Math.log10(span / count)));
    const err = span / (count * step);
    const mult = err >= 7.5 ? 10 : err >= 3 ? 5 : err >= 1.5 ? 2 : 1;
    const niceStep = step * mult;
    const t0 = Math.ceil(min / niceStep) * niceStep;
    const ticks: number[] = [];
    for (let t = t0; t <= max + 1e-12; t += niceStep) ticks.push(+t.toFixed(12));
    return ticks;
};

export const niceLogTicks = (minPos: number, maxPos: number) => {
    const a = Math.floor(Math.log10(minPos));
    const b = Math.ceil(Math.log10(maxPos));
    const ticks: number[] = [];
    for (let p = a; p <= b; p++) ticks.push(Math.pow(10, p));
    return ticks;
};

export const linePath = (pts: XY[], xScale: (n: number) => number, yScale: (n: number) => number) =>
    pts.map((p, i) => `${i ? "L" : "M"}${xScale(p.x)},${yScale(p.y)}`).join(" ");

export function buildChartGroups(jsonResult: ApiJsonResult | null): ChartGroup[] {
    if (!Array.isArray(jsonResult)) return [];
    const byKey: Record<string, ChartGroup> = {};

    for (const block of jsonResult) {
        const s = block?.series ?? {};
        const a = block?.accel ?? {};
        const seriesName = s?.name ?? "";
        const xVal = s?.arguments && typeof s.arguments === "object" && "x" in s.arguments ? s.arguments.x : "";
        const lim = toNum(s?.lim);
        const accelName = a?.name ?? "";
        const m = a?.m_value ?? "";
        const blkId = typeof block?.id === "string" ? block.id : "";

        const key = `${seriesName}|x=${xVal}|${accelName}|m=${m}`;
        if (!byKey[key]) {
            byKey[key] = {
                id: key,
                caption: `${seriesName} (x=${xVal}${lim != null ? `, lim=${lim}` : ""}) · ${accelName}${
                    m !== "" ? ` (m=${m})` : ""
                }${blkId ? ` · id=${blkId}` : ""}`,
                limit: lim,
                partial: [],
                accel: [],
                errPartial: [],
                errAccel: [],
                ratio: [],
            };
        }
        const items: any[] = Array.isArray(block?.computed) ? block.computed : [];
        for (const it of items) {
            const n = toNum(it?.n);
            if (n == null) continue;

            const ps = toNum(it?.partial_sum);
            if (ps != null) byKey[key].partial.push({ x: n, y: ps });

            const av = toNum(it?.accel_value);
            if (av != null) byKey[key].accel.push({ x: n, y: av });

            if (lim != null && ps != null) byKey[key].errPartial.push({ x: n, y: Math.abs(ps - lim) });
            if (lim != null && av != null) byKey[key].errAccel.push({ x: n, y: Math.abs(av - lim) });

            if (lim != null && ps != null && av != null) {
                const eP = Math.abs(ps - lim);
                const eA = Math.abs(av - lim);
                if (eA > 0) byKey[key].ratio.push({ x: n, y: eP / eA });
            }
        }
    }

    const groups = Object.values(byKey);
    for (const g of groups) {
        g.partial.sort((a, b) => a.x - b.x);
        g.accel.sort((a, b) => a.x - b.x);
        g.errPartial.sort((a, b) => a.x - b.x);
        g.errAccel.sort((a, b) => a.x - b.x);
        g.ratio.sort((a, b) => a.x - b.x);
    }
    return groups;
}
