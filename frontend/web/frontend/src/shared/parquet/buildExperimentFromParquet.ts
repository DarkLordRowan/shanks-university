// src/shared/parquet/buildExperimentFromParquet.ts

import type {
    Accel,
    Complex,
    Experiment,
    ScalarArg,
    Series,
    SeriesAccel,
    SeriesAccelComputedPoint,
    SeriesAccelError,
    SeriesAccelEvent,
} from "@/entities/experiment/model/experiment";

import type {
    ParquetAccelComputed,
    ParquetAccelRow,
    ParquetComplex,
    ParquetErrorRow,
    ParquetEventRow,
    ParquetScalar,
    ParquetSeriesRow,
} from "./types";

import type { SeriesComputedPoint } from "@/entities/experiment/model/experiment";

interface ArrowListVectorLike<T> {
    get(index: number): T;
    _offsets: number[];
}

function isArrowListVectorLike<T = unknown>(v: unknown): v is ArrowListVectorLike<T> {
    if (typeof v !== "object" || v === null) return false;
    const obj = v as Record<string, unknown>;
    return typeof obj.get === "function" && Array.isArray(obj._offsets);
}

/**
 * КРИТИЧНО: fallback обязателен.
 * Если Arrow отдаёт не Array и не ListVectorLike (а object/scalar),
 * старое поведение должно сохранять элемент, а не терять его.
 */
function listLikeToArray<T = unknown>(v: unknown): T[] {
    if (v == null) return [];

    if (Array.isArray(v)) {
        return v as T[];
    }

    if (isArrowListVectorLike<T>(v)) {
        const offsets = v._offsets;
        // В parquet-wasm+arrow-js часто ListVectorLike в row-режиме представляет "один list на строку".
        // Берём первый диапазон offsets[0..1].
        const start = offsets[0] ?? 0;
        const end = offsets[1] ?? start;
        const length = end - start;

        const result: T[] = [];
        for (let i = 0; i < length; i++) {
            result.push(v.get(i));
        }
        return result;
    }

    if (typeof v === "object") {
        return [v as T];
    }

    return [v as T];
}

function toNumberOrNull(v: unknown): number | null {
    if (typeof v === "number") return Number.isNaN(v) ? null : v;
    if (typeof v === "bigint") return Number(v);

    if (typeof v === "string") {
        const s = v.trim();
        if (s === "") return null;

        const low = s.toLowerCase();
        if (low === "nan") return NaN;
        if (low === "+inf" || low === "inf" || low === "infinity") return Infinity;
        if (low === "-inf" || low === "-infinity") return -Infinity;

        const n = Number(s);
        return Number.isNaN(n) ? null : n;
    }

    return null;
}

function toComplex(src: ParquetComplex | null | undefined): Complex | null {
    if (!src) return null;

    const re = toNumberOrNull(src.real);
    const im = toNumberOrNull(src.imag);

    if (re == null && im == null) return null;

    return { re: re ?? 0, im: im ?? 0 };
}

function normalizeArgs(src: unknown): Record<string, ScalarArg> | null {
    if (src == null) return null;
    if (typeof src !== "object") return null;

    const obj = src as Record<string, unknown>;
    const keys = Object.keys(obj);
    if (keys.length === 0) return null;

    const res: Record<string, ScalarArg> = {};
    for (const k of keys) {
        const v = obj[k];
        if (v == null) continue;

        if (typeof v === "string" || typeof v === "number" || typeof v === "boolean") {
            res[k] = v;
            continue;
        }

        if (typeof v === "bigint") {
            res[k] = Number(v);
            continue;
        }

        res[k] = String(v);
    }

    return Object.keys(res).length === 0 ? null : res;
}

function normalizeSeriesArgs(src: ParquetSeriesRow["arguments"]): Record<string, ScalarArg> | null {
    return normalizeArgs(src);
}

function normalizeAccelArgs(
    src: ParquetAccelRow["additional_args"]
): Record<string, ScalarArg> | null {
    return normalizeArgs(src);
}

