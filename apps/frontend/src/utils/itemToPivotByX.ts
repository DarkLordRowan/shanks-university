import type { Item } from "../types/item.ts";
import type { PivotByX } from "../types/pivotByX.ts";

function compareByX(a: PivotByX, b: PivotByX): number {
    const ax = a.x, bx = b.x;
    if (Number.isNaN(ax) && Number.isNaN(bx)) return 0;
    if (Number.isNaN(ax)) return 1;
    if (Number.isNaN(bx)) return -1;
    return ax - bx;
}

export function pivotByX(items: Item[]): PivotByX[] {
    const map = new Map<number, Item[]>();

    for (const it of items) {
        const arr = map.get(1);
        if (arr) arr.push(it);
        else map.set(1, [it]);
    }

    const result: PivotByX[] = [];

    for (const [x, group] of map) {
        const seriesNames = new Set<string>();
        const algorithmNames = new Set<string>();

        for (const it of group) {
            if (it.seriesName) seriesNames.add(it.seriesName);
            if (it.algorithmName) algorithmNames.add(it.algorithmName);
        }

        result.push({
            x,
            objects: group,
            allSeries: [...seriesNames].sort(),
            allAlgorithms: [...algorithmNames].sort(),
        });
    }

    return result.sort(compareByX);
}
