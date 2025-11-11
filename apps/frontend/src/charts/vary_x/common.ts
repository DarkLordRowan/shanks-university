import type { Item, ResponseComputed } from "../../types/item";

/** Ключ серии по x. */
export function xKeyOf(item: Item): string {
    const x = item.series.x;
    return `x=${x}`;
}

/** Уникальные ключи серий по x (сортировка по возрастанию x). */
export function uniqueXKeys(items: Item[]): string[] {
    const arr: Array<{ key: string; x: number }> = [];
    const seen = new Set<string>();
    for (const it of items) {
        const key = xKeyOf(it);
        if (!seen.has(key)) {
            seen.add(key);
            arr.push({key, x: it.series.x});
        }
    }
    arr.sort((a, b) => a.x - b.x);
    return arr.map(e => e.key);
}

/**
 * Группировка: по n, колонки — разные x.
 * pick получает число из блока ResponseComputed.
 */
export function groupByNByX(
    items: Item[],
    pick: (c: ResponseComputed) => number | null
): Array<Record<string, number | null>> {
    type Row = Record<string, number | null> & { n: number };
    const byN = new Map<number, Row>();

    for (const it of items) {
        const k = xKeyOf(it);
        for (const c of it.computed) {
            const n = c.n;
            const v = pick(c);
            const row = byN.get(n) ?? {n};
            row[k] = v;
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
