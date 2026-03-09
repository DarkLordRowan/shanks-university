import { describe, expect, it } from "vitest";
import { appendAlgorithmArgsTooltipLines } from "./matrixTooltip";

describe("appendAlgorithmArgsTooltipLines", () => {
    it("appends sorted args and summary", () => {
        const lines: string[] = [];

        const hasArgs = appendAlgorithmArgsTooltipLines(
            lines,
            {
                zeta: 2,
                alpha: 1,
                nil: null,
            },
            "alpha=1, zeta=2"
        );

        expect(hasArgs).toBe(true);
        expect(lines).toEqual([
            "  alpha = 1",
            "  zeta = 2",
            "  (alpha=1, zeta=2)",
        ]);
    });

    it("returns false when nothing to append", () => {
        const lines: string[] = [];

        const hasArgs = appendAlgorithmArgsTooltipLines(lines, null, "");

        expect(hasArgs).toBe(false);
        expect(lines).toEqual([]);
    });
});
