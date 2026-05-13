import type { ResponseRecord } from "@/shared/api/experiments/dto";
import type { AlgorithmArgs, Item, ResponseComputed, ResponseError, } from "../entities/item/model/item";

type NumLike = number | string | null | undefined;

function toNumber(v: NumLike): number {
    if (v === null || v === undefined) return NaN;
    if (typeof v === "number") return v;

    const s = v.trim();

    if (s === "Infinity") return Infinity;
    if (s === "-Infinity") return -Infinity;
    if (s === "NaN" || s === "+NaN" || s === "-NaN") return NaN;

    const n = Number(s);
    return Number.isNaN(n) ? NaN : n;
}

function toNumberOrNull(v: unknown): number | null {
    if (v === null || v === undefined) return null;
    if (typeof v !== "number" && typeof v !== "string") return null;
    const n = toNumber(v);
    return Number.isNaN(n) ? null : n;
}

function firstNonEmptyString(...values: unknown[]): string {
    for (const value of values) {
        if (typeof value !== "string") continue;
        const text = value.trim();
        if (text) return text;
    }
    return "Unknown error";
}

function toItemComputed(raw: {
    n: NumLike;
    series_value: NumLike;
    partial_sum: NumLike;
    partial_sum_deviation: NumLike;
    accel_value: NumLike;
    accel_value_deviation: NumLike;
}): ResponseComputed {
    const nVal = toNumber(raw.n);

    return {
        n: Number.isNaN(nVal) ? NaN : nVal,
        series_value: toNumber(raw.series_value),
        partial_sum: toNumber(raw.partial_sum),
        partial_sum_deviation: toNumber(raw.partial_sum_deviation),
        accel_value: toNumber(raw.accel_value),
        accel_value_deviation: toNumber(raw.accel_value_deviation),
    };
}

function toItemError(raw: ResponseRecord["error"]): ResponseError | null {
    if (!raw) return null;

    const record = raw as Record<string, unknown>;
    const data = (raw.data ?? {}) as Record<string, unknown>;
    const n =
        toNumberOrNull(record.n) ??
        toNumberOrNull(data.n) ??
        toNumberOrNull(record.computed_index) ??
        toNumberOrNull(data.computed_index);

    return {
        description: firstNonEmptyString(
            record.message,
            record.description,
            data.message,
            data.description
        ),
        data: {
            n,
        },
    };
}

/** Превращает additional_args в словарь string→string, отбрасывая null/undefined */
function normalizeAlgorithmArgs(raw: unknown): AlgorithmArgs | null {
    if (!raw || typeof raw !== "object") return null;

    const result: Record<string, string> = {};
    for (const [key, value] of Object.entries(raw as Record<string, unknown>)) {
        if (value === null || value === undefined) continue;
        result[key] = typeof value === "string" ? value : String(value);
    }

    return Object.keys(result).length > 0 ? result : null;
}

/**
 * Строит детерминированный идентификатор алгоритма из:
 *  - имени
 *  - m
 *  - аргументов (по отсортированным ключам)
 */
function buildAlgorithmId(
    name: string,
    m: number | null,
    args: AlgorithmArgs | null,
): string {
    const mStr = m == null || Number.isNaN(m) ? "null" : String(m);

    const entries = args
        ? Object.entries(args).sort(([a], [b]) => a.localeCompare(b))
        : [];

    const argsStr = entries
        .map(([k, v]) => `${k}=${v}`)
        .join("&");

    return `${name}|m=${mStr}|${argsStr}`;
}

function normalizeName(raw: string): string {
    // режем пробел + F32/F64/Flong/Arb в конце (без учёта регистра)
    return raw.replace(/\s*(F32|F64|Flong|Arb)\s*$/i, "");
}

export function normalizeRecord(rec: ResponseRecord, idx: number): Item {
    // x может быть number|string|null
    const xRaw = (rec.series.arguments as { x?: NumLike })?.x;
    const xNum = toNumber(xRaw);
    const x = Number.isNaN(xNum) ? 0 : xNum;

    // lim тоже number|string|null
    const limRaw = rec.series.lim as NumLike;
    const limNum =
        limRaw === null || limRaw === undefined
            ? NaN
            : toNumber(limRaw);
    const seriesLim = Number.isNaN(limNum) ? null : limNum;

    const algoArgs: AlgorithmArgs | null = normalizeAlgorithmArgs(
        rec.accel.additional_args ?? null,
    );

    // m_value: number|string|null
    const mRaw = rec.accel.m_value as NumLike;
    const mNum =
        mRaw === null || mRaw === undefined ? NaN : toNumber(mRaw);
    const m = Number.isNaN(mNum) ? null : mNum;

    const rawName = rec.accel.name;
    const algorithmName = normalizeName(rawName);

    const algorithmId = buildAlgorithmId(
        algorithmName,
        m,
        algoArgs,
    );

    return {
        id: rec.stack_id ?? String(idx),
        series: {
            x,
            seriesName: normalizeName(rec.series.name),
            seriesLim,
            seriesArgs: {x},
        },
        algorithm: {
            algorithmName: algorithmName,
            m,
            algorithmArgs: algoArgs,
            algorithmId,
        },
        computed: rec.computed.map(toItemComputed),
        error: toItemError(rec.error),
    };
}

export function normalizeRecords(records: ResponseRecord[]): Item[] {
    return records.map((rec, idx) => normalizeRecord(rec, idx));
}
