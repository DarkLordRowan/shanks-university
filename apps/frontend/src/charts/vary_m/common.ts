import type { Item } from "../../types/item";
import type { ResponseComputed } from "../../types/item";

/** Ключ серии по m. */
export function mKeyOf(item: Item): string {
  const m = item.algorithm.m;
  return m == null ? "m=∅" : `m=${m}`;
}

/** Уникальные ключи серий по m в порядке возрастания m, null в конце. */
export function uniqueMKeys(items: Item[]): string[] {
  const entries: Array<{ key: string; m: number | null }> = [];
  const seen = new Set<string>();
  for (const it of items) {
    const key = mKeyOf(it);
    if (!seen.has(key)) {
      seen.add(key);
      entries.push({ key, m: it.algorithm.m ?? null });
    }
  }
  entries.sort((a, b) => {
    if (a.m == null && b.m == null) return a.key.localeCompare(b.key);
    if (a.m == null) return 1;
    if (b.m == null) return -1;
    return a.m - b.m;
  });
  return entries.map(e => e.key);
}

/**
 * Группировка: по n, колонки — разные m (ключи `m=...`).
 * `pick` берёт число из блока `ResponseComputed`.
 */
export function groupByNByM(
  items: Item[],
  pick: (c: ResponseComputed) => number | null
): Array<Record<string, number | null>> {
  type Row = Record<string, number | null> & { n: number };
  const byN = new Map<number, Row>();

  for (const it of items) {
    const k = mKeyOf(it);
    for (const c of it.computed) {
      const n = c.n;
      const v = pick(c);
      const row = byN.get(n) ?? { n };
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
