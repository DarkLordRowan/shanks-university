import { useEffect, useState } from "react";
import type { Experiment, Series } from "@/entities/experiment/model/experiment";
import type { ProgressState, SeriesComputedConvergenceAnalysis } from "./types";
import { analyzeSeriesComputedConvergence } from "./seriesComputedConvergenceUtils";
import { runChunkedWithProgress } from "@/shared/lib/chunkProgress";

const DEFAULT_CHUNK = 24;

export function useSeriesComputedConvergence(
    experiment: Experiment | null,
    chunkSize: number = DEFAULT_CHUNK
): {
    seriesList: Series[];
    analysisBySeriesId: Record<string, SeriesComputedConvergenceAnalysis>;
    progress: ProgressState;
} {
    const [seriesList, setSeriesList] = useState<Series[]>([]);
    const [analysisBySeriesId, setAnalysisBySeriesId] = useState<
        Record<string, SeriesComputedConvergenceAnalysis>
    >({});
    const [progress, setProgress] = useState<ProgressState>({
        running: false,
        current: 0,
        total: 0,
    });

    useEffect(() => {
        let cancelled = false;

        if (!experiment) {
            setSeriesList([]);
            setAnalysisBySeriesId({});
            setProgress({ running: false, current: 0, total: 0 });
            return;
        }

        const list = (experiment.seriesList ?? []).filter((s) => (s.computed ?? []).length > 0);

        setSeriesList(list);
        setAnalysisBySeriesId({});
        setProgress({ running: true, current: 0, total: list.length });

        const total = list.length;
        const acc: Record<string, SeriesComputedConvergenceAnalysis> = {};

        runChunkedWithProgress({
            total,
            chunkSize,
            isCancelled: () => cancelled,
            onRange: (start, end) => {
                for (let i = start; i < end; i++) {
                    const s = list[i];
                    acc[s.id] = analyzeSeriesComputedConvergence(s);
                }
            },
            onProgress: setProgress,
            onDone: () => {
                if (!cancelled) {
                    setAnalysisBySeriesId({ ...acc });
                }
            },
        });

        return () => {
            cancelled = true;
        };
    }, [experiment, chunkSize]);

    return { seriesList, analysisBySeriesId, progress };
}
