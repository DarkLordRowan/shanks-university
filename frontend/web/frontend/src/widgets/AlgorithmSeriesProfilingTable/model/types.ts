import type {
    Experiment,
    Series,
    Accel,
    SeriesAccel,
    Profiling,
    SeriesArgs,
    AccelArgs,
} from "@/entities/experiment/model/experiment";

export type SeriesKey = string;
export type AlgoKey = string;

export interface SelectedCell {
    seriesId: SeriesKey;
    accelId: AlgoKey;
}

export type OpsTrendType =
    | "unknown"
    | "constant"
    | "increasing"
    | "non_decreasing"
    | "decreasing_error";

export interface ProfilingPoint {
    n: number;

    totalOps: number | null;
    opsByKind: Record<string, number | null>;

    diffTotalOps: number | null;
    diffByKind: Record<string, number | null>;
}

export interface ProfilingAnalysis {
    seriesId: SeriesKey;
    accelId: AlgoKey;

    stepsAnalyzed: number;

    trendTotal: OpsTrendType;
    maxTotalOps: number | null;
    maxTotalOpsN: number | null;
    decreaseNsTotal: number[];

    maxByKind: Record<string, { max: number; n: number }>;
    decreaseNsByKind: Record<string, number[]>;
    topKinds: string[];
}

export interface SeriesInfo {
    key: SeriesKey;
    seriesName: string;
    xLabel: string;
    xSort: number | null;
    precision: string;
}

export interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: AccelArgs | null;
}

export interface ProfilingMatrix {
    seriesList: SeriesInfo[];
    algoList: AlgoInfo[];
    cells: Record<string, ProfilingAnalysis>;
}

export interface ProgressState {
    running: boolean;
    current: number;
    total: number;
}

export interface SelectedDetail {
    seriesInfo: SeriesInfo | null;
    algoInfo: AlgoInfo | null;

    series: Series | null;
    accel: Accel | null;

    analysis: ProfilingAnalysis | null;
    points: ProfilingPoint[];
}

export type { Experiment, Series, Accel, SeriesAccel, Profiling, SeriesArgs, AccelArgs };
