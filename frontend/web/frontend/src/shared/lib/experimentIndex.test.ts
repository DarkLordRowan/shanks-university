import { describe, expect, it } from "vitest";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { buildExperimentIndex, buildSeriesAccelPairKey } from "./experimentIndex";

const sampleExperiment: Experiment = {
    id: "exp-1",
    seriesList: [
        {
            id: "s-1",
            name: "S1",
            precision: "double",
            args: { x: 1 },
            limit: { re: 1, im: 0 },
        },
    ],
    accelList: [
        {
            id: "a-1",
            name: "A1",
            m: 2,
            args: { alpha: 0.1 },
        },
    ],
    seriesAccelList: [
        {
            series_id: "s-1",
            accel_id: "a-1",
            computed: [],
            errors: [],
            events: [],
        },
    ],
};

describe("experimentIndex", () => {
    it("builds stable pair keys", () => {
        expect(buildSeriesAccelPairKey("a", "s")).toBe("a::s");
    });

    it("creates lookup maps and resolves series-accel pairs", () => {
        const index = buildExperimentIndex(sampleExperiment);

        expect(index.seriesById.get("s-1")?.name).toBe("S1");
        expect(index.accelById.get("a-1")?.name).toBe("A1");
        expect(index.getSeriesAccel("s-1", "a-1")?.series_id).toBe("s-1");
        expect(index.getSeriesAccel("missing", "a-1")).toBeNull();
        expect(index.seriesAccelByPair.size).toBe(1);
    });

    it("returns empty maps for null experiment", () => {
        const index = buildExperimentIndex(null);

        expect(index.seriesById.size).toBe(0);
        expect(index.accelById.size).toBe(0);
        expect(index.seriesAccelByPair.size).toBe(0);
        expect(index.getSeriesAccel("s", "a")).toBeNull();
    });
});
