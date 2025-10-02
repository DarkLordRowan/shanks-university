import { parseResponse, type ResponseRecord } from "../types/response.zod.ts";
import type { Item } from "../types/item.ts";

export function normalizeRecords(records: ResponseRecord[]): Item[] {
    return records.map(rec => ({
        id: rec.id,
        x: rec.series.arguments.x,
        seriesName: rec.series.name,
        seriesLim: rec.series.lim,
        seriesArgs: rec.series.arguments,
        algorithmName: rec.accel.name,
        m: rec.accel.m_value ?? null,
        algorithmArgs: rec.accel.additional_args,
        computed: rec.computed,
        error: rec.error,
    }));
}

export function normalizeFromJson(json: unknown): Item[] {
    const data: ResponseRecord[] = parseResponse(json);
    return normalizeRecords(data);
}