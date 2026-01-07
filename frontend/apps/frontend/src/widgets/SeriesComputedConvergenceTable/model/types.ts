import type {
    Experiment,
    Series,
    Complex,
    SeriesArgs,
} from "@/entities/experiment/model/experiment";

export type SideType = "one_sided" | "two_sided" | "unknown";

export type MonotonicityType =
    | "strict_decreasing_error"
    | "non_increasing_error"
    | "constant_error"
    | "strict_increasing_error"
    | "non_decreasing_error"
    | "random_error"
    | "unknown";

export interface ProgressState {
    running: boolean;
    current: number;
    total: number;
}

export interface DetailPoint {
    n: number;

    valueRe: number | null;
    valueIm: number | null;

    // e_n = ||S_n - S||
    err: number | null;

    // sgn(Re(S_n - S))
    sign: -1 | 0 | 1 | null;

    // Δ_n = S_n - S_{n-1}
    diffRe: number | null;
    diffIm: number | null;
    diffNorm: number | null;
}

export interface SeriesComputedConvergenceAnalysis {
    seriesId: string;

    sideRaw: SideType;
    monotonicityRaw: MonotonicityType;

    stepsAnalyzed: number;

    // sign changes of Re(S_n - S)
    signChangesCount: number;
    signChangeNs: number[];

    // counts for error monotonicity
    incCount: number;
    decCount: number;
    eqCount: number;

    // n where e_n > e_{n-1}
    violationsNs: number[];
}

export type { Experiment, Series, Complex, SeriesArgs };
