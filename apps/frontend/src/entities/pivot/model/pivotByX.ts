import type { Item } from "@/entities/item/model/item.ts";

export interface PivotByX {
    x: number;
    objects: Item[];
    allSeries: string[];
    allAlgorithms: string[];
}