function mapAccelComputed(raw: unknown): SeriesAccelComputedPoint[] {
    const arr = listLikeToArray<ParquetAccelComputed | null>(raw);
    if (arr.length === 0) return [];

    return arr.map<SeriesAccelComputedPoint>((c, idx) => {
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

type RawPoint = { n: number; value: ParquetComplex | null };

function mapSeriesComputed(raw: unknown): SeriesComputedPoint[] {
    const arr = listLikeToArray<RawPoint>(raw);
    if (arr.length === 0) return [];

    return arr.map((p, idx) => {
        const n = typeof p?.n === "number" ? p.n : idx + 1;

        if (!p || p.value == null) {
            return { n, value: null };
        }

        return {
            n,
            value: {
                re: toNumberOrNull(p.value.real),
                im: toNumberOrNull(p.value.imag),
            },
        };
    });
}

function mapErrors(raw: unknown): SeriesAccelError[] {
    const arr = listLikeToArray<ParquetErrorRow>(raw);
    if (arr.length === 0) return [];

    return arr.map<SeriesAccelError>((e) => ({
        n: toNumberOrNull(e.n) ?? 0,
        message: typeof e.message === "string" ? e.message : String(e.message),
    }));
}

function mapEvents(raw: unknown): SeriesAccelEvent[] {
    const arr = listLikeToArray<ParquetEventRow>(raw);
    if (arr.length === 0) return [];

    return arr.map<SeriesAccelEvent>((ev, idx) => ({
        n: toNumberOrNull(ev.n) ?? idx,
        name: typeof ev.name === "string" ? ev.name : "",
        description:
            typeof ev.description === "string" ? ev.description : String(ev.description ?? ""),
    }));
}

const ORDERED_ACCEL_ARG_KEYS = [
    "m",
    "beta",
    "gamma",
    "parameter",
    "numerator",
    "rho",
    "epsilon_threshold",
    "useRecurrentFormula",
    "remainder",
] as const;

function extractMValue(row: ParquetAccelRow): number | null {
    return toNumberOrNull(row.m_value);
}

function buildAccelId(row: ParquetAccelRow): string {
    const parts: string[] = [];
    parts.push(`name=${row.accel_name}`);

    const m = extractMValue(row);
    if (m !== null) parts.push(`m=${m}`);

    const args = row.additional_args ?? {};
    const presentKeys = Object.keys(args);
    const present = new Set(presentKeys);

    for (const k of ORDERED_ACCEL_ARG_KEYS) {
        if (!present.has(k)) continue;
        const v = (args as any)[k];
        if (v == null) continue;
        parts.push(`${k}=${String(v)}`);
    }

    for (const k of presentKeys.sort()) {
        if ((ORDERED_ACCEL_ARG_KEYS as readonly string[]).includes(k)) continue;
        const v = (args as any)[k];
        if (v == null) continue;
        parts.push(`${k}=${String(v)}`);
    }

    return parts.join("|");
}

function buildSeriesList(seriesRows: ParquetSeriesRow[]): Series[] {
    const map = new Map<number, Series>();

    for (const r of seriesRows) {
        const sid = toNumberOrNull(r.series_id);
        if (sid == null) continue;

        const series: Series = {
            id: String(sid),
            name: r.series_name,
            precision: r.precision,
            args: normalizeSeriesArgs(r.arguments),
            limit: toComplex((r as any).series_limit),
            computed: (r as any).computed,
        };

        map.set(sid, series);
    }

    return [...map.values()].sort((a, b) => Number(a.id) - Number(b.id));
}

const PROGRESS_CHUNK = 2500;

export async function buildExperimentFromParquet(
    seriesRows: ParquetSeriesRow[],
    accelRows: ParquetAccelRow[],
    onProgress?: (processed: number, total: number) => void
): Promise<Experiment> {
    const id: string = "0";

    const seriesList = buildSeriesList(seriesRows);

    const seriesMap = new Map<number, Series>();
    for (const s of seriesList) {
        const n = toNumberOrNull(s.id);
        if (n != null) seriesMap.set(n, s);
    }

    const accelMap = new Map<string, Accel>();
    const seriesAccelList: SeriesAccel[] = [];

    const total = accelRows.length;
    let processed = 0;

    for (const row of accelRows) {
        processed += 1;

        const sid = toNumberOrNull(row.series_id);
        if (sid == null) continue;

        const series = seriesMap.get(sid);
        if (!series) continue;

        const accelId = buildAccelId(row);

        let accel = accelMap.get(accelId);
        if (!accel) {
            accel = {
                id: accelId,
                name: row.accel_name,
                m: extractMValue(row),
                args: normalizeAccelArgs(row.additional_args),
            };
            accelMap.set(accelId, accel);
        }

        const computed = mapAccelComputed(row.computed);
        const errors = mapErrors(row.errors);
        const events = mapEvents(row.events);

        seriesAccelList.push({
            series_id: series.id,
            accel_id: accel.id,
            computed,
            errors,
            events,
        });

        if (onProgress && (processed % PROGRESS_CHUNK === 0 || processed === total)) {
            onProgress(processed, total);
            await new Promise<void>((resolve) => requestAnimationFrame(() => resolve()));
        }
    }

    const accelList = [...accelMap.values()];

    return { id, seriesList, accelList, seriesAccelList };
}

// ------------------------------
// Инкрементальная сборка (без больших промежуточных массивов).
// ------------------------------

export function buildSeriesEntityFromParquetRow(row: ParquetSeriesRow): Series | null {
    const sid = toNumberOrNull(row.series_id);
    if (sid == null) return null;

    return {
        id: String(sid),
        name: row.series_name,
        precision: row.precision,
        args: normalizeSeriesArgs(row.arguments),
        limit: toComplex((row as any).series_limit),
        computed: mapSeriesComputed((row as any).computed),
    };
}

export function buildAccelAndSeriesAccelEntitiesFromParquetRow(params: {
    row: ParquetAccelRow;
    series: Series;
}): { accelId: string; accel: Accel; seriesAccel: SeriesAccel } {
    const { row, series } = params;

    const accelId = buildAccelId(row);

    const accel: Accel = {
        id: accelId,
        name: row.accel_name,
        m: extractMValue(row),
        args: normalizeAccelArgs(row.additional_args),
    };

    const seriesAccel: SeriesAccel = {
        series_id: series.id,
        accel_id: accel.id,
        computed: mapAccelComputed(row.computed),
        errors: mapErrors(row.errors),
        events: mapEvents(row.events),
    };

    return { accelId, accel, seriesAccel };
}
