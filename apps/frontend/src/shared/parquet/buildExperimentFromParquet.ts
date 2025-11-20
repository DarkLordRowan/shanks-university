// src/shared/parquet/buildExperimentFromParquet.ts

import type {
    Experiment,
    Series,
    Accel,
    SeriesAccel,
    Complex,
    ScalarArg,
    SeriesAccelComputedPoint,
    SeriesAccelError,
    SeriesAccelEvent,
} from "@/types/experiment";

import type {
    ParquetSeriesRow,
    ParquetAccelRow,
    ParquetComplex,
    ParquetAccelComputed,
    ParquetErrorRow,
    ParquetEventRow,
} from "./types";

/** Приведение строки/числа/bigint → number | null */
function toNumberOrNull(v: unknown): number | null {
    if (typeof v === "number") return Number.isNaN(v) ? null : v;
    if (typeof v === "bigint") return Number(v);
    if (typeof v === "string") {
        const s = v.trim();
        if (s === "") return null;
        const low = s.toLowerCase();
        if (low === "nan") return NaN;
        if (low === "inf" || low === "+inf" || low === "infinity") return Infinity;
        if (low === "-inf" || low === "-infinity") return -Infinity;
        const n = Number(s);
        return Number.isNaN(n) ? null : n;
    }
    return null;
}

/**
 * Приведение Arrow List / обычного массива / одиночного значения к массиву.
 *
 * Поддерживает:
 *  - JS-массив: [ ... ]
 *  - Arrow ListVector: { get(i), _offsets: [start, end], ... }
 *  - одиночный объект: { ... } → [ {...} ]
 *  - null/undefined → []
 */
function listLikeToArray<T = unknown>(v: unknown): T[] {
    if (v == null) return [];

    // 1) Обычный массив
    if (Array.isArray(v)) return v as T[];

    // 2) Arrow ListVector
    if (
        typeof v === "object" &&
        v !== null &&
        "get" in v &&
        typeof (v as any).get === "function" &&
        "_offsets" in v &&
        Array.isArray((v as any)._offsets)
    ) {
        const list = v as any;
        const offsets: number[] = list._offsets;
        const start = offsets[0] ?? 0;
        const end = offsets[1] ?? start;
        const length = end - start;

        const result: T[] = [];
        for (let i = 0; i < length; i++) {
            result.push(list.get(i) as T);
        }
        return result;
    }

    // 3) Структура / одиночный объект → массив из одного элемента
    if (typeof v === "object") {
        return [v as T];
    }

    // 4) Всё остальное — тоже одиночное значение → [value]
    return [v as T];
}

/** Преобразование комплекса */
function toComplex(c: ParquetComplex | null): Complex | null {
    if (!c) return null;
    const re = toNumberOrNull(c.real);
    const im = toNumberOrNull(c.imag);
    if (re === null && im === null) return null;
    return { re, im };
}

/** Скалярный аргумент */
function toScalarArg(v: unknown): ScalarArg {
    if (v === null || v === undefined) return null;

    if (typeof v === "number") return Number.isNaN(v) ? null : v;
    if (typeof v === "boolean") return v;
    if (typeof v === "bigint") return Number(v);

    if (typeof v === "string") {
        const s = v.trim();
        if (s === "") return null;
        const low = s.toLowerCase();
        if (low === "true") return true;
        if (low === "false") return false;
        const n = Number(s);
        if (!Number.isNaN(n)) return n;
        return s;
    }

    return null;
}

/** Series.args */
function normalizeSeriesArgs(src: ParquetSeriesRow["arguments"]): Record<string, ScalarArg> | null {
    if (!src) return null;
    const out: Record<string, ScalarArg> = {};
    for (const [k, v] of Object.entries(src)) {
        out[k] = toScalarArg(v);
    }
    return Object.keys(out).length === 0 ? null : out;
}

/** Accel.args */
function normalizeAccelArgs(src: ParquetAccelRow["additional_args"]): Record<string, ScalarArg> | null {
    if (!src) return null;
    const out: Record<string, ScalarArg> = {};
    for (const [k, v] of Object.entries(src)) {
        out[k] = toScalarArg(v);
    }
    return Object.keys(out).length === 0 ? null : out;
}

