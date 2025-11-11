// src/charts/common.ts
import type { Item, ResponseComputed } from "../types/item";

export type ChartRow = { n: number } & Record<string, number | null>;

export function colorByKey(key: string): string {
    let h = 0;
    for (let i = 0; i < key.length; i++) h = (h * 31 + key.charCodeAt(i)) >>> 0;
    return `hsl(${h % 360} 70% 45%)`;
}

export function algoKeyOf(item: Item): string {
    return item.algorithm.m != null
        ? `${item.algorithm.algorithmName} (m=${item.algorithm.m})`
        : item.algorithm.algorithmName;
}

export function uniqueAlgoKeys(items: Item[]): string[] {
    const s = new Set<string>();
    for (const it of items) s.add(algoKeyOf(it));
    return Array.from(s);
}

export function seriesLimit(items: Item[]): number | null {
    return items.length ? items[0].series.seriesLim : null;
}

export function buildSeriesHeader(items: Item[]): string {
    if (!items.length) return "";
    const s = items[0].series;
    const base = `${s.seriesName} | x=${s.x}`;
    return s.seriesLim != null ? `${base} | lim=${s.seriesLim}` : base;
}

export function safeLog10(x: number): number | null {
    if (!(x > 0)) return null;
    const v = Math.log10(x);
    return Number.isFinite(v) ? v : null;
}

/**
 * Обобщённый конструктор таблицы вида:
 * [{ n, "<Algo A>": value, "<Algo B>": value, ... }]
 * selector задаёт, как получить значение из ResponseComputed.
 */
export function groupByN(
    items: Item[],
    selector: (c: ResponseComputed) => number | null
): ChartRow[] {
    const byN = new Map<number, ChartRow>();

    for (const it of items) {
        const key = algoKeyOf(it);
        for (const c of it.computed) {
            let row = byN.get(c.n);
            if (!row) {
                row = { n: c.n };
                byN.set(c.n, row);
            }
            row[key] = selector(c);
        }
    }
    return Array.from(byN.values()).sort((a, b) => a.n - b.n);
}

/** Опциональная «обрезка» значений для стабилизации шкалы */
export function clamp(y: number | null, min: number, max: number): number | null {
    if (y == null || !Number.isFinite(y)) return null;
    if (y < min) return min;
    if (y > max) return max;
    return y;
}



/** Текстовый ключ серии по m. */
export function mKeyOf(item: Item): string {
    const m = item.algorithm.m;
    return m == null ? "m=∅" : `m=${m}`;
}

/** Уникальные ключи серий по m в порядке возрастания m, null в конце. */
export function uniqueMKeys(items: Item[]): string[] {
    const pairs: Array<{ k: string; m: number | null }> = [];
    const seen = new Set<string>();
    for (const it of items) {
        const k = mKeyOf(it);
        if (!seen.has(k)) {
            seen.add(k);
            pairs.push({ k, m: it.algorithm.m ?? null });
        }
    }
    pairs.sort((a, b) => {
        if (a.m == null && b.m == null) return 0;
        if (a.m == null) return 1;
        if (b.m == null) return -1;
        return a.m - b.m;
    });
    return pairs.map(p => p.k);
}

/**
 * Группировка по n с разложением по сериям m.
 * pick получает блок computed, должен вернуть число или null.
 */
export function groupByNByM(
    items: Item[],
    pick: (c: Item["computed"]) => number | null
): Array<Record<string, number | null>> {
    // Считаем, что в выборке items фиксированы series.name, series.arguments.x и accel.name.
    // Объединяем по n, серии отождествляем по mKeyOf(item).
    type Row = Record<string, number | null> & { n: number };
    const byN = new Map<number, Row>();

    for (const it of items) {
        const n = it.computed.n;
        const key = mKeyOf(it);
        const v = pick(it.computed);
        const row = byN.get(n) ?? { n };
        row[key] = v;
        byN.set(n, row);
    }

    return Array.from(byN.values()).sort((a, b) => a.n - b.n);
}