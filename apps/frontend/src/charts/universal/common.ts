// src/charts/universal/common.ts

import type { Item, ResponseComputed } from "@/types/item.ts";

export type UniversalChartRow = {
    n: number;
} & Record<string, number | null>;

/**
 * Человекочитаемый ключ для линии:
 * seriesName · x=... · algo · (m=...)
 */
export function universalSeriesKey(item: Item): string {
    const parts: string[] = [];

    parts.push(item.series.seriesName);
    parts.push(`x=${item.series.x}`);

    const algoBase = item.algorithm.algorithmName;
    const m = item.algorithm.m;
    const algoFull = m != null ? `${algoBase} (m=${m})` : algoBase;
    parts.push(algoFull);

    return parts.join(" · ");
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
 *
 * pick(c) вытаскивает нужное поле из ResponseComputed
 * (accel_value, partial_sum, deviation и т.д.).
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
 * Если lim один и тот же везде (и не null), возвращаем его.
 * Иначе -> null (чтобы не рисовать одну "ложную" линию).
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
            return null; // разные lim -> не рисуем
        }
    }

    return hasValue ? value : null;
}

/** Логарифм по основанию 10, безопасный к 0/NaN/∞. */
export function log10Safe(v: number | null | undefined): number | null {
    if (typeof v !== "number" || !Number.isFinite(v) || v === 0) return null;
    const a = Math.abs(v);
    if (!Number.isFinite(a) || a <= 0) return null;
    return Math.log10(a);
}

/**
 * Заголовок для универсальных графиков.
 * Короткая сводка: сколько разных рядов, x и алгоритмов.
 */
export function buildUniversalHeader(items: Item[]): string {
    const seriesNames = new Set<string>();
    const xs = new Set<number>();
    const algos = new Set<string>();

    for (const it of items) {
        seriesNames.add(it.series.seriesName);
        xs.add(it.series.x);
        algos.add(it.algorithm.algorithmName);
    }

    return `Ряды: ${seriesNames.size}, x: ${xs.size}, алгоритмы: ${algos.size}`;
}
