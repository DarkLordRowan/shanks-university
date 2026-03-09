import type { Accel, Experiment, Series, SeriesAccel } from "@/entities/experiment/model/experiment";

export interface ExperimentIndex {
    seriesById: Map<string, Series>;
    accelById: Map<string, Accel>;
    seriesAccelByPair: Map<string, SeriesAccel>;
    getSeriesAccel: (seriesId: string, accelId: string) => SeriesAccel | null;
}

export function buildSeriesAccelPairKey(accelId: string, seriesId: string): string {
    return accelId + "::" + seriesId;
}

export function buildExperimentIndex(experiment: Experiment | null | undefined): ExperimentIndex {
    const seriesById = new Map<string, Series>();
    for (const series of experiment?.seriesList ?? []) {
        seriesById.set(series.id, series);
    }

    const accelById = new Map<string, Accel>();
    for (const accel of experiment?.accelList ?? []) {
        accelById.set(accel.id, accel);
    }

    const seriesAccelByPair = new Map<string, SeriesAccel>();
    for (const sa of experiment?.seriesAccelList ?? []) {
        seriesAccelByPair.set(buildSeriesAccelPairKey(sa.accel_id, sa.series_id), sa);
    }

    return {
        seriesById,
        accelById,
        seriesAccelByPair,
        getSeriesAccel: (seriesId: string, accelId: string) => {
            return seriesAccelByPair.get(buildSeriesAccelPairKey(accelId, seriesId)) ?? null;
        },
    };
}
