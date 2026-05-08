import { describe, expect, it } from "vitest";
import type { Experiment } from "@/entities/experiment/model/experiment";
import {
    buildAlgoStatsFromExperiment,
    computeHowMuch,
    computeHowMuchFormula,
    createAlgoRankingStatsAccumulator,
    getVisibleArgColumnCount,
    getVisibleArgKeys,
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
                    { n: 3, value: { re: 0, im: 0 }, deviation: 1e-3 },
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
    it("computes how_much for known algorithm families", () => {
        expect(computeHowMuch({ name: "Anderson Acceleration", m: 4, args: null }, 10)).toBe(11);
        expect(computeHowMuch({ name: "Chang-Wynn Algorithm", m: 4, args: null }, 10)).toBe(11);
        expect(computeHowMuch({ name: "Richardson Algorithm", m: 4, args: null }, 10)).toBe(11);
        expect(computeHowMuch({ name: "Ford-Sidi 2 Algorithm", m: 4, args: null }, 10)).toBe(12);
        expect(computeHowMuch({ name: "Ford-Sidi 3 Algorithm", m: 4, args: null }, 10)).toBe(10);
        expect(computeHowMuch({ name: "Brezinski Theta Algorithm", m: 4, args: null }, 10)).toBe(17);
        expect(computeHowMuch({ name: "Weniger Algorithm", m: 4, args: null }, 10)).toBe(15);
        expect(computeHowMuch({ name: "Shanks Alternating", m: 4, args: null }, 10)).toBe(15);
        expect(computeHowMuch({ name: "Shanks Algorithm", m: 4, args: null }, 10)).toBe(19);
        expect(computeHowMuch({ name: "Overholt Algorithm", m: 4, args: null }, 10)).toBe(19);
        expect(computeHowMuch({ name: "Wynn Epsilon 3 Algorithm", m: 4, args: null }, 10)).toBe(19);
        expect(computeHowMuch({ name: "Lubkin W Algorithm", m: 4, args: null }, 10)).toBe(23);
        expect(computeHowMuch({ name: "J-Transformation", m: 4, args: null }, 10)).toBe(15);
        expect(computeHowMuch({ name: "Unknown", m: 4, args: null }, 10)).toBe(10);
        expect(computeHowMuchFormula({ name: "Wynn Epsilon 3 Algorithm", m: 4, args: null })).toBe(
            "n + 2*order + 1"
        );
        expect(computeHowMuchFormula({ name: "Unknown", m: 4, args: null })).toBe("n");
    });

    it("computes conditional how_much for rho and remainder variants", () => {
        expect(
            computeHowMuch({ name: "Wynn Rho Algorithm", m: 3, args: { numerator_type: "rho_type" } }, 10)
        ).toBe(17);
        expect(
            computeHowMuch({ name: "Wynn Rho Algorithm", m: 3, args: { rho_type: true } }, 10)
        ).toBe(17);
        expect(
            computeHowMuch({ name: "Wynn Rho Algorithm", m: 3, args: { rho: "1.0" } }, 10)
        ).toBe(17);
        expect(
            computeHowMuch({ name: "Wynn Rho Algorithm", m: 3, args: { rho: 0 } }, 10)
        ).toBe(14);
        expect(
            computeHowMuch(
                { name: "Wynn Rho Algorithm", m: 3, args: { numerator_type: "gamma_rho_type" } },
                10
            )
        ).toBe(14);
        expect(
            computeHowMuch({ name: "pJ Algorithm", m: 3, args: { remainder_type: "t_wave" } }, 10)
        ).toBe(15);
        expect(
            computeHowMuch({ name: "Levin-Sidi S Algorithm", m: 3, args: { type: "v_type" } }, 10)
        ).toBe(15);
        expect(
            computeHowMuch({ name: "Drummond D Algorithm", m: 3, args: { remainder: "v_wave_type" } }, 10)
        ).toBe(16);
        expect(computeHowMuch({ name: "Levin-Sidi S Algorithm", m: 3, args: { type: "t~" } }, 10)).toBe(15);
        expect(computeHowMuch({ name: "Levin-Sidi S Algorithm", m: 3, args: { type: "v~" } }, 10)).toBe(16);
        expect(computeHowMuch({ name: "recLevinSidiSAlgorithmT~", m: 3, args: null }, 10)).toBe(15);
        expect(computeHowMuch({ name: "recLevinSidiSAlgorithmV~", m: 3, args: null }, 10)).toBe(16);
        expect(computeHowMuch({ name: "recLevinSidiSAlgorithmVType", m: 3, args: null }, 10)).toBe(15);
        expect(computeHowMuch({ name: "recLevinSidiSAlgorithmVWaveType", m: 3, args: null }, 10)).toBe(16);
        expect(
            computeHowMuchFormula({ name: "Wynn Rho Algorithm", m: 3, args: { rho_type: true } })
        ).toBe("n + 2*order + 1 (rho_type)");
        expect(
            computeHowMuchFormula({ name: "Drummond D Algorithm", m: 3, args: { remainder: "v_wave_type" } })
        ).toBe("n + order + 1 + delta, delta=2");
        expect(computeHowMuchFormula({ name: "Levin-Sidi S Algorithm", m: 3, args: { type: "v~" } })).toBe(
            "n + order + 1 + delta, delta=2"
        );
        expect(computeHowMuchFormula({ name: "recLevinSidiSAlgorithmV~", m: 3, args: null })).toBe(
            "n + order + 1 + delta, delta=2"
        );
        expect(computeHowMuchFormula({ name: "recLevinSidiSAlgorithmVWaveType", m: 3, args: null })).toBe(
            "n + order + 1 + delta, delta=2"
        );
    });

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
        expect(fast?.baseAlgorithmName).toBe("Fast");
        expect(fast?.variant).toBe("raw");
        expect(fast?.howMuchFormula).toBe("n");
        expect(fast?.avgBestDeviation).toBeCloseTo(5e-6);
        expect(fast?.medianBestDeviation).toBeCloseTo(5e-6);
        expect(fast?.worstBestDeviation).toBeCloseTo(1e-5);
        expect(fast?.avgRelativeError).toBeCloseTo(0.1);
        expect(fast?.avgOrdersGain).toBeCloseTo(0.5);
        expect(fast?.avgAmpAtMinN).toBeCloseTo(0.5);
        expect(fast?.notBetterThanSeriesShare).toBeCloseTo(0.5);
        expect(fast?.avgMinDeviationN).toBe(2);
        expect(fast?.medianMinDeviationN).toBe(2);
        expect(fast?.worstMinDeviationN).toBe(2);
        expect(fast?.avgLastMinusMin).toBe(0);
        expect(fast?.avgStepsToTol).toBe(2);
        expect(fast?.avgEpsSavedSteps).toBe(0);
        expect(fast?.medianEpsSavedSteps).toBe(0);
        expect(fast?.worstEpsSavedSteps).toBe(0);
        expect(fast?.bestMinShare).toBe(1);
        expect(fast?.worstMinShare).toBe(0);
        expect(fast?.bestLastShare).toBe(1);
        expect(fast?.worstLastShare).toBe(0);
        expect(fast?.rankPrecision).toBe(1);
        expect(fast?.rankSpeed).toBe(1);
        expect(fast?.rankStability).toBe(1);

        expect(slow?.avgBestDeviation).toBeCloseTo(5.5e-4);
        expect(slow?.medianBestDeviation).toBeCloseTo(5.5e-4);
        expect(slow?.worstBestDeviation).toBeCloseTo(1e-3);
        expect(slow?.avgOrdersGain).toBeCloseTo(-1.5);
        expect(slow?.avgAmpAtMinN).toBeCloseTo(-1.5);
        expect(slow?.notBetterThanSeriesShare).toBe(1);
        expect(slow?.avgMinDeviationN).toBe(2);
        expect(slow?.avgLastMinusMin).toBeCloseTo(4.5e-4);
        expect(slow?.medianLastMinusMin).toBeCloseTo(4.5e-4);
        expect(slow?.worstLastMinusMin).toBeCloseTo(9e-4);
        expect(slow?.bestMinShare).toBe(0);
        expect(slow?.worstMinShare).toBe(1);
        expect(slow?.bestLastShare).toBe(0);
        expect(slow?.worstLastShare).toBe(1);
        expect(slow?.avgRelativeError).toBe(1);

        expect((fast?.totalRankScore ?? Infinity) < (slow?.totalRankScore ?? 0)).toBe(true);
    });

    it("produces the same filtered stats through the progressive accumulator", () => {
        const experiment = buildExperiment();
        const allowedSeriesIds = new Set(["s-1"]);
        const allowedAccelIds = new Set(["a-fast"]);
        const expected = buildAlgoStatsFromExperiment(
            experiment,
            1e-4,
            "double",
            allowedSeriesIds,
            allowedAccelIds
        );
        const accumulator = createAlgoRankingStatsAccumulator({
            experiment,
            epsilon: 1e-4,
            precisionFilter: "double",
            allowedSeriesIds,
            allowedAccelIds,
        });

        for (const seriesAccel of experiment.seriesAccelList.slice(0, 2)) {
            accumulator.processSeriesAccel(seriesAccel);
        }
        for (const seriesAccel of experiment.seriesAccelList.slice(2)) {
            accumulator.processSeriesAccel(seriesAccel);
        }

        expect(accumulator.finalize()).toEqual(expected);
        expect(accumulator.finalize()).toEqual(expected);
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

    it("exposes real argument keys for ranking table comparison", () => {
        const experiment: Experiment = {
            id: "exp-arg-keys",
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
                {
                    id: "a-1",
                    name: "Base",
                    m: 2,
                    args: { alpha: 1 },
                    variant: "raw",
                },
                {
                    id: "a-1-filtered",
                    name: "Base [filtered: Savitzky-Golay: window_length=7]",
                    m: 2,
                    args: {
                        alpha: 1,
                        filtered: true,
                        filter_method: "Savitzky-Golay",
                        filter_window_length: "7",
                        filter_polyorder: "2",
                    },
                    variant: "filtered",
                    baseAccelId: "a-1",
                    filteredMethodName: "Savitzky-Golay: window_length=7",
                },
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
                    accel_id: "a-1-filtered",
                    computed: [{ n: 1, value: { re: 0, im: 0 }, deviation: 1e-4 }],
                    errors: [],
                    events: [],
                },
            ],
        };

        const stats = buildAlgoStatsFromExperiment(experiment, 1e-4, null);
        const filtered = stats.find((item) => item.variant === "filtered");

        expect(getVisibleArgKeys(stats)).toEqual([
            "filter_method",
            "filter_window_length",
            "filter_polyorder",
            "alpha",
        ]);
        expect(filtered?.baseAlgorithmName).toBe("Base");
        expect(filtered?.filteredMethodName).toBe("Savitzky-Golay: window_length=7");
    });

    it("computes one-sided share for each algorithm", () => {
        const experiment: Experiment = {
            id: "exp-one-sided",
            seriesList: [
                {
                    id: "s-1",
                    name: "S1",
                    precision: "double",
                    args: { x: 1 },
                    limit: { re: 1, im: 0 },
                    computed: [
                        { n: 1, value: { re: 0.8, im: 0 }, deviation: 2e-1 },
                        { n: 2, value: { re: 0.95, im: 0 }, deviation: 5e-2 },
                    ],
                },
            ],
            accelList: [
                { id: "a-1", name: "OneSided", m: 2, args: { alpha: 1 } },
                { id: "a-2", name: "TwoSided", m: 2, args: { alpha: 2 } },
            ],
            seriesAccelList: [
                {
                    series_id: "s-1",
                    accel_id: "a-1",
                    computed: [
                        { n: 1, value: { re: 0.7, im: 0 }, deviation: 3e-1 },
                        { n: 2, value: { re: 0.9, im: 0 }, deviation: 1e-1 },
                    ],
                    errors: [],
                    events: [],
                },
                {
                    series_id: "s-1",
                    accel_id: "a-2",
                    computed: [
                        { n: 1, value: { re: 0.7, im: 0 }, deviation: 3e-1 },
                        { n: 2, value: { re: 1.1, im: 0 }, deviation: 1e-1 },
                    ],
                    errors: [],
                    events: [],
                },
            ],
        };

        const stats = buildAlgoStatsFromExperiment(experiment, 1e-4, null);

        expect(stats.find((item) => item.algorithmName === "OneSided")?.oneSidedShare).toBe(1);
        expect(stats.find((item) => item.algorithmName === "TwoSided")?.oneSidedShare).toBe(0);
    });

    it("computes amp at the first n where the algorithm reaches its minimum", () => {
        const experiment: Experiment = {
            id: "exp-amp-at-min-n",
            seriesList: [
                {
                    id: "s-1",
                    name: "S1",
                    precision: "double",
                    args: { x: 1 },
                    limit: { re: 1, im: 0 },
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-1 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-4 },
                        { n: 3, value: { re: 0, im: 0 }, deviation: 1e-8 },
                    ],
                },
            ],
            accelList: [{ id: "a-1", name: "Amp", m: 2, args: { alpha: 1 } }],
            seriesAccelList: [
                {
                    series_id: "s-1",
                    accel_id: "a-1",
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-2 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-6 },
                        { n: 3, value: { re: 0, im: 0 }, deviation: 1e-6 },
                    ],
                    errors: [],
                    events: [],
                },
            ],
        };

        const stats = buildAlgoStatsFromExperiment(experiment, 1e-4, null);
        const amp = stats[0];

        expect(amp?.avgOrdersGain).toBeCloseTo(-2);
        expect(amp?.avgAmpAtMinN).toBeCloseTo(2);
        expect(amp?.avgMinDeviationN).toBe(2);
    });

    it("uses how_much(n_min) when comparing series deviation at algorithm minimum", () => {
        const experiment: Experiment = {
            id: "exp-how-much-at-min-n",
            seriesList: [
                {
                    id: "s-1",
                    name: "S1",
                    precision: "double",
                    args: { x: 1 },
                    limit: { re: 1, im: 0 },
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-1 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-2 },
                        { n: 3, value: { re: 0, im: 0 }, deviation: 1e-3 },
                        { n: 4, value: { re: 0, im: 0 }, deviation: 1e-4 },
                    ],
                },
            ],
            accelList: [{ id: "a-1", name: "Shanks Algorithm", m: 1, args: null }],
            seriesAccelList: [
                {
                    series_id: "s-1",
                    accel_id: "a-1",
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-6 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-3 },
                    ],
                    errors: [],
                    events: [],
                },
            ],
        };

        const stats = buildAlgoStatsFromExperiment(experiment, 1e-7, null);
        const shanks = stats[0];

        expect(computeHowMuch({ name: "Shanks Algorithm", m: 1, args: null }, 1)).toBe(4);
        expect(shanks?.avgAmpAtMinN).toBeCloseTo(2);
    });

    it("computes epsilon saved steps with how_much and ignores unreached epsilon cases", () => {
        const experiment: Experiment = {
            id: "exp-eps-saved-steps",
            seriesList: [
                {
                    id: "s-1",
                    name: "S1",
                    precision: "double",
                    args: { x: 1 },
                    limit: { re: 1, im: 0 },
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-1 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-2 },
                        { n: 3, value: { re: 0, im: 0 }, deviation: 1e-3 },
                        { n: 4, value: { re: 0, im: 0 }, deviation: 1e-4 },
                        { n: 5, value: { re: 0, im: 0 }, deviation: 2e-5 },
                        { n: 6, value: { re: 0, im: 0 }, deviation: 1e-5 },
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
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-2 },
                        { n: 3, value: { re: 0, im: 0 }, deviation: 1e-3 },
                        { n: 4, value: { re: 0, im: 0 }, deviation: 2e-5 },
                        { n: 5, value: { re: 0, im: 0 }, deviation: 1e-5 },
                    ],
                },
            ],
            accelList: [
                { id: "a-shanks", name: "Shanks Algorithm", m: 1, args: null },
                { id: "a-noreach", name: "NoReach", m: null, args: null },
            ],
            seriesAccelList: [
                {
                    series_id: "s-1",
                    accel_id: "a-shanks",
                    computed: [{ n: 1, value: { re: 0, im: 0 }, deviation: 1e-5 }],
                    errors: [],
                    events: [],
                },
                {
                    series_id: "s-2",
                    accel_id: "a-shanks",
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-4 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 1e-5 },
                    ],
                    errors: [],
                    events: [],
                },
                {
                    series_id: "s-1",
                    accel_id: "a-noreach",
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-4 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 2e-5 },
                    ],
                    errors: [],
                    events: [],
                },
                {
                    series_id: "s-2",
                    accel_id: "a-noreach",
                    computed: [
                        { n: 1, value: { re: 0, im: 0 }, deviation: 1e-4 },
                        { n: 2, value: { re: 0, im: 0 }, deviation: 2e-5 },
                    ],
                    errors: [],
                    events: [],
                },
            ],
        };

        const stats = buildAlgoStatsFromExperiment(experiment, 1e-5, null);
        const shanks = stats.find((item) => item.algorithmName === "Shanks Algorithm");
        const noReach = stats.find((item) => item.algorithmName === "NoReach");

        expect(shanks?.avgEpsSavedSteps).toBe(1);
        expect(shanks?.medianEpsSavedSteps).toBe(1);
        expect(shanks?.worstEpsSavedSteps).toBe(0);
        expect(noReach?.avgEpsSavedSteps).toBe(Number.NEGATIVE_INFINITY);
        expect(noReach?.medianEpsSavedSteps).toBe(Number.NEGATIVE_INFINITY);
        expect(noReach?.worstEpsSavedSteps).toBe(Number.NEGATIVE_INFINITY);
    });
});
