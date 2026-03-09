import { useEffect, useState } from "react";
import type {
    Accel,
    AccelArgs,
    Experiment,
    Series,
    SeriesAccel,
    SeriesArgs,
} from "@/entities/experiment/model/experiment";
import { runChunkedWithProgress, type ChunkProgressState } from "@/shared/lib/chunkProgress";
import { buildSeriesAccelPairKey } from "@/shared/lib/experimentIndex";
import {
    buildMatrixAlgoInfos,
    buildMatrixSeriesInfos,
    type MatrixAlgoInfo,
    type MatrixSeriesInfo,
} from "@/shared/lib/matrixMeta";

export interface AlgorithmSeriesAnalyzeCellContext {
    sa: SeriesAccel;
    series: Series | null;
    accel: Accel | null;
}

export interface AlgorithmSeriesBuildMatrixContext<TCell> {
    seriesInfos: MatrixSeriesInfo[];
    algoInfos: MatrixAlgoInfo[];
    cells: Record<string, TCell>;
}

interface UseAlgorithmSeriesChunkedMatrixOptions<TCell, TMatrix> {
    experiment: Experiment | null;
    chunkSize: number;
    clearMatrixOnStart?: boolean;
    parseSeriesX: (args: SeriesArgs | null) => { xLabel: string; xSort: number | null };
    buildAlgoArgsSummary: (args: AccelArgs | null) => string;
    analyzeCell: (ctx: AlgorithmSeriesAnalyzeCellContext) => TCell;
    buildMatrix: (ctx: AlgorithmSeriesBuildMatrixContext<TCell>) => TMatrix;
}

interface UseAlgorithmSeriesChunkedMatrixResult<TMatrix> {
    matrix: TMatrix | null;
    progress: ChunkProgressState;
}

export function useAlgorithmSeriesChunkedMatrix<TCell, TMatrix>(
    options: UseAlgorithmSeriesChunkedMatrixOptions<TCell, TMatrix>
): UseAlgorithmSeriesChunkedMatrixResult<TMatrix> {
    const {
        experiment,
        chunkSize,
        clearMatrixOnStart = false,
        parseSeriesX,
        buildAlgoArgsSummary,
        analyzeCell,
        buildMatrix,
    } = options;

    const [matrix, setMatrix] = useState<TMatrix | null>(null);
    const [progress, setProgress] = useState<ChunkProgressState>({
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
        const cells: Record<string, TCell> = {};

        const total = seriesAccelList.length;
        setProgress({ running: true, current: 0, total });
        if (clearMatrixOnStart) {
            setMatrix(null);
        }

        runChunkedWithProgress({
            total,
            chunkSize,
            isCancelled: () => cancelled,
            onRange: (startIndex, endIndex) => {
                for (let i = startIndex; i < endIndex; i++) {
                    const sa = seriesAccelList[i];

                    usedSeriesIds.add(sa.series_id);
                    usedAccelIds.add(sa.accel_id);

                    const series = seriesMap.get(sa.series_id) ?? null;
                    const accel = accelMap.get(sa.accel_id) ?? null;

                    const key = buildSeriesAccelPairKey(sa.accel_id, sa.series_id);
                    cells[key] = analyzeCell({ sa, series, accel });
                }
            },
            onProgress: setProgress,
            onDone: () => {
                const seriesInfos = buildMatrixSeriesInfos(seriesListRaw, usedSeriesIds, parseSeriesX);
                const algoInfos = buildMatrixAlgoInfos(
                    accelListRaw,
                    usedAccelIds,
                    buildAlgoArgsSummary
                );

                if (!cancelled) {
                    setMatrix(
                        buildMatrix({
                            seriesInfos,
                            algoInfos,
                            cells,
                        })
                    );
                }
            },
        });

        return () => {
            cancelled = true;
        };
    }, [
        experiment,
        chunkSize,
        clearMatrixOnStart,
        parseSeriesX,
        buildAlgoArgsSummary,
        analyzeCell,
        buildMatrix,
    ]);

    return { matrix, progress };
}
