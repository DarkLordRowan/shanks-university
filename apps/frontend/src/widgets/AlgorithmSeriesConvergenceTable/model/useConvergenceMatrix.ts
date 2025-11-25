import { useEffect, useState } from "react";
import {
    type Experiment,
    type Series,
    type Accel,
    type SeriesInfo,
    type AlgoInfo,
    type ConvergenceMatrix,
    type ProgressState,
} from "./types";
import { analyzeSeriesAccelConvergence, buildArgsSummary, parseX } from "./convergenceUtils";

const CHUNK_SIZE = 24;

export function useConvergenceMatrix(
    experiment: Experiment | null,
    chunkSize: number = CHUNK_SIZE
): { matrix: ConvergenceMatrix | null; progress: ProgressState } {
    const [matrix, setMatrix] = useState<ConvergenceMatrix | null>(null);
    const [progress, setProgress] = useState<ProgressState>({
        running: false,
        current: 0,
        total: 0,
    });

    useEffect(() => {
        let cancelled = false;

        if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
            setMatrix(null);
            setProgress({ running: false, current: 0, total: 0 });
            return;
        }

        const seriesListRaw = experiment.seriesList ?? [];
        const accelListRaw = experiment.accelList ?? [];
        const seriesAccelList = experiment.seriesAccelList;

        const seriesMap = new Map<string, Series>();
        for (const s of seriesListRaw) {
            seriesMap.set(s.id, s);
        }

        const accelMap = new Map<string, Accel>();
        for (const a of accelListRaw) {
            accelMap.set(a.id, a);
        }

        const usedSeriesIds = new Set<string>();
        const usedAccelIds = new Set<string>();

        const cells: Record<string, ConvergenceMatrix["cells"][string]> = {};

        const total = seriesAccelList.length;
        setProgress({ running: true, current: 0, total });
        setMatrix(null);

        function processChunk(startIndex: number) {
            if (cancelled) return;

            const endIndex = Math.min(startIndex + chunkSize, total);

            for (let i = startIndex; i < endIndex; i++) {
                const sa = seriesAccelList[i];

                usedSeriesIds.add(sa.series_id);
                usedAccelIds.add(sa.accel_id);

                const series = seriesMap.get(sa.series_id);
                const accel = accelMap.get(sa.accel_id);

                const analysis = analyzeSeriesAccelConvergence(series, accel, sa);
                const key = `${sa.accel_id}::${sa.series_id}`;
                cells[key] = analysis;
            }

            const current = endIndex;
            const running = endIndex < total;
            setProgress({ running, current, total });

            if (running) {
                setTimeout(() => processChunk(endIndex), 0);
            } else {
                const seriesInfos: SeriesInfo[] = seriesListRaw
                    .filter((s) => usedSeriesIds.has(s.id))
                    .map((s) => {
                        const { xLabel, xSort } = parseX(s.args ?? null);
                        return {
                            key: s.id,
                            seriesName: s.name,
                            xLabel,
                            xSort,
                            precision: s.precision,
                        };
                    })
                    .sort((a, b) => {
                        const byName = a.seriesName.localeCompare(b.seriesName);
                        if (byName !== 0) return byName;

                        const ax = a.xSort;
                        const bx = b.xSort;
                        if (ax != null && bx != null) return ax - bx;
                        if (ax != null) return -1;
                        if (bx != null) return 1;
                        return a.xLabel.localeCompare(b.xLabel);
                    });

                const algoInfos: AlgoInfo[] = accelListRaw
                    .filter((a) => usedAccelIds.has(a.id))
                    .map((a) => {
                        const args = a.args ?? null;
                        return {
                            key: a.id,
                            algorithmName: a.name,
                            m: a.m,
                            argsSummary: buildArgsSummary(args),
                            algorithmArgs: args,
                        };
                    })
                    .sort((a, b) => {
                        const byName = a.algorithmName.localeCompare(b.algorithmName);
                        if (byName !== 0) return byName;
                        return (a.m ?? 0) - (b.m ?? 0);
                    });

                if (!cancelled) {
                    setMatrix({
                        seriesList: seriesInfos,
                        algoList: algoInfos,
                        cells,
                    });
                }
            }
        }

        processChunk(0);

        return () => {
            cancelled = true;
        };
    }, [experiment, chunkSize]);

    return { matrix, progress };
}
