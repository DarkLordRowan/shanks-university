import type { Item } from "../types/item";

export type SeriesKey = string;
export type AlgoKey = string;

export interface DiffItem {
    seriesKey: SeriesKey;
    seriesName: string;
    x: number;

    algoKey: AlgoKey;
    algorithmName: string;
    m: number | null;
    algorithmArgs: Item["algorithm"]["algorithmArgs"] | null;

    /** score ∈ [0, 100], либо null, если не удалось посчитать */
    score: number | null;

    /** для тултипов (опционально, но полезно) */
    hasError: boolean;
    errorN: number | null;
    errorDescription: string | null;
}
