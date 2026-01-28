// src/entities/experiment/model/experiment.ts

export interface Complex {
    re: number | null;
    im: number | null;
}

export interface Profiling {
    add: number;
    mul: number;
    div: number;
    special: number;
}

export type ScalarArg = number | string | boolean | null;

export interface SeriesArgs {
    [key: string]: ScalarArg;
}

export interface AccelArgs {
    [key: string]: ScalarArg;
}

export interface SeriesComputedPoint {
    n: number;
    value: Complex | null;
    deviation?: number | null;
    profiling?: Profiling | null;
}

export interface Series {
    id: string;
    name: string;
    precision: string;
    args: SeriesArgs | null;
    limit: Complex | null;

    computed?: SeriesComputedPoint[];
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
    profiling?: Profiling | null;
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

export interface SeriesAccelFilteredMethod {
    values: ({ re: number | null; im: number | null } | null)[];
    average: { re: number | null; im: number | null } | null;
}

export interface SeriesAccelFiltered {
    startN: number;
    segmentLength: number;
    methods: Record<string, SeriesAccelFilteredMethod>;
}

export interface SeriesAccel {
    series_id: string;
    accel_id: string;

    computed: SeriesAccelComputedPoint[];
    errors: SeriesAccelError[];
    events: SeriesAccelEvent[];

    noise?: string | null;
    filtered?: SeriesAccelFiltered | null;
}

export interface Experiment {
    id: string;
    seriesList: Series[];
    accelList: Accel[];
    seriesAccelList: SeriesAccel[];
}
