import { describe, expect, it } from "vitest";
import type { Series } from "@/entities/experiment/model/experiment";
import {
    buildDetailPoints,
} from "./seriesComputedConvergenceUtils";
import {
    buildSeriesComputedClassLegendTitle,
    computeSeriesComputedDevStats,
    getSeriesComputedClassInfo,
} from "./seriesComputedSummary";

function buildSeries(
    computed: NonNullable<Series["computed"]>,
    id: string = "s-1"
): Series {
    return {
        id,
        name: `Series ${id}`,
        precision: "double",
        args: { x: 1 },
        limit: { re: 1, im: 0 },
        computed,
    };
}

describe("seriesComputedSummary", () => {
    it("computes min n, last-minus-min, last/min amp, plateau, and max/min amp", () => {
        const series = buildSeries([
            { n: 1, value: { re: 1.1, im: 0 } },
            { n: 2, value: { re: 1.01, im: 0 } },
            { n: 3, value: { re: 1.001, im: 0 } },
            { n: 4, value: { re: 1.001, im: 0 } },
        ]);

        const stats = computeSeriesComputedDevStats(series);

        expect(stats.count).toBe(4);
        expect(stats.minN).toBe(3);
        expect(stats.lastN).toBe(4);
        expect(stats.lastMinusMin).toBeCloseTo(0);
        expect(stats.plateauStartN).toBe(3);
        expect(stats.amplitudeOrders).toBeCloseTo(0);
        expect(stats.maxAmplitudeOrders).toBeCloseTo(2);
    });

    it("treats log10(0) as 0 for both last/min and max/min amp", () => {
        const series = buildSeries([
            { n: 1, value: { re: 1.1, im: 0 } },
            { n: 2, value: { re: 1, im: 0 } },
        ]);

        const stats = computeSeriesComputedDevStats(series);

        expect(stats.min).toBe(0);
        expect(stats.amplitudeOrders).toBeCloseTo(0);
        expect(stats.maxAmplitudeOrders).toBeCloseTo(-1);
    });

    it("uses stored deviations when JS-rounded values equal the limit", () => {
        const series = buildSeries([
            { n: 1, value: { re: 1, im: 0 }, deviation: -3.5810259903450162e-34 },
        ]);

        const stats = computeSeriesComputedDevStats(series);
        const points = buildDetailPoints(series);

        expect(stats.min).toBeCloseTo(3.5810259903450162e-34);
        expect(stats.min).not.toBe(0);
        expect(points[0]?.err).toBeCloseTo(3.5810259903450162e-34);
        expect(points[0]?.sign).toBe(-1);
    });

    it("assigns requested class order and legend text", () => {
        const staticSeries = buildSeries([
            { n: 1, value: { re: 1.1, im: 0 } },
            { n: 2, value: { re: 1.1, im: 0 } },
        ]);
        const lateMinSeries = buildSeries(
            [
                { n: 1, value: { re: 1.2, im: 0 } },
                { n: 2, value: { re: 1.05, im: 0 } },
                { n: 3, value: { re: 1.01, im: 0 } },
            ],
            "s-2"
        );
        const firstMinSeries = buildSeries(
            [
                { n: 1, value: { re: 1.001, im: 0 } },
                { n: 2, value: { re: 1.1, im: 0 } },
                { n: 3, value: { re: 1.2, im: 0 } },
            ],
            "s-3"
        );

        const staticInfo = getSeriesComputedClassInfo(
            "one_sided",
            "constant_error",
            computeSeriesComputedDevStats(staticSeries)
        );
        const lateMinInfo = getSeriesComputedClassInfo(
            "one_sided",
            "strict_decreasing_error",
            computeSeriesComputedDevStats(lateMinSeries)
        );
        const firstMinInfo = getSeriesComputedClassInfo(
            "two_sided",
            "random_error",
            computeSeriesComputedDevStats(firstMinSeries)
        );

        expect(staticInfo.order).toBeLessThan(lateMinInfo.order);
        expect(lateMinInfo.label).toBe("1s ↓↓");
        expect(firstMinInfo.label).toBe("2s ↑↑");
        expect(firstMinInfo.order).toBeGreaterThan(lateMinInfo.order);

        const legend = buildSeriesComputedClassLegendTitle(0, 0);
        expect(legend).toContain("↓→");
        expect(legend).toContain("↑↑");
        expect(legend).toContain("violet");
    });
});
