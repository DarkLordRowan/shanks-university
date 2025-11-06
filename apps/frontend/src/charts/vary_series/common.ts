import type { Item, ResponseComputed } from "../../types/item";

/** Ключ серии по названию ряда. */
export function seriesKeyOf(item: Item): string {
    return item.series.seriesName;
}

/** Заголовок: фиксированный алгоритм и m. */
export function algoHeader(items: Item[]): string {
    const a = items[0]?.algorithm;
    const name = a?.algorithmName ?? "algorithm";
    const m = a?.m;
    return m == null ? `${name}, m=∅` : `${name}, m=${m}`;
}

/** Уникальные ключи серий по названию ряда (лексикографический порядок). */
export function uniqueSeriesKeys(items: Item[]): string[] {
    const set = new Set<string>();
    for (const it of items) set.add(seriesKeyOf(it));
    return Array.from(set).sort((a, b) => a.localeCompare(b));
}

/**
 * Группировка: по n, колонки — разные ряды.
 * pick получает число из блока ResponseComputed.
 */
export function groupByNBySeries(
    items: Item[],
    pick: (c: ResponseComputed) => number | null
): Array<Record<string, number | null>> {
    type Row = Record<string, number | null> & { n: number };
    const byN = new Map<number, Row>();

    for (const it of items) {
        const key = seriesKeyOf(it);
        for (const c of it.computed) {
            const n = c.n;
            const v = pick(c);
            const row = byN.get(n) ?? {n};
            row[key] = v;
            byN.set(n, row);
        }
    }
    return Array.from(byN.values()).sort((a, b) => a.n - b.n);
}

/** Логарифм по основанию 10, безопасный к 0/NaN. */
export function log10Safe(v: number | null | undefined): number | null {
    if (typeof v !== "number" || !Number.isFinite(v) || v === 0) return null;
    const a = Math.abs(v);
    if (!Number.isFinite(a) || a <= 0) return null;
    return Math.log10(a);
}
