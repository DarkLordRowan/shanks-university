import { useEffect, useState } from "react";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { runChunkedWithProgress, type ChunkProgressState } from "@/shared/lib/chunkProgress";
import {
    createAlgoRankingStatsAccumulator,
    type AlgoStats,
} from "./AlgoRankingTable.model";

const DEFAULT_CHUNK_SIZE = 24;

export interface UseAlgoRankingStatsOptions {
    experiment: Experiment | null;
    epsilon: number;
    precisionFilter: string | null;
    allowedSeriesIds?: Set<string> | null;
    allowedAccelIds?: Set<string> | null;
    chunkSize?: number;
}

export function useAlgoRankingStats(options: UseAlgoRankingStatsOptions): {
    stats: AlgoStats[] | null;
    progress: ChunkProgressState;
} {
    const {
        experiment,
        epsilon,
        precisionFilter,
        allowedSeriesIds,
        allowedAccelIds,
        chunkSize = DEFAULT_CHUNK_SIZE,
    } = options;
    const [stats, setStats] = useState<AlgoStats[] | null>(null);
    const [progress, setProgress] = useState<ChunkProgressState>({
        running: false,
        current: 0,
        total: 0,
    });

    useEffect(() => {
        let cancelled = false;
        const seriesAccelList = experiment?.seriesAccelList ?? [];

        if (!experiment || seriesAccelList.length === 0) {
            setStats([]);
            setProgress({ running: false, current: 0, total: 0 });
            return;
        }

        const accumulator = createAlgoRankingStatsAccumulator({
            experiment,
            epsilon,
            precisionFilter,
            allowedSeriesIds,
            allowedAccelIds,
        });
        const total = seriesAccelList.length;

        setStats(null);
        setProgress({ running: true, current: 0, total });

        runChunkedWithProgress({
            total,
            chunkSize,
            isCancelled: () => cancelled,
            onRange: (startIndex, endIndex) => {
                for (let i = startIndex; i < endIndex; i++) {
                    accumulator.processSeriesAccel(seriesAccelList[i]);
                }
            },
            onProgress: (nextProgress) => {
                if (!cancelled) setProgress(nextProgress);
            },
            onDone: () => {
                if (cancelled) return;
                setStats(accumulator.finalize());
                setProgress({ running: false, current: total, total });
            },
        });

        return () => {
            cancelled = true;
        };
    }, [
        experiment,
        epsilon,
        precisionFilter,
        allowedSeriesIds,
        allowedAccelIds,
        chunkSize,
    ]);

    return { stats, progress };
}
