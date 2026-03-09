import { afterEach, describe, expect, it, vi } from "vitest";
import { runChunkedWithProgress, type ChunkProgressState } from "./chunkProgress";

describe("runChunkedWithProgress", () => {
    afterEach(() => {
        vi.useRealTimers();
    });

    it("processes ranges in chunks and reports progress", () => {
        vi.useFakeTimers();

        const ranges: Array<[number, number]> = [];
        const progress: ChunkProgressState[] = [];
        let doneCount = 0;

        runChunkedWithProgress({
            total: 5,
            chunkSize: 2,
            isCancelled: () => false,
            onRange: (start, end) => {
                ranges.push([start, end]);
            },
            onProgress: (state) => {
                progress.push(state);
            },
            onDone: () => {
                doneCount += 1;
            },
        });

        expect(ranges).toEqual([[0, 2]]);
        expect(progress).toEqual([{ running: true, current: 2, total: 5 }]);
        expect(doneCount).toBe(0);

        vi.runAllTimers();

        expect(ranges).toEqual([
            [0, 2],
            [2, 4],
            [4, 5],
        ]);
        expect(progress).toEqual([
            { running: true, current: 2, total: 5 },
            { running: true, current: 4, total: 5 },
            { running: false, current: 5, total: 5 },
        ]);
        expect(doneCount).toBe(1);
    });

    it("stops processing when cancelled", () => {
        vi.useFakeTimers();

        let cancelled = false;
        const ranges: Array<[number, number]> = [];
        let doneCount = 0;

        runChunkedWithProgress({
            total: 5,
            chunkSize: 2,
            isCancelled: () => cancelled,
            onRange: (start, end) => {
                ranges.push([start, end]);
            },
            onProgress: (state) => {
                if (state.current >= 2) cancelled = true;
            },
            onDone: () => {
                doneCount += 1;
            },
        });

        vi.runAllTimers();

        expect(ranges).toEqual([[0, 2]]);
        expect(doneCount).toBe(0);
    });

    it("handles empty total without chunk callback", () => {
        const progress: ChunkProgressState[] = [];
        let rangeCalls = 0;
        let doneCount = 0;

        runChunkedWithProgress({
            total: 0,
            chunkSize: 10,
            isCancelled: () => false,
            onRange: () => {
                rangeCalls += 1;
            },
            onProgress: (state) => {
                progress.push(state);
            },
            onDone: () => {
                doneCount += 1;
            },
        });

        expect(rangeCalls).toBe(0);
        expect(progress).toEqual([{ running: false, current: 0, total: 0 }]);
        expect(doneCount).toBe(1);
    });
});
