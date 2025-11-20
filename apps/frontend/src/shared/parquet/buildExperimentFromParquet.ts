// buildExperimentFromParquet.ts

import type {
    Experiment,
    Series,
    Accel,
    SeriesAccel,
    Complex,
    SeriesArgs,
    AccelArgs,
    SeriesAccelComputedPoint,
    SeriesAccelError,
    SeriesAccelEvent,
} from "./experiment";

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
        [key: string]: unknown;
    } | null;

    series_limit: ParquetComplex | null;

    computed: ParquetSeriesComputed[]; // по факту может быть чем угодно
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

    computed: (ParquetAccelComputed | null)[]; // по факту тоже может быть чем угодно
    errors: ParquetErrorRow[] | null;
    events: ParquetEventRow[] | null;
}

/* helpers */

function parseNumberOrNull(src: string | null | undefined): number | null {
    if (src == null) return null;
    const v = Number(src);
    return Number.isFinite(v) ? v : null;
}

function fromParquetComplex(c: ParquetComplex | null): Complex | null {
    if (!c) return null;
    return {
        re: parseNumberOrNull(c.real),
        im: parseNumberOrNull(c.imag),
    };
}

function fromParquetSeriesArgs(src: ParquetSeriesRow["arguments"]): SeriesArgs | null {
    if (!src) return null;
    const result: SeriesArgs = {};
    for (const [k, v] of Object.entries(src)) {
        if (v === undefined) continue;
        result[k] = v as string | null;
    }
    return result;
}

function fromParquetAccelArgs(src: ParquetAccelRow["additional_args"]): AccelArgs | null {
    if (!src) return null;
    const result: AccelArgs = {};
    for (const [k, v] of Object.entries(src)) {
        if (v === undefined) continue;
        result[k] = v as string | null;
    }
    return result;
}

/**
 * Безопасное приведение "чего угодно" к массиву.
 * Поддерживаем:
 *   - уже массив
 *   - null/undefined → []
 *   - "объект с числовыми ключами" → Object.values(...)
 */
function asArray<T = unknown>(value: unknown): T[] {
    if (Array.isArray(value)) return value as T[];
    if (value == null) return [];
    if (typeof value === "object") {
        return Object.values(value as Record<string, T>);
    }
    return [];
}

/**
 * accel_id определяется по имени и всем аргументам (включая m_value).
 */
function buildAccelId(accel: ParquetAccelRow): string {
    const parts: string[] = [];

    parts.push(`name=${accel.accel_name}`);

    if (accel.m_value != null) {
        parts.push(`m=${accel.m_value}`);
    }

    if (accel.additional_args) {
        const entries = Object.entries(accel.additional_args)
            .filter(([_, v]) => v !== null && v !== undefined)
            .sort(([k1], [k2]) => k1.localeCompare(k2));

        for (const [k, v] of entries) {
            parts.push(`${k}=${String(v)}`);
        }
    }

    return parts.join("|");
}

function mapSeriesComputedPoints(row: ParquetSeriesRow): SeriesAccelComputedPoint[] {
    const raw = (row as any).computed;
    const list = asArray<ParquetSeriesComputed>(raw);

    return list.map<SeriesAccelComputedPoint>((p) => ({
        n: p.n,
        value: fromParquetComplex(p.value),
        // deviation для исходного ряда не определена
    }));
}

function mapAccelComputedPoints(row: ParquetAccelRow): SeriesAccelComputedPoint[] {
    const raw = (row as any).computed;
    const list = asArray<ParquetAccelComputed | null>(raw);

    return list.map<SeriesAccelComputedPoint>((p, idx) => ({
        n: idx, // в ParquetAccelComputed нет n, используем индекс
        value: p ? fromParquetComplex(p.value) : null,
        deviation: p ? parseNumberOrNull(p.deviation) : null,
    }));
}

function mapErrors(errs: ParquetErrorRow[] | null): SeriesAccelError[] {
    const list = asArray<ParquetErrorRow>(errs);
    return list.map<SeriesAccelError>((e) => ({
        n: e.n,
        message: e.message,
    }));
}

function mapEvents(events: ParquetEventRow[] | null): SeriesAccelEvent[] {
    const list = asArray<ParquetEventRow>(events);
    return list.map<SeriesAccelEvent>((e) => ({
        n: e.n,
        name: e.name,
        description: e.description,
    }));
}

/* main */

export function buildExperimentFromParquet(
    seriesRows: ParquetSeriesRow[],
    accelRows: ParquetAccelRow[],
): Experiment {
    const seriesList: Series[] = [];
    const accelListMap = new Map<string, Accel>();
    const seriesAccelList: SeriesAccel[] = [];

    const seriesById = new Map<number, ParquetSeriesRow>();

    // серии
    for (const s of seriesRows) {
        seriesById.set(s.series_id, s);

        const series: Series = {
            id: String(s.series_id),
            name: s.series_name,
            precision: s.precision,
            args: fromParquetSeriesArgs(s.arguments),
            limit: fromParquetComplex(s.series_limit),
        };

        seriesList.push(series);
    }

    // алгоритмы + связки series-accel
    for (const a of accelRows) {
        const seriesRow = seriesById.get(a.series_id);
        if (!seriesRow) {
            // нет ряда для данного series_id — игнорируем эту строку
            continue;
        }

        const accelId = buildAccelId(a);
        const accelArgs = fromParquetAccelArgs(a.additional_args);

        // регистрируем алгоритм, если ещё не был добавлен
        if (!accelListMap.has(accelId)) {
            const accel: Accel = {
                id: accelId,
                name: a.accel_name,
                m: a.m_value,
                args: accelArgs,
            };
            accelListMap.set(accelId, accel);
        }

        const seriesPoints = mapSeriesComputedPoints(seriesRow);
        const accelPoints = mapAccelComputedPoints(a);

        const seriesAccel: SeriesAccel = {
            series_id: String(a.series_id),
            accel_id: accelId,
            computed: [...seriesPoints, ...accelPoints],
            errors: mapErrors(a.errors),
            events: mapEvents(a.events),
        };

        seriesAccelList.push(seriesAccel);
    }

    const accelList: Accel[] = Array.from(accelListMap.values());

    const experiment: Experiment = {
        seriesList,
        accelList,
        seriesAccelList,
    };

    return experiment;
}
