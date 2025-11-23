import type { Item } from "@/entities/item/model/item";

export type SeriesKey = string;
export type AlgoKey = string;

export interface DiffCell {
    seriesKey: SeriesKey;
    seriesName: string;
    x: number;

    algoKey: AlgoKey;
    algorithmName: string;
    m: number | null;
    algorithmArgs: Item["algorithm"]["algorithmArgs"] | null;

    // JSON #1 (prev)
    scorePrev: number | null;
    hasErrorPrev: boolean;
    errorNPrev: number | null;
    errorDescriptionPrev: string | null;

    // JSON #2 (next)
    scoreNext: number | null;
    hasErrorNext: boolean;
    errorNNext: number | null;
    errorDescriptionNext: string | null;
}
