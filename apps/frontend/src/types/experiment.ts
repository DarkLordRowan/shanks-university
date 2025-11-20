// experiment.ts

export interface Complex {
    re: number | null;
    im: number | null;
}

export type ScalarArg = number | string | boolean | null;

export interface SeriesArgs {
    [key: string]: ScalarArg;
}

export interface AccelArgs {
    [key: string]: ScalarArg;
}

export interface Series {
    id: string;
    name: string;
    precision: string;
    args: SeriesArgs | null;
    limit: Complex | null;
}

export interface Accel {
    id: string;
    name: string;
    m: number | null;
    args: AccelArgs | null;
}

export interface SeriesAccelComputedPoint {
    n: number;
    value: { re: number | null; im: number | null } | null;
    deviation?: number | null;
}

export interface SeriesAccelError {
    n: number;
    message: string;
}

export interface SeriesAccelEvent {
    n: number;
    name: string;
    description: string;
}

export interface SeriesAccel {
    series_id: string;
    accel_id: string;

    computed: SeriesAccelComputedPoint[];
    errors: SeriesAccelError[];
    events: SeriesAccelEvent[];
}

export interface Experiment {
    seriesList?: Series[];
    accelList?: Accel[];
    seriesAccelList?: SeriesAccel[];
}
