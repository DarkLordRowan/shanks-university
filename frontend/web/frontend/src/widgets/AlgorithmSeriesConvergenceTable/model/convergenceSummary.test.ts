import { describe, expect, it } from "vitest";
import type { Complex, SeriesAccel } from "@/entities/experiment/model/experiment";
import {
    buildConvergenceClassLegendTitle,
    buildConvergenceDetailPoints,
    computeSeriesAlgoAmplitudeOrders,
    computeConvergenceDevStats,
    computeConvergenceDevStatsFromSeriesAccel,
    getConvergenceClassInfo,
} from "./convergenceSummary";

function buildSeriesAccel(computed: SeriesAccel["computed"]): SeriesAccel {
    return {
        series_id: "series-1",
        accel_id: "algo-1",
        computed,
        errors: [],
        events: [],
    };
}

const LIMIT: Complex = { re: 1, im: 0 };

describe("convergenceSummary", () => {
    it("computes min n, last-minus-min, last/min amp, plateau, and max/min amp for A_n", () => {
        const accel = buildSeriesAccel([
            { n: 4, value: { re: 1.001, im: 0 } },
            { n: 1, value: { re: 1.1, im: 0 } },
            { n: 3, value: { re: 1.001, im: 0 } },
            { n: 2, value: { re: 1.01, im: 0 } },
        ]);

        const stats = computeConvergenceDevStatsFromSeriesAccel(accel, LIMIT);

        expect(stats.count).toBe(4);
        expect(stats.minN).toBe(3);
        expect(stats.lastN).toBe(4);
        expect(stats.lastMinusMin).toBeCloseTo(0);
        expect(stats.plateauStartN).toBe(3);
        expect(stats.amplitudeOrders).toBeCloseTo(0);
        expect(stats.maxAmplitudeOrders).toBeCloseTo(2);
    });

    it("treats log10(0) as 0 for both last/min and max/min amp", () => {
        const accel = buildSeriesAccel([
            { n: 1, value: { re: 1.1, im: 0 } },
            { n: 2, value: { re: 1, im: 0 } },
        ]);

        const stats = computeConvergenceDevStatsFromSeriesAccel(accel, LIMIT);

        expect(stats.min).toBe(0);
        expect(stats.amplitudeOrders).toBeCloseTo(0);
        expect(stats.maxAmplitudeOrders).toBeCloseTo(-1);
    });

    it("uses stored deviations when JS-rounded values equal the limit", () => {
        const accel = buildSeriesAccel([
            { n: 1, value: { re: 1, im: 0 }, deviation: -3.5810259903450162e-34 },
        ]);

        const points = buildConvergenceDetailPoints(accel, LIMIT);
        const stats = computeConvergenceDevStatsFromSeriesAccel(accel, LIMIT);

        expect(stats.min).toBeCloseTo(3.5810259903450162e-34);
        expect(stats.min).not.toBe(0);
        expect(points[0]?.err).toBeCloseTo(3.5810259903450162e-34);
        expect(points[0]?.sign).toBe(-1);
    });

    it("matches stats built from detail points", () => {
        const accel = buildSeriesAccel([
            { n: 1, value: { re: 1.2, im: 0 } },
            { n: 2, value: { re: 1.05, im: 0 } },
            { n: 3, value: { re: 1.01, im: 0 } },
        ]);

        const points = buildConvergenceDetailPoints(accel, LIMIT);
        const fromPoints = computeConvergenceDevStats(points);
        const fromSeriesAccel = computeConvergenceDevStatsFromSeriesAccel(accel, LIMIT);

        expect(fromPoints.minN).toBe(fromSeriesAccel.minN);
        expect(fromPoints.lastMinusMin).toBeCloseTo(fromSeriesAccel.lastMinusMin ?? NaN);
        expect(fromPoints.amplitudeOrders).toBeCloseTo(fromSeriesAccel.amplitudeOrders ?? NaN);
        expect(fromPoints.maxAmplitudeOrders).toBeCloseTo(
            fromSeriesAccel.maxAmplitudeOrders ?? NaN
        );
    });

    it("treats positive series/algo amp as algorithm better than the series", () => {
        expect(computeSeriesAlgoAmplitudeOrders(1e-4, 1e-6)).toBeCloseTo(2);
        expect(computeSeriesAlgoAmplitudeOrders(1e-6, 1e-4)).toBeCloseTo(-2);
        expect(computeSeriesAlgoAmplitudeOrders(0, 1e-4)).toBeCloseTo(-4);
        expect(computeSeriesAlgoAmplitudeOrders(0, 0)).toBe(0);
    });

    it("assigns requested class order and legend text", () => {
        const staticInfo = getConvergenceClassInfo("one_sided", "constant_error", {
            count: 2,
            first: 0.1,
            firstN: 1,
            min: 0.1,
            minN: 1,
            mean: 0.1,
            median: 0.1,
            max: 0.1,
            last: 0.1,
            lastN: 2,
            lastMinusMin: 0,
            amplitudeOrders: 0,
            maxAmplitudeOrders: 0,
            plateauStartN: 1,
        });
        const lateMinInfo = getConvergenceClassInfo("one_sided", "strict_decreasing_error", {
            count: 3,
            first: 0.2,
            firstN: 1,
            min: 0.01,
            minN: 3,
            mean: 0.1,
            median: 0.05,
            max: 0.2,
            last: 0.01,
            lastN: 3,
            lastMinusMin: 0,
            amplitudeOrders: 0,
            maxAmplitudeOrders: 1.30103,
            plateauStartN: 3,
        });
        const firstMinInfo = getConvergenceClassInfo("two_sided", "random_error", {
            count: 3,
            first: 0.001,
            firstN: 1,
            min: 0.001,
            minN: 1,
            mean: 0.1005,
            median: 0.1,
            max: 0.2,
            last: 0.2,
            lastN: 3,
            lastMinusMin: 0.199,
            amplitudeOrders: 2.30103,
            maxAmplitudeOrders: 2.30103,
            plateauStartN: 1,
        });

        expect(staticInfo.order).toBeLessThan(lateMinInfo.order);
        expect(lateMinInfo.label).toBe("1s ↓↓");
        expect(firstMinInfo.label).toBe("2s ↑↑");
        expect(firstMinInfo.order).toBeGreaterThan(lateMinInfo.order);

        const legend = buildConvergenceClassLegendTitle(0, 0);
        expect(legend).toContain("↓→");
        expect(legend).toContain("↑↑");
        expect(legend).toContain("violet");
    });
});
