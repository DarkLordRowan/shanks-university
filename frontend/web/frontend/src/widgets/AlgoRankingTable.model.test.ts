import { describe, expect, it } from "vitest";
import type { Experiment } from "@/entities/experiment/model/experiment";
import {
    buildAlgoStatsFromExperiment,
    getVisibleArgColumnCount,
} from "./AlgoRankingTable.model";

function buildExperiment(): Experiment {
    return {
        id: "exp-ranking",
        seriesList: [
            {
                id: "s-1",
                name: "S1",
                precision: "double",
                args: { x: 1 },
                limit: { re: 1, im: 0 },
                computed: [
                    { n: 1, value: { re: 0, im: 0 }, deviation: 1e-2 },
                    { n: 2, value: { re: 0, im: 0 }, deviation: 1e-4 },
                    { n: 3, value: { re: 0, im: 0 }, deviation: 1e-4 },
                ],
            },
            {
                id: "s-2",
                name: "S2",
                precision: "double",
                args: { x: 2 },
                limit: { re: 1, im: 0 },
                computed: [
                    { n: 1, value: { re: 0, im: 0 }, deviation: 1e-1 },
                    { n: 2, value: { re: 0, im: 0 }, deviation: 0 },
                ],
            },
        ],
        accelList: [
            { id: "a-fast", name: "Fast", m: 2, args: { alpha: 1, beta: 2 } },
            { id: "a-slow", name: "Slow", m: 3, args: { alpha: 1 } },
        ],
        seriesAccelList: [
            {
                series_id: "s-1",
                accel_id: "a-fast",
                computed: [
                    { n: 1, value: { re: 0, im: 0 }, deviation: 1e-3 },
                    { n: 2, value: { re: 0, im: 0 }, deviation: 1e-5 },
                    { n: 3, value: { re: 0, im: 0 }, deviation: 1e-5 },
                ],
                errors: [],
                events: [],
            },
            {
                series_id: "s-1",
                accel_id: "a-slow",
                computed: [
                    { n: 1, value: { re: 0, im: 0 }, deviation: 1e-2 },
                    { n: 2, value: { re: 0, im: 0 }, deviation: 1e-4 },
                ],
                errors: [],
                events: [],
            },
            {
                series_id: "s-2",
                accel_id: "a-fast",
                computed: [
                    { n: 1, value: { re: 0, im: 0 }, deviation: 1e-2 },
                    { n: 2, value: { re: 0, im: 0 }, deviation: 0 },
                ],
                errors: [],
                events: [],
            },
            {
                series_id: "s-2",
                accel_id: "a-slow",
                computed: [
                    { n: 1, value: { re: 0, im: 0 }, deviation: 1e-1 },
                    { n: 2, value: { re: 0, im: 0 }, deviation: 1e-3 },
                ],
                errors: [],
                events: [],
            },
        ],
    };
}

describe("AlgoRankingTable.model", () => {
    it("computes requested ranking metrics from min and last deviations", () => {
        const stats = buildAlgoStatsFromExperiment(buildExperiment(), 1e-4, null);

        expect(stats).toHaveLength(2);
        expect(getVisibleArgColumnCount(stats)).toBe(2);

        const fast = stats.find((item) => item.algorithmName === "Fast");
        const slow = stats.find((item) => item.algorithmName === "Slow");

        expect(fast).toBeTruthy();
        expect(slow).toBeTruthy();

        expect(fast?.arg1).toBe("alpha=1");
        expect(fast?.arg2).toBe("beta=2");
        expect(fast?.avgBestDeviation).toBeCloseTo(5e-6);
        expect(fast?.avgRelativeError).toBeCloseTo(0.55);
        expect(fast?.avgMinDeviationN).toBe(2);
        expect(fast?.avgStepsToTol).toBe(2);
        expect(fast?.bestMinShare).toBe(1);
        expect(fast?.worstMinShare).toBe(0);
        expect(fast?.bestLastShare).toBe(1);
        expect(fast?.worstLastShare).toBe(0);
        expect(fast?.rankPrecision).toBe(1);
        expect(fast?.rankSpeed).toBe(1);
        expect(fast?.rankStability).toBe(1);

        expect(slow?.avgBestDeviation).toBeCloseTo(5.5e-4);
        expect(slow?.avgMinDeviationN).toBe(2);
        expect(slow?.bestMinShare).toBe(0);
        expect(slow?.worstMinShare).toBe(1);
        expect(slow?.bestLastShare).toBe(0);
        expect(slow?.worstLastShare).toBe(1);
        expect(slow?.avgRelativeError).toBe(Number.POSITIVE_INFINITY);

        expect((fast?.totalRankScore ?? Infinity) < (slow?.totalRankScore ?? 0)).toBe(true);
    });

    it("treats equal metrics as ties when assigning ranks", () => {
        const experiment: Experiment = {
            id: "exp-tie",
            seriesList: [
                {
                    id: "s-1",
                    name: "S1",
                    precision: "double",
                    args: { x: 1 },
                    limit: { re: 1, im: 0 },
                    computed: [{ n: 1, value: { re: 0, im: 0 }, deviation: 1e-2 }],
                },
            ],
            accelList: [
                { id: "a-1", name: "A1", m: 2, args: { alpha: 1, beta: 2, gamma: 3 } },
                { id: "a-2", name: "A2", m: 2, args: { alpha: 4, beta: 5, gamma: 6 } },
            ],
            seriesAccelList: [
                {
                    series_id: "s-1",
                    accel_id: "a-1",
                    computed: [{ n: 1, value: { re: 0, im: 0 }, deviation: 1e-3 }],
                    errors: [],
                    events: [],
                },
                {
                    series_id: "s-1",
                    accel_id: "a-2",
                    computed: [{ n: 1, value: { re: 0, im: 0 }, deviation: 1e-3 }],
                    errors: [],
                    events: [],
                },
            ],
        };

        const stats = buildAlgoStatsFromExperiment(experiment, 1e-4, null);

        expect(getVisibleArgColumnCount(stats)).toBe(3);
        expect(stats).toHaveLength(2);
        expect(stats[0].rankPrecision).toBe(1);
        expect(stats[1].rankPrecision).toBe(1);
        expect(stats[0].rankSpeed).toBe(1);
        expect(stats[1].rankSpeed).toBe(1);
        expect(stats[0].rankStability).toBe(1);
        expect(stats[1].rankStability).toBe(1);
        expect(stats[0].totalRankScore).toBe(stats[1].totalRankScore);
        expect(stats[0].bestMinShare).toBe(1);
        expect(stats[1].bestMinShare).toBe(1);
        expect(stats[0].worstMinShare).toBe(1);
        expect(stats[1].worstMinShare).toBe(1);
    });
});
