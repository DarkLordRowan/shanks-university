import type {
    Experiment,
    ProfilingAnalysis,
    ProfilingMatrix,
    ProgressState,
} from "./types";
import { analyzeSeriesAccelProfiling, buildArgsSummary, parseX } from "./profilingUtils";
import {
    type AlgorithmSeriesAnalyzeCellContext,
    type AlgorithmSeriesBuildMatrixContext,
    useAlgorithmSeriesChunkedMatrix,
} from "@/shared/lib/useAlgorithmSeriesChunkedMatrix";

const CHUNK_SIZE = 24;

type ProfilingCell = ProfilingAnalysis;

function analyzeProfilingCell({ sa }: AlgorithmSeriesAnalyzeCellContext): ProfilingCell {
    return analyzeSeriesAccelProfiling(sa);
}

function buildProfilingMatrix({
    seriesInfos,
    algoInfos,
    cells,
}: AlgorithmSeriesBuildMatrixContext<ProfilingCell>): ProfilingMatrix {
    return {
        seriesList: seriesInfos,
        algoList: algoInfos,
        cells,
    };
}

export function useProfilingMatrix(
    experiment: Experiment | null,
    chunkSize: number = CHUNK_SIZE
): { matrix: ProfilingMatrix | null; progress: ProgressState } {
    return useAlgorithmSeriesChunkedMatrix<ProfilingCell, ProfilingMatrix>({
        experiment,
        chunkSize,
        parseSeriesX: parseX,
        buildAlgoArgsSummary: buildArgsSummary,
        analyzeCell: analyzeProfilingCell,
        buildMatrix: buildProfilingMatrix,
    });
}
