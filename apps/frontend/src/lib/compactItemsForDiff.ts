import type { Item } from "../types/item";
import type { DiffItem } from "../types/diffItem";
import { computeItemScore } from "./computeItemScore";

function buildSeriesKey(seriesName: string, x: number): string {
    return `${seriesName} | x=${x}`;
}

export function compactItemsForDiff(items: Item[]): DiffItem[] {
    return items.map<DiffItem>((it) => {
        const seriesKey = buildSeriesKey(it.series.seriesName, it.series.x);
        const algoKey = it.algorithm.algorithmId;

        const score = computeItemScore(it);
        const hasError = !!it.error;
        const errorN = it.error?.data?.n ?? null;
        const errorDescription = it.error?.description ?? null;

        return {
            seriesKey,
            seriesName: it.series.seriesName,
            x: it.series.x,

            algoKey,
            algorithmName: it.algorithm.algorithmName,
            m: it.algorithm.m,
            algorithmArgs: it.algorithm.algorithmArgs ?? null,

            score,
            hasError,
            errorN,
            errorDescription,
        };
    });
}
