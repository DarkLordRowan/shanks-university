// src/shared/parquet/types.ts
//
// Типы входных строк Parquet.
// Arrow/wasm может возвращать сложные значения (ListVector/Struct и т.п.).
// Здесь оставлены поля, которые реально используются в сборке Experiment.

export type ParquetScalar = string | number | boolean | null;

export interface ParquetComplex {
    real: ParquetScalar;
    imag: ParquetScalar;
}

export interface ParquetAccelComputed {
    value: ParquetComplex | null;
    deviation: ParquetScalar;
}

export interface ParquetErrorRow {
    n: ParquetScalar;
    message: unknown;
}

export interface ParquetEventRow {
    n: ParquetScalar;
    name: unknown;
    description: unknown;
}

export interface ParquetSeriesRow {
    series_name: string;
    series_id: ParquetScalar;
    precision: string;

    arguments: Record<string, unknown> | null;
    series_limit: ParquetComplex | null;
    computed?: unknown;
}

export interface ParquetAccelRow {
    series_id: ParquetScalar;

    accel_name: string;
    m_value: ParquetScalar;

    additional_args: Record<string, unknown> | null;

    computed: unknown;
    errors: unknown;
    events: unknown;
}
