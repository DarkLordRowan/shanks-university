import type { SeriesNode } from "@/entities/experiment/model/seriesData";

export function findSeriesById(list: SeriesNode[], id: string) {
    return list.find((s) => s.id === id);
}
