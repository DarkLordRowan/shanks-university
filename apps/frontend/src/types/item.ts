export interface ResponseComputed {
    n: number;
    series_value: number;
    partial_sum: number;
    partial_sum_deviation: number;
    accel_value: number;
    accel_value_deviation: number;
}

export interface ResponseError {
    description: string;
    data: { n: number | null; }
}

export interface Item {
    id: string;
    series: {
        x: number;
        seriesName: string;
        seriesLim: number | null;
        seriesArgs: { x: number };
    };
    algorithm: {
        algorithmName: string;
        m: number | null;
        algorithmArgs: {
            remainder: string;
            useRecFormulas: string;
            beta: string;
        } | null;
    };
    computed: ResponseComputed[];
    error?: ResponseError | null;
}