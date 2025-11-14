// src/charts/universal/common.ts
import type { Item, ResponseComputed } from "@/types/item";

export type UniversalChartRow = {
    n: number;
} & Record<string, number | null>;

/**
 * Ключ для линии на графике.
 * Теперь показываем только алгоритм и m, без seriesName и x,
 * т.к. они уже выбраны фильтрами.
 */
export function universalSeriesKey(item: Item): string {
    const algoBase = item.algorithm.algorithmName;
    const m = item.algorithm.m;
    return m != null ? `${algoBase} (m=${m})` : algoBase;
}

/** Уникальные ключи линий для набора items. */
export function uniqueUniversalKeys(items: Item[]): string[] {
    const seen = new Set<string>();
    const out: string[] = [];

    for (const it of items) {
        const key = universalSeriesKey(it);
        if (!seen.has(key)) {
            seen.add(key);
            out.push(key);
        }
    }
    return out;
}

/**
 * Группировка по n для произвольного набора items:
 * строки вида { n, [key1]: value1, [key2]: value2, ... }
 */
export function groupByNUniversal(
    items: Item[],
    pick: (c: ResponseComputed) => number | null,
): UniversalChartRow[] {
    type Row = UniversalChartRow;
    const byN = new Map<number, Row>();

    for (const it of items) {
        const key = universalSeriesKey(it);

        for (const c of it.computed) {
            const n = c.n;
            let row = byN.get(n);
            if (!row) {
                row = { n };
                byN.set(n, row);
            }
            row[key] = pick(c);
        }
    }

    return Array.from(byN.values()).sort((a, b) => a.n - b.n);
}

/**
 * Попытка найти единственный lim по ряду.
 */
export function universalSeriesLimit(items: Item[]): number | null {
    let hasValue = false;
    let value: number | null = null;

    for (const it of items) {
        const lim = it.series.seriesLim;
        if (lim == null) continue;
        if (!hasValue) {
            hasValue = true;
            value = lim;
        } else if (value !== lim) {
            return null;
        }
    }

    return hasValue ? value : null;
}

/**
 * Логарифм по основанию 10, безопасный к 0/NaN/∞.
 */
export function log10Safe(v: number | null | undefined): number | null {
    if (typeof v !== "number" || !Number.isFinite(v) || v === 0) return null;
    const a = Math.abs(v);
    if (!Number.isFinite(a) || a <= 0) return null;
    return Math.log10(a);
}

/**
 * Заголовок для графиков.
 * Здесь как раз показываем выбранный ряд и x (один),
 * чтобы не повторять их в тултипе.
 */
export function buildUniversalHeader(items: Item[]): string {
    if (!items.length) return "Нет данных";

    const first = items[0];
    const seriesName = first.series.seriesName;
    const x = first.series.x;
    const lim = universalSeriesLimit(items);

    let suffix = "";
    if (lim != null) {
        suffix = `, lim = ${lim}`;
    }

    return `${seriesName}, x = ${x}${suffix}`;
}
