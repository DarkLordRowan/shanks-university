import type {
    Experiment,
    Series,
    Accel,
    SeriesAccel,
    Complex,
    SeriesArgs,
    AccelArgs,
} from "@/entities/experiment/model/experiment";

export type SideType = "one_sided" | "two_sided" | "unknown" | "no_limit";

export type MonotonicityType =
    | "strict_decreasing_error"
    | "non_increasing_error"
    | "constant_error"
    | "has_growth"
    | "not_enough_data"
    | "no_limit";

export interface ConvergenceAnalysis {
    seriesId: string;
    accelId: string;

    side: SideType;
    monotonicity: MonotonicityType;

    signChangesCount: number;
    firstSignChangeN: number | null;
    firstGrowthN: number | null;

    /** Число шагов, на которых |Aₙ − lim| выросло относительно предыдущего */
    growthViolationsCount: number;

    /** Число проанализированных пар (n-1, n), где обе ошибки корректны */
    stepsAnalyzed: number;

    /** Шаги, на которых изменился знак ошибки */
    signChangeNs: number[];

    /** Шаги, на которых ошибка по норме выросла */
    growthNs: number[];
}

export type SeriesKey = string;
export type AlgoKey = string;

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

export interface ConvergenceMatrix {
    seriesList: SeriesInfo[];
    algoList: AlgoInfo[];
    cells: Record<string, ConvergenceAnalysis>;
}

export interface ProgressState {
    running: boolean;
    current: number;
    total: number;
}

export interface DetailPoint {
    n: number;
    valueRe: number | null;
    valueIm: number | null;
    err: number | null;
    sign: -1 | 0 | 1 | null;
    diffRe: number | null;
    diffIm: number | null;
    diffNorm: number | null;
}

export interface SelectedDetail {
    seriesInfo: SeriesInfo | null;
    algoInfo: AlgoInfo | null;
    series: Series | null;
    accel: Accel | null;
    analysis: ConvergenceAnalysis | null;
    limit: Complex | null;
    points: DetailPoint[];
}

export interface SelectedCell {
    seriesId: string;
    accelId: string;
}

export type { Experiment, Series, Accel, SeriesAccel, Complex, SeriesArgs, AccelArgs };
