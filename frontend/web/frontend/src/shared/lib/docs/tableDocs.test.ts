import { describe, expect, it } from "vitest";
import {
    ALGO_SERIES_CONVERGENCE_TABLE_DOCS,
    ALGO_RANKING_TABLE_DOCS,
    buildDocsHref,
    getAlgoSeriesConvergenceColumnAnchorId,
    getAlgoRankingColumnAnchorId,
    getSeriesComputedColumnAnchorId,
    listAllDocsAnchorIds,
    SERIES_COMPUTED_TABLE_DOCS,
} from "./tableDocs";

describe("tableDocs", () => {
    const getSheetFieldNames = (
        table:
            | typeof ALGO_RANKING_TABLE_DOCS
            | typeof SERIES_COMPUTED_TABLE_DOCS
            | typeof ALGO_SERIES_CONVERGENCE_TABLE_DOCS,
        sheetName: string
    ) => table.xlsxSheets.find((sheet) => sheet.name === sheetName)!.fields.map((field) => field.name);

    it("keeps all anchor ids unique", () => {
        const ids = listAllDocsAnchorIds();
        expect(new Set(ids).size).toBe(ids.length);
    });

    it("builds canonical docs href", () => {
        expect(buildDocsHref("docs-algo-ranking")).toBe("/docs#docs-algo-ranking");
        expect(buildDocsHref("docs-series-computed-col-class")).toBe(
            "/docs#docs-series-computed-col-class"
        );
        expect(
            buildDocsHref("docs-algo-series-convergence-sheet-summary-field-seriesAlgoAmp")
        ).toBe("/docs#docs-algo-series-convergence-sheet-summary-field-seriesAlgoAmp");
    });

    it("covers all algo ranking columns with anchors", () => {
        const expected = [
            "precision",
            "m",
            "args",
            "howMuchFormula",
            "seriesCount",
            "avgBestDeviation",
            "medianBestDeviation",
            "worstBestDeviation",
            "avgRelativeError",
            "medianRelativeError",
            "worstRelativeError",
            "avgOrdersGain",
            "medianOrdersGain",
            "worstOrdersGain",
            "avgAmpAtMinN",
            "medianAmpAtMinN",
            "worstAmpAtMinN",
            "avgFilterTriggerN",
            "medianFilterTriggerN",
            "worstFilterTriggerN",
            "avgFilterTriggerDeltaFromMinN",
            "medianFilterTriggerDeltaFromMinN",
            "worstFilterTriggerDeltaFromMinN",
            "notBetterThanSeriesShare",
            "avgMinDeviationN",
            "medianMinDeviationN",
            "worstMinDeviationN",
            "avgLastMinusMin",
            "medianLastMinusMin",
            "worstLastMinusMin",
            "avgStepsToTol",
            "medianStepsToTol",
            "worstStepsToTol",
            "avgEpsSavedSteps",
            "avgDiffInStepsToEps",
            "medianEpsSavedSteps",
            "worstEpsSavedSteps",
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

    it("covers all algorithm × series screen elements with anchors", () => {
        const expected = [
            "axes",
            "cell",
            "thresholds",
            "class",
            "selectedDetail",
        ] as const;

        expect(ALGO_SERIES_CONVERGENCE_TABLE_DOCS.columns.map((column) => column.key)).toEqual(
            expected
        );
        expect(getAlgoSeriesConvergenceColumnAnchorId("class")).toBe(
            "docs-algo-series-convergence-col-class"
        );
    });

    it("documents all algo ranking xlsx headers", () => {
        expect(getSheetFieldNames(ALGO_RANKING_TABLE_DOCS, "overview")).toEqual([
            "field",
            "value",
            "epsilon",
            "epsilon exponent",
            "precision filter",
            "sort key",
            "sort dir",
            "rows",
            "columns",
        ]);

        expect(getSheetFieldNames(ALGO_RANKING_TABLE_DOCS, "algo_ranking")).toEqual([
            "place",
            "algorithm",
            "precision",
            "m",
            "args.<key>",
            "how_much formula",
            "series",
            "avg min |dev|",
            "med min |dev|",
            "worst min |dev|",
            "avg rel error",
            "med rel error",
            "worst rel error",
            "avg series/algo amp",
            "med series/algo amp",
            "worst series/algo amp",
            "avg series@min n/algo amp",
            "med series@min n/algo amp",
            "worst series@min n/algo amp",
            "avg filter n",
            "med filter n",
            "worst filter n",
            "avg filter-min n",
            "med filter-min n",
            "worst filter-min n",
            "min algo >= min series, %",
            "avg min dev n",
            "med min dev n",
            "worst min dev n",
            "avg last-min",
            "med last-min",
            "worst last-min",
            "avg steps to eps",
            "med steps to eps",
            "worst steps to eps",
            "avg eps saved steps",
            "AvgDiffInStepsToEps",
            "med eps saved steps",
            "worst eps saved steps",
            "reached eps, %",
            "1-sided, %",
            "best min div, %",
            "worst min div, %",
            "best last div, %",
            "worst last div, %",
            "rank precision",
            "rank speed",
            "rank stability",
            "total rank",
        ]);
    });

    it("documents all partial sums xlsx headers", () => {
        expect(getSheetFieldNames(SERIES_COMPUTED_TABLE_DOCS, "overview")).toEqual([
            "field",
            "value",
            "rows",
            "total rows before filter",
            "max sign changes",
            "max violations",
            "sort",
            "selected series",
        ]);

        expect(getSheetFieldNames(SERIES_COMPUTED_TABLE_DOCS, "summary")).toEqual([
            "#",
            "series",
            "precision",
            "args",
            "limit",
            "side",
            "class",
            "class title",
            "steps",
            "sign changes",
            "violations",
            "min |S_n-S|",
            "min n",
            "last |S_n-S|",
            "last n",
            "last - min",
            "last/min amp",
            "max/min amp",
            "mean |S_n-S|",
            "median |S_n-S|",
            "max |S_n-S|",
        ]);

        expect(getSheetFieldNames(SERIES_COMPUTED_TABLE_DOCS, "selected_meta")).toEqual([
            "field",
            "value",
            "series",
            "precision",
            "args",
            "limit",
            "side",
            "class",
            "class title",
            "class description",
            "steps",
            "sign changes",
            "violations",
            "min |S_n-S|",
            "min n",
            "last |S_n-S|",
            "last n",
            "last - min",
            "last/min amp",
            "max/min amp",
            "mean |S_n-S|",
            "median |S_n-S|",
            "max |S_n-S|",
        ]);

        expect(getSheetFieldNames(SERIES_COMPUTED_TABLE_DOCS, "selected_points")).toEqual([
            "n",
            "Re(S_n)",
            "Im(S_n)",
            "|S_n-S|",
            "sgn*|S_n-S|",
            "sgn(Re(S_n-S))",
        ]);

        expect(getSheetFieldNames(SERIES_COMPUTED_TABLE_DOCS, "selected_diffs")).toEqual([
            "n",
            "Re(S_n-S_{n-1})",
            "Im(S_n-S_{n-1})",
            "|S_n-S_{n-1}|",
            "sgn*|S_n-S_{n-1}|",
        ]);
    });

    it("documents all algorithm × series xlsx headers and matrix structure", () => {
        expect(getSheetFieldNames(ALGO_SERIES_CONVERGENCE_TABLE_DOCS, "overview")).toEqual([
            "field",
            "value",
            "algorithms",
            "series",
            "pairs",
            "max sign changes",
            "max violations",
            "selected cell",
        ]);

        expect(getSheetFieldNames(ALGO_SERIES_CONVERGENCE_TABLE_DOCS, "summary")).toEqual([
            "algorithm",
            "m",
            "args",
            "series",
            "x",
            "precision",
            "limit",
            "side",
            "class",
            "class title",
            "steps",
            "sign changes",
            "violations",
            "min |A_n-lim|",
            "min n",
            "last |A_n-lim|",
            "last n",
            "last - min",
            "last/min amp",
            "max/min amp",
            "series/algo amp",
        ]);

        expect(getSheetFieldNames(ALGO_SERIES_CONVERGENCE_TABLE_DOCS, "matrix")).toEqual([
            "algorithm \\ series",
            "algorithm row header",
            "series column header",
            "cell payload",
            "cell color",
        ]);

        expect(getSheetFieldNames(ALGO_SERIES_CONVERGENCE_TABLE_DOCS, "selected_meta")).toEqual([
            "field",
            "value",
            "algorithm",
            "m",
            "args",
            "series",
            "x",
            "precision",
            "limit",
            "side",
            "class",
            "class title",
            "class description",
            "steps",
            "sign changes",
            "violations",
            "min |A_n-lim|",
            "min n",
            "last |A_n-lim|",
            "last n",
            "last - min",
            "last/min amp",
            "max/min amp",
            "series/algo amp",
        ]);

        expect(getSheetFieldNames(ALGO_SERIES_CONVERGENCE_TABLE_DOCS, "selected_points")).toEqual([
            "n",
            "Re(A_n)",
            "Im(A_n)",
            "|A_n-lim|",
            "sgn*|A_n-lim|",
            "sgn(Re(A_n-lim))",
        ]);

        expect(getSheetFieldNames(ALGO_SERIES_CONVERGENCE_TABLE_DOCS, "selected_diffs")).toEqual([
            "n",
            "Re(A_n-A_{n-1})",
            "Im(A_n-A_{n-1})",
            "|A_n-A_{n-1}|",
        ]);
    });
});
