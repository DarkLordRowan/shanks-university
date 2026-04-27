import { afterEach, describe, expect, it } from "vitest";
import {
    buildExperimentSessionStateKey,
    clearInMemorySessionState,
    readInMemorySessionState,
    resetInMemorySessionStateForTests,
    writeInMemorySessionState,
} from "./inMemorySessionState";

describe("inMemorySessionState", () => {
    afterEach(() => {
        resetInMemorySessionStateForTests();
    });

    it("stores isolated values by key", () => {
        writeInMemorySessionState("k1", { value: 1 });
        writeInMemorySessionState("k2", { value: 2 });

        expect(readInMemorySessionState<{ value: number }>("k1")).toEqual({ value: 1 });
        expect(readInMemorySessionState<{ value: number }>("k2")).toEqual({ value: 2 });
    });

    it("clears individual keys", () => {
        writeInMemorySessionState("k1", { value: 1 });
        writeInMemorySessionState("k2", { value: 2 });

        clearInMemorySessionState("k1");

        expect(readInMemorySessionState("k1")).toBeUndefined();
        expect(readInMemorySessionState("k2")).toEqual({ value: 2 });
    });

    it("clones values on write and read when clone is provided", () => {
        const clone = (value: { items: string[] }) => ({ items: [...value.items] });
        const source = { items: ["a"] };

        writeInMemorySessionState("k1", source, clone);
        source.items.push("b");

        const restored = readInMemorySessionState<{ items: string[] }>("k1", clone);
        expect(restored).toEqual({ items: ["a"] });

        restored?.items.push("c");

        expect(readInMemorySessionState<{ items: string[] }>("k1", clone)).toEqual({
            items: ["a"],
        });
    });

    it("builds experiment-scoped keys with namespace and version", () => {
        expect(buildExperimentSessionStateKey(17, "matrix-filters")).toBe(
            "exp-session:v1:17:matrix-filters"
        );
        expect(buildExperimentSessionStateKey("exp-42", "view:error-matrix", "v2")).toBe(
            "exp-session:v2:exp-42:view:error-matrix"
        );
    });
});
