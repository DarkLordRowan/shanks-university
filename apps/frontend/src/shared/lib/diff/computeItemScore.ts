import type { Item } from "../entities/item/model/item";

/**
 * score ∈ [0, 100]:
 * 100 — нет ошибки,
 * 0   — ошибка "сразу" или данных мало,
 * (0,100) — доля шагов до ошибки.
 */
export function computeItemScore(it: Item): number | null {
    const computed = it.computed ?? [];

    let maxN = 0;
    for (const c of computed) {
        if (typeof c.n === "number" && Number.isFinite(c.n)) {
            if (c.n > maxN) maxN = c.n;
        }
    }

    const err = it.error ?? null;
    const errN = err?.data?.n ?? null;

    // нет ошибки => считаем, что ряд собран
    if (!err) {
        return 100;
    }

    // ошибок много / мало данных
    if (!Number.isFinite(maxN) || maxN <= 0 || errN == null || !Number.isFinite(errN)) {
        return 0;
    }

    const completed = Math.max(0, errN - 1);
    const ratio = Math.max(0, Math.min(1, completed / maxN));
    return Math.round(ratio * 100);
}
