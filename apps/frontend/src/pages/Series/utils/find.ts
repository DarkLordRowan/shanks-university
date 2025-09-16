import type { SeriesNode } from "../../../data/series";

export function findSeriesById(list: SeriesNode[], id: string) {
    return list.find((s) => s.id === id);
}
