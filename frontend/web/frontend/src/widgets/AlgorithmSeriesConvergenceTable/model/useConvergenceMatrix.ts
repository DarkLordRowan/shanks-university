import type {
    ConvergenceMatrix,
    Experiment,
    ProgressState,
} from "./types";
import { analyzeSeriesAccelConvergence, buildArgsSummary, parseX } from "./convergenceUtils";
import {
    type AlgorithmSeriesAnalyzeCellContext,
    type AlgorithmSeriesBuildMatrixContext,
    useAlgorithmSeriesChunkedMatrix,
} from "@/shared/lib/useAlgorithmSeriesChunkedMatrix";

const CHUNK_SIZE = 24;

type ConvergenceCell = ConvergenceMatrix["cells"][string];

function analyzeConvergenceCell({ sa, series, accel }: AlgorithmSeriesAnalyzeCellContext): ConvergenceCell {
    return analyzeSeriesAccelConvergence(series, accel, sa);
}

function buildConvergenceMatrix({
    seriesInfos,
    algoInfos,
    cells,
}: AlgorithmSeriesBuildMatrixContext<ConvergenceCell>): ConvergenceMatrix {
    return {
        seriesList: seriesInfos,
        algoList: algoInfos,
        cells,
    };
}

export function useConvergenceMatrix(
    experiment: Experiment | null,
    chunkSize: number = CHUNK_SIZE
): { matrix: ConvergenceMatrix | null; progress: ProgressState } {
    return useAlgorithmSeriesChunkedMatrix<ConvergenceCell, ConvergenceMatrix>({
        experiment,
        chunkSize,
        clearMatrixOnStart: true,
        parseSeriesX: parseX,
        buildAlgoArgsSummary: buildArgsSummary,
        analyzeCell: analyzeConvergenceCell,
        buildMatrix: buildConvergenceMatrix,
    });
}