/** Построение детерминированного accel_id */
function buildAccelId(row: ParquetAccelRow): string {
    const parts: string[] = [];

    parts.push(`name=${row.accel_name}`);

    const m = toNumberOrNull((row as any).m_value ?? row.m_value);
    if (m !== null) {
        parts.push(`m=${m}`);
    }

    const args = row.additional_args ?? {};
    const orderedKeys = [
        "beta",
        "gamma",
        "parameter",
        "numerator",
        "rho",
        "epsilon_threshold",
        "remainder",
        "useRecurrentFormula",
    ];

    const present = new Set(Object.keys(args));

    for (const k of orderedKeys) {
        if (present.has(k)) {
            const v = (args as any)[k];
            if (v != null) parts.push(`${k}=${String(v)}`);
        }
    }

    const other = [...present].filter((k) => !orderedKeys.includes(k)).sort();
    for (const k of other) {
        const v = (args as any)[k];
        if (v != null) parts.push(`${k}=${String(v)}`);
    }

    return parts.join("|");
}

/** computed */
function mapAccelComputed(raw: unknown): SeriesAccelComputedPoint[] {
    const arr = listLikeToArray<ParquetAccelComputed | null>(raw);
    if (arr.length === 0) return [];

    return arr.map((c, idx) => {
        const n = idx + 1;

        if (c == null) {
            return { n, value: null, deviation: null };
        }
        return {
            n,
            value: toComplex(c.value),
            deviation: toNumberOrNull(c.deviation),
        };
    });
}


/** errors */
function mapErrors(raw: unknown): SeriesAccelError[] {
    const arr = listLikeToArray<ParquetErrorRow>(raw);
    if (arr.length === 0) return [];
    return arr.map((e) => ({
        n: toNumberOrNull(e.n) ?? 0,
        message: typeof e.message === "string" ? e.message : String(e.message),
    }));
}

/** events */
function mapEvents(raw: unknown): SeriesAccelEvent[] {
    const arr = listLikeToArray<ParquetEventRow>(raw);
    if (arr.length === 0) return [];
    return arr.map((ev, idx) => ({
        n: toNumberOrNull(ev.n) ?? idx,
        name: typeof ev.name === "string" ? ev.name : "",
        description: typeof ev.description === "string" ? ev.description : "",
    }));
}

/** Series list */
function buildSeriesList(seriesRows: ParquetSeriesRow[]): Series[] {
    const map = new Map<number, Series>();

    for (const r of seriesRows) {
        const sid = toNumberOrNull(r.series_id) ?? -1;
        const id = String(sid);

        const series: Series = {
            id,
            name: r.series_name,
            precision: r.precision,
            args: normalizeSeriesArgs(r.arguments),
            limit: toComplex(r.series_limit),
        };

        map.set(sid, series);
    }

    return [...map.values()].sort((a, b) => Number(a.id) - Number(b.id));
}

/** Основная сборка Experiment */
export function buildExperimentFromParquet(
    seriesRows: ParquetSeriesRow[],
    accelRows: ParquetAccelRow[],
): Experiment {
    const seriesList = buildSeriesList(seriesRows);

    const seriesMap = new Map<number, Series>();
    for (const s of seriesList) {
        const n = toNumberOrNull(s.id);
        if (n != null) seriesMap.set(n, s);
    }

    const accelMap = new Map<string, Accel>();
    const seriesAccelList: SeriesAccel[] = [];

    let count = 0;

    for (const row of accelRows) {
        if (count % 1000 == 0) {
            console.log("buildExperimentFromParquet", count)
        }
        count = count + 1;

        const sid = toNumberOrNull(row.series_id) ?? -1;
        const series = seriesMap.get(sid);
        if (!series) continue;

        const accelId = buildAccelId(row);

        let accel = accelMap.get(accelId);
        if (!accel) {
            accel = {
                id: accelId,
                name: row.accel_name,
                m: toNumberOrNull((row as any).m_value ?? row.m_value),
                args: normalizeAccelArgs(row.additional_args),
            };
            accelMap.set(accelId, accel);
        }

        const computed = mapAccelComputed((row as any).computed);
        const errors = mapErrors((row as any).errors);
        const events = mapEvents((row as any).events);

        seriesAccelList.push({
            series_id: series.id,
            accel_id: accel.id,
            computed,
            errors,
            events,
        });
    }

    const accelList = [...accelMap.values()];

    return {
        seriesList,
        accelList,
        seriesAccelList,
    };
}
