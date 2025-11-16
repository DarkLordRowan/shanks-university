import type { Item } from "@/types/item";
import type { DiffCell, SeriesKey, AlgoKey } from "@/types/diffCell";

/**
 * Оценка ряда в [0, 100].
 */
export function scoreFromItem(item: Item | undefined | null): number | null {
    if (!item) return null;

    const computed = item.computed ?? [];
    let maxN = 0;
    for (const c of computed) {
        if (typeof c.n === "number" && Number.isFinite(c.n)) {
            if (c.n > maxN) maxN = c.n;
        }
    }

    const errN = item.error?.data?.n ?? null;

    if (!item.error) {
        return 100;
    }

    if (!Number.isFinite(maxN) || maxN <= 0 || errN == null || !Number.isFinite(errN)) {
        return 0;
    }

    const completed = Math.max(0, errN - 1);
    const ratio = Math.max(0, Math.min(1, completed / maxN));
    return Math.round(ratio * 100);
}

export function buildSeriesKey(seriesName: string, x: number): SeriesKey {
    return `${seriesName} | x=${x}`;
}

export function buildCellKey(algoKey: AlgoKey, seriesKey: SeriesKey): string {
    return `${algoKey}||${seriesKey}`;
}

/**
 * Обновление DiffCell данными из первого JSON (prev).
 */
export function applyFromPrev(item: Item, cell: DiffCell | null): DiffCell {
    const seriesKey = buildSeriesKey(item.series.seriesName, item.series.x);
    const algoKey: AlgoKey = item.algorithm.algorithmId;
    const score = scoreFromItem(item);
    const hasError = !!item.error;
    const errorN = item.error?.data?.n ?? null;
    const errorDescription = item.error?.description ?? null;

    if (cell) {
        return {
            ...cell,
            seriesKey,
            seriesName: item.series.seriesName,
            x: item.series.x,
            algoKey,
            algorithmName: item.algorithm.algorithmName,
            m: item.algorithm.m,
            algorithmArgs: item.algorithm.algorithmArgs ?? null,
            scorePrev: score,
            hasErrorPrev: hasError,
            errorNPrev: errorN,
            errorDescriptionPrev: errorDescription,
        };
    }

    return {
        seriesKey,
        seriesName: item.series.seriesName,
        x: item.series.x,
        algoKey,
        algorithmName: item.algorithm.algorithmName,
        m: item.algorithm.m,
        algorithmArgs: item.algorithm.algorithmArgs ?? null,
        scorePrev: score,
        hasErrorPrev: hasError,
        errorNPrev: errorN,
        errorDescriptionPrev: errorDescription,
        scoreNext: null,
        hasErrorNext: false,
        errorNNext: null,
        errorDescriptionNext: null,
    };
}

/**
 * Обновление DiffCell данными из второго JSON (next).
 */
export function applyFromNext(item: Item, cell: DiffCell | null): DiffCell {
    const seriesKey = buildSeriesKey(item.series.seriesName, item.series.x);
    const algoKey: AlgoKey = item.algorithm.algorithmId;
    const score = scoreFromItem(item);
    const hasError = !!item.error;
    const errorN = item.error?.data?.n ?? null;
    const errorDescription = item.error?.description ?? null;

    if (cell) {
        return {
            ...cell,
            seriesKey,
            seriesName: item.series.seriesName,
            x: item.series.x,
            algoKey,
            algorithmName: item.algorithm.algorithmName,
            m: item.algorithm.m,
            algorithmArgs: item.algorithm.algorithmArgs ?? null,
            scoreNext: score,
            hasErrorNext: hasError,
            errorNNext: errorN,
            errorDescriptionNext: errorDescription,
        };
    }

    return {
        seriesKey,
        seriesName: item.series.seriesName,
        x: item.series.x,
        algoKey,
        algorithmName: item.algorithm.algorithmName,
        m: item.algorithm.m,
        algorithmArgs: item.algorithm.algorithmArgs ?? null,
        scorePrev: null,
        hasErrorPrev: false,
        errorNPrev: null,
        errorDescriptionPrev: null,
        scoreNext: score,
        hasErrorNext: hasError,
        errorNNext: errorN,
        errorDescriptionNext: errorDescription,
    };
}
