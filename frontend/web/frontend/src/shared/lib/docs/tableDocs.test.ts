import { describe, expect, it } from "vitest";
import {
    ALGO_RANKING_TABLE_DOCS,
    buildDocsHref,
    getAlgoRankingColumnAnchorId,
    getSeriesComputedColumnAnchorId,
    listAllDocsAnchorIds,
    SERIES_COMPUTED_TABLE_DOCS,
} from "./tableDocs";

describe("tableDocs", () => {
    it("keeps all anchor ids unique", () => {
        const ids = listAllDocsAnchorIds();
        expect(new Set(ids).size).toBe(ids.length);
    });

    it("builds canonical docs href", () => {
        expect(buildDocsHref("docs-algo-ranking")).toBe("/docs#docs-algo-ranking");
        expect(buildDocsHref("docs-series-computed-col-class")).toBe(
            "/docs#docs-series-computed-col-class"
        );
    });

    it("covers all algo ranking columns with anchors", () => {
        const expected = [
            "precision",
            "m",
            "arg1",
            "arg2",
            "arg3",
            "seriesCount",
            "avgBestDeviation",
            "avgRelativeError",
            "avgOrdersGain",
            "avgAmpAtMinN",
            "notBetterThanSeriesShare",
            "avgMinDeviationN",
            "avgLastMinusMin",
            "avgStepsToTol",
            "fracReachedTol",
            "oneSidedShare",
            "bestMinShare",
            "worstMinShare",
            "bestLastShare",
            "worstLastShare",
            "rankPrecision",
            "rankSpeed",
            "rankStability",
            "totalRankScore",
        ] as const;

        expect(ALGO_RANKING_TABLE_DOCS.columns.map((column) => column.key)).toEqual(expected);
        expect(getAlgoRankingColumnAnchorId("avgRelativeError")).toBe(
            "docs-algo-ranking-col-avgRelativeError"
        );
    });

    it("covers all partial sums columns with anchors", () => {
        const expected = [
            "name",
            "precision",
            "args",
            "class",
            "k",
            "sign",
            "viol",
            "devMin",
            "minN",
            "devLast",
            "lastN",
            "lastMinusMin",
            "ampOrders",
            "maxAmpOrders",
            "devMean",
            "devMedian",
            "devMax",
        ] as const;

        expect(SERIES_COMPUTED_TABLE_DOCS.columns.map((column) => column.key)).toEqual(expected);
        expect(getSeriesComputedColumnAnchorId("class")).toBe("docs-series-computed-col-class");
    });
});
