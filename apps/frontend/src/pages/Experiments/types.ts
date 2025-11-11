export type ApiJsonResult = ResultBlock[];

export interface ResultBlock {
    series: SeriesDef;
    accel: AccelDef;
    computed: ComputedItem[];
    error: CalcError | null;
    id?: string;
}

export interface SeriesDef {
    name: string;
    lim: number;
    arguments: Record<string, number | string | boolean | null>;
}

export interface AccelDef {
    name: string;
    m_value?: number;
    additional_args?: Record<string, string>;
}

export interface ComputedItem {
    n: number;
    series_value: number;
    partial_sum: number;
    /** |S_n - L| */
    partial_sum_deviation: number;
    accel_value: number;
    /** |A_n - L| */
    accel_value_deviation: number;
}

export interface CalcError {
    description: string;
    data?: Record<string, unknown>;
}

export type ErrorFilterMode = "all" | "onlyGood" | "onlyBad";

export type XY = { x: number; y: number };

export type ChartGroup = {
    id: string;
    caption: string;
    limit?: number | null;
    partial: XY[];
    accel: XY[];
    errPartial: XY[]; // |S_n - L|
    errAccel: XY[];   // |A_n - L|
    ratio: XY[];      // |S_n - L| / |A_n - L|
};
