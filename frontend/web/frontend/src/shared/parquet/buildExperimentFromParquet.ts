// src/shared/parquet/buildExperimentFromParquet.ts

import type {
    Accel,
    Complex,
    Experiment,
    Profiling,
    ScalarArg,
    Series,
    SeriesAccel,
    SeriesAccelComputedPoint,
    SeriesAccelError,
    SeriesAccelEvent,
    SeriesAccelFiltered,
    SeriesAccelFilteredMethod,
} from "@/entities/experiment/model/experiment";

import type {
    ParquetAccelComputed,
    ParquetAccelRow,
    ParquetComplex,
    ParquetErrorRow,
    ParquetEventRow,
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

function toProfiling(src: unknown): Profiling | null {
    if (src == null || typeof src !== "object") return null;
    const o = src as Record<string, unknown>;

    const add = toNumberOrNull(o.add);
    const mul = toNumberOrNull(o.mul);
    const div = toNumberOrNull(o.div);
    const special = toNumberOrNull(o.special);

    if (add == null && mul == null && div == null && special == null) return null;

    return {
        add: add ?? 0,
        mul: mul ?? 0,
        div: div ?? 0,
        special: special ?? 0,
    };
}

function normalizeArgs(src: unknown): Record<string, ScalarArg> | null {
    if (src == null) return null;
    if (typeof src !== "object") return null;

    const obj = src as Record<string, unknown>;
    const keys = Object.keys(obj).filter((k) => k !== "__dummy__");
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
            return { n, value: null, deviation: null, profiling: null };
        }

        return {
            n,
            value: toComplex(c.value),
            deviation: toNumberOrNull(c.deviation),
            profiling: toProfiling(c.profiling),
        };
    });
}

type RawSeriesPoint = {
    n?: unknown;
    value?: ParquetComplex | null;
    deviation?: unknown;
    profiling?: unknown;
};

function mapSeriesComputed(raw: unknown): SeriesComputedPoint[] {
    const arr = listLikeToArray<RawSeriesPoint>(raw);
    if (arr.length === 0) return [];

    return arr.map((p, idx) => {
        const n = toNumberOrNull(p?.n) ?? idx + 1;

        const value = p?.value ? toComplex(p.value) : null;
        const deviation = toNumberOrNull(p?.deviation);
        const profiling = toProfiling(p?.profiling);

        return {
            n,
            value,
            deviation: deviation ?? null,
            profiling,
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
        name: typeof ev.name === "string" ? ev.name : String(ev.name ?? ""),
        description:
            typeof ev.description === "string" ? ev.description : String(ev.description ?? ""),
    }));
}

function mapFiltered(raw: unknown): SeriesAccelFiltered | null {
    if (raw == null || typeof raw !== "object") return null;
    const o = raw as Record<string, unknown>;

    const startN = toNumberOrNull(o.start_n) ?? 0;
    const segmentLength = toNumberOrNull(o.segment_length) ?? 0;

    const methodsRaw = o.methods;
    const methods: Record<string, SeriesAccelFilteredMethod> = {};

    if (methodsRaw && typeof methodsRaw === "object") {
        const mo = methodsRaw as Record<string, unknown>;
        for (const key of Object.keys(mo)) {
            if (key === "__dummy__") continue;

            const v = mo[key];
            if (v == null || typeof v !== "object") continue;

            const m = v as Record<string, unknown>;
            const valuesArr = listLikeToArray<ParquetComplex | null>(m.values);
            const values = valuesArr.map((x) => (x ? toComplex(x) : null));

            const avg = m.average as ParquetComplex | null | undefined;
            const average = avg ? toComplex(avg) : null;

            methods[key] = { values, average };
        }
    }

    return {
        startN,
        segmentLength,
        methods,
    };
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
    const presentKeys = Object.keys(args).filter((k) => k !== "__dummy__");
    const present = new Set(presentKeys);

    for (const k of ORDERED_ACCEL_ARG_KEYS) {
        if (!present.has(k)) continue;
        const v = args[k];
        if (v == null) continue;
        parts.push(`${k}=${String(v)}`);
    }

    for (const k of presentKeys.sort()) {
        if ((ORDERED_ACCEL_ARG_KEYS as readonly string[]).includes(k)) continue;
        const v = args[k];
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
            limit: toComplex(r.series_limit),
            computed: mapSeriesComputed(r.computed),
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

        const noise = typeof row.noise_str === "string" ? row.noise_str : null;
        const filtered = mapFiltered(row.filtered);

        seriesAccelList.push({
            series_id: series.id,
            accel_id: accel.id,
            computed,
            errors,
            events,
            noise,
            filtered,
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
        limit: toComplex(row.series_limit),
        computed: mapSeriesComputed(row.computed),
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
        noise: typeof row.noise_str === "string" ? row.noise_str : null,
        filtered: mapFiltered(row.filtered),
    };

    return { accelId, accel, seriesAccel };
}
