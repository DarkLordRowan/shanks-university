import type { ResponseRecord } from "@/shared/api/experiments/dto";
import type { Item, ResponseComputed, ResponseError } from "../types/item";

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

export function normalizeRecords(records: ResponseRecord[]): Item[] {
    return records.map((rec, idx) => {
        const xRaw = rec.series.arguments?.x ?? null;
        const x = typeof xRaw === "number" ? xRaw : 0;

        const algoArgsRaw = rec.accel.additional_args ?? null;

        const algorithmArgs =
            algoArgsRaw == null
                ? null
                : {
                    remainder: algoArgsRaw.remainder ?? "",
                    useRecFormulas: algoArgsRaw.useRecFormulas ?? "",
                    beta: algoArgsRaw.beta ?? "",
                };

        return {
            id: rec.stack_id ?? String(idx), // или crypto.randomUUID(), если хочется
            series: {
                x,
                seriesName: rec.series.name,
                seriesLim: rec.series.lim ?? null,
                seriesArgs: { x },
            },
            algorithm: {
                algorithmName: rec.accel.name,
                m: rec.accel.m_value ?? null,
                algorithmArgs,
            },
            computed: rec.computed.map(toItemComputed),
            error: toItemError(rec.error),
        };
    });
}
