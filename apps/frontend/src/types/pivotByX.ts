import type { Item } from "./item";

export interface PivotByX {
    x: number;
    objects: Item[];
    allSeries: string[];
    allAlgorithms: string[];
}