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
    ParquetSeriesRow,
} from "./types";

interface ArrowListVectorLike<T> {
    get(index: number): T;

    _offsets: number[];
}

function isArrowListVectorLike<T = unknown>(v: unknown): v is ArrowListVectorLike<T> {
    if (typeof v !== "object" || v === null) return false;

    const obj = v as Record<string, unknown>;
    const offsets = obj._offsets;

    return typeof obj.get === "function" && Array.isArray(offsets);
}

function listLikeToArray<T = unknown>(v: unknown): T[] {
    if (v == null) return [];

    if (Array.isArray(v)) {
        return v as T[];
    }

    if (isArrowListVectorLike<T>(v)) {
        const offsets = v._offsets;
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
    if (typeof v === "number") {
        return Number.isNaN(v) ? null : v;
    }

    if (typeof v === "bigint") {
        return Number(v);
    }

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

function toScalarArg(v: unknown): ScalarArg {
    if (v === null || v === undefined) return null;

    if (typeof v === "number") {
        return Number.isNaN(v) ? null : v;
    }

    if (typeof v === "boolean") {
        return v;
    }

    if (typeof v === "bigint") {
        return Number(v);
    }

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

function toComplex(c: ParquetComplex | null): Complex | null {
    if (!c) return null;

    const re = toNumberOrNull(c.real);
    const im = toNumberOrNull(c.imag);

    if (re === null && im === null) return null;
    return { re, im };
}

type ArgsSource = Record<string, unknown> | null | undefined;

function normalizeArgs(src: ArgsSource): Record<string, ScalarArg> | null {
    if (!src) return null;

    const out: Record<string, ScalarArg> = {};
    for (const [k, v] of Object.entries(src)) {
        out[k] = toScalarArg(v);
    }

    return Object.keys(out).length === 0 ? null : out;
}

function normalizeSeriesArgs(src: ParquetSeriesRow["arguments"]): Record<string, ScalarArg> | null {
    return normalizeArgs(src);
}

function normalizeAccelArgs(
    src: ParquetAccelRow["additional_args"]
): Record<string, ScalarArg> | null {
    return normalizeArgs(src);
}

function extractMValue(row: ParquetAccelRow): number | null {
    return toNumberOrNull(row.m_value);
}

const ORDERED_ACCEL_ARG_KEYS = [
    "beta",
    "gamma",
    "parameter",
    "numerator",
    "rho",
    "epsilon_threshold",
    "remainder",
    "useRecurrentFormula",
] as const;

function buildAccelId(row: ParquetAccelRow): string {
    const parts: string[] = [];

    parts.push(`name=${row.accel_name}`);

    const m = extractMValue(row);
    if (m !== null) {
        parts.push(`m=${m}`);
    }

    const args = row.additional_args ?? {};
    const presentKeys = Object.keys(args);
    const present = new Set(presentKeys);

    for (const k of ORDERED_ACCEL_ARG_KEYS) {
        if (present.has(k)) {
            const v = (args as Record<string, unknown>)[k];
            if (v != null) {
                parts.push(`${k}=${String(v)}`);
            }
        }
    }

    const other = presentKeys
        .filter(
            (k) => !ORDERED_ACCEL_ARG_KEYS.includes(k as (typeof ORDERED_ACCEL_ARG_KEYS)[number])
        )
        .sort();

    for (const k of other) {
        const v = (args as Record<string, unknown>)[k];
        if (v != null) {
            parts.push(`${k}=${String(v)}`);
        }
    }

    return parts.join("|");
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

function buildSeriesList(seriesRows: ParquetSeriesRow[]): Series[] {
    const map = new Map<number, Series>();

    for (const r of seriesRows) {
        const sid = toNumberOrNull(r.series_id);
        if (sid == null) continue;

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

const PROGRESS_CHUNK = 1000;

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
        if (n != null) {
            seriesMap.set(n, s);
        }
    }

    const accelMap = new Map<string, Accel>();
    const seriesAccelList: SeriesAccel[] = [];

    const total = accelRows.length;
    let processed = 0;

    for (const row of accelRows) {
        processed += 1;

        const sid = toNumberOrNull(row.series_id);
        if (sid == null) {
            continue;
        }
        const series = seriesMap.get(sid);
        if (!series) {
            continue;
        }

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
            await new Promise<void>((resolve) => {
                requestAnimationFrame(() => resolve());
            });
        }
    }

    const accelList = [...accelMap.values()];

    return {
        id,
        seriesList,
        accelList,
        seriesAccelList,
    };
}
