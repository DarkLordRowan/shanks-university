import type { ResponseRecord } from "@/shared/api/experiments/dto";
import type { AlgorithmArgs, Item, ResponseComputed, ResponseError } from "../types/item";
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

function toItemComputed(raw: {
    n: number;
    series_value: NumLike;
    partial_sum: NumLike;
    partial_sum_deviation: NumLike;
    accel_value: NumLike;
    accel_value_deviation: NumLike;
}): ResponseComputed {
    return {
        n: raw.n,
        series_value: toNumber(raw.series_value),
        partial_sum: toNumber(raw.partial_sum),
        partial_sum_deviation: toNumber(raw.partial_sum_deviation),
        accel_value: toNumber(raw.accel_value),
        accel_value_deviation: toNumber(raw.accel_value_deviation),
    };
}

function toItemError(raw: ResponseRecord["error"]): ResponseError | null {
    if (!raw) return null;

    return {
        description: raw.description,
        data: {
            n: raw.data?.n ?? null,
        },
    };
}

/** Превращает additional_args в словарь string→string, отбрасывая null/undefined */
function normalizeAlgorithmArgs(raw: unknown): AlgorithmArgs | null {
    if (!raw || typeof raw !== "object") return null;

    const result: Record<string, string> = {};
    for (const [key, value] of Object.entries(raw as Record<string, unknown>)) {
        if (value === null || value === undefined) continue;
        // сохраним всё как строки
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
    args: AlgorithmArgs | null
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


export function normalizeRecords(records: ResponseRecord[]): Item[] {
    return records.map((rec, idx) => {
        const xRaw = rec.series.arguments?.x ?? null;
        const x = typeof xRaw === "number" ? xRaw : 0;

        const algoArgs: AlgorithmArgs | null = normalizeAlgorithmArgs(
            rec.accel.additional_args ?? null
        );

        const m = rec.accel.m_value ?? null;

        const algorithmId = buildAlgorithmId(
            rec.accel.name,
            m,
            algoArgs
        );

        return {
            id: rec.stack_id ?? String(idx), // при желании можно заменить на crypto.randomUUID()
            series: {
                x,
                seriesName: rec.series.name,
                seriesLim: rec.series.lim ?? null,
                seriesArgs: { x },
            },
            algorithm: {
                algorithmName: rec.accel.name,
                m,
                algorithmArgs: algoArgs,
                algorithmId,
            },
            computed: rec.computed.map(toItemComputed),
            error: toItemError(rec.error),
        };
    });
}