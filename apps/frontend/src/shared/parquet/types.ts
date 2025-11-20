export interface ParquetComplex {
    real: string | null;
    imag: string | null;
}

export interface ParquetSeriesComputed {
    n: number;
    value: ParquetComplex | null;
}

export interface ParquetSeriesRow {
    series_name: string;
    series_id: number;
    precision: string;

    arguments: {
        x?: string | null;
        b?: string | null;
        m?: string | null;
        a?: string | null;
        // возможны дополнительные поля
        [key: string]: unknown;
    } | null;

    series_limit: ParquetComplex | null;

    computed: ParquetSeriesComputed[];
}

export interface ParquetAccelComputed {
    value: ParquetComplex | null;
    deviation: string | null;
}

export interface ParquetErrorRow {
    n: number;
    message: string;
}

export interface ParquetEventRow {
    n: number;
    name: string;
    description: string;
}

export interface ParquetAccelRow {
    series_id: number;
    accel_name: string;
    m_value: number | null;

    additional_args: {
        remainder?: string | null;
        useRecurrentFormula?: string | null;
        beta?: string | null;
        gamma?: string | null;
        parameter?: string | null;
        numerator?: string | null;
        rho?: string | null;
        epsilon_threshold?: string | null;
        [key: string]: unknown;
    } | null;

    computed: (ParquetAccelComputed | null)[];
    errors: ParquetErrorRow[] | null;
    events: ParquetEventRow[] | null;
}
