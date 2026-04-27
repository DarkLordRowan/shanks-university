import { describe, expect, it } from "vitest";
import {
    cloneMatrixAlgoSeriesFilterState,
    createMatrixAlgoSeriesFilterState,
} from "./MatrixAlgoSeriesFilter";

describe("MatrixAlgoSeriesFilter state helpers", () => {
    it("creates independent sets and arg clauses from initial state", () => {
        const accelGroups = new Set(["group-a"]);
        const accelIds = new Set(["accel-1"]);
        const seriesGroups = new Set(["series-a"]);
        const seriesIds = new Set(["series-1"]);
        const precisions = new Set(["f64"]);
        const accelClauses = [{ key: "k", value: "1" }];
        const seriesClauses = [{ key: "x", value: "2" }];

        const state = createMatrixAlgoSeriesFilterState({
            accel: {
                query: "algo",
                groupMode: "blacklist",
                selectedGroupKeys: accelGroups,
                idMode: "blacklist",
                selectedIds: accelIds,
                mMinText: "1",
                mMaxText: "10",
                argsOp: "or",
                argClauses: accelClauses,
            },
            series: {
                query: "series",
                groupMode: "blacklist",
                selectedGroupKeys: seriesGroups,
                idMode: "blacklist",
                selectedIds: seriesIds,
                precisionMode: "blacklist",
                selectedPrecisions: precisions,
                argsOp: "or",
                argClauses: seriesClauses,
            },
        });

        accelGroups.add("group-b");
        accelIds.add("accel-2");
        seriesGroups.add("series-b");
        seriesIds.add("series-2");
        precisions.add("f32");
        accelClauses[0]!.value = "mutated";
        seriesClauses[0]!.value = "mutated";

        expect(Array.from(state.accel.selectedGroupKeys)).toEqual(["group-a"]);
        expect(Array.from(state.accel.selectedIds)).toEqual(["accel-1"]);
        expect(Array.from(state.series.selectedGroupKeys)).toEqual(["series-a"]);
        expect(Array.from(state.series.selectedIds)).toEqual(["series-1"]);
        expect(Array.from(state.series.selectedPrecisions)).toEqual(["f64"]);
        expect(state.accel.argClauses).toEqual([{ key: "k", value: "1" }]);
        expect(state.series.argClauses).toEqual([{ key: "x", value: "2" }]);
    });

    it("clones filter state without sharing mutable references", () => {
        const original = createMatrixAlgoSeriesFilterState({
            accel: {
                selectedGroupKeys: new Set(["group-a"]),
                selectedIds: new Set(["accel-1"]),
                argClauses: [{ key: "k", value: "1" }],
            },
            series: {
                selectedGroupKeys: new Set(["series-a"]),
                selectedIds: new Set(["series-1"]),
                selectedPrecisions: new Set(["f64"]),
                argClauses: [{ key: "x", value: "2" }],
            },
        });

        const cloned = cloneMatrixAlgoSeriesFilterState(original);

        cloned.accel.selectedGroupKeys.add("group-b");
        cloned.accel.selectedIds.add("accel-2");
        cloned.series.selectedPrecisions.add("f32");
        cloned.accel.argClauses[0]!.value = "changed";
        cloned.series.argClauses.push({ key: "y", value: "3" });

        expect(Array.from(original.accel.selectedGroupKeys)).toEqual(["group-a"]);
        expect(Array.from(original.accel.selectedIds)).toEqual(["accel-1"]);
        expect(Array.from(original.series.selectedPrecisions)).toEqual(["f64"]);
        expect(original.accel.argClauses).toEqual([{ key: "k", value: "1" }]);
        expect(original.series.argClauses).toEqual([{ key: "x", value: "2" }]);
    });
});
