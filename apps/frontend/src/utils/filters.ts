import type { Item } from "@/types/item";

export interface ItemsFilterState {
    seriesNames: string[];          // пустой массив = не фильтровать по seriesName
    algorithmNames: string[];       // пустой массив = не фильтровать по algorithmName
    mValues: (number | null)[];     // пустой массив = не фильтровать по m
    xMin?: number;                  // undefined = не ограничивать
    xMax?: number;                  // undefined = не ограничивать
    hasError?: boolean | null;      // true = только с error, false = только без error, null/undefined = не фильтровать
}

export interface ItemsFilterOptions {
    seriesNames: string[];
    algorithmNames: string[];
    mValues: (number | null)[];
    xMin: number | null;
    xMax: number | null;
}

/**
 * Построение допустимых значений фильтра по имеющимся items.
 */
export function buildFilterOptions(items: Item[]): ItemsFilterOptions {
    const seriesNamesSet = new Set<string>();
    const algorithmNamesSet = new Set<string>();
    const mValuesSet = new Set<number | null>();

    let xMin: number | null = null;
    let xMax: number | null = null;

    for (const it of items) {
        seriesNamesSet.add(it.series.seriesName);
        algorithmNamesSet.add(it.algorithm.algorithmName);
        mValuesSet.add(it.algorithm.m);

        const x = it.series.x;
        if (xMin === null || x < xMin) xMin = x;
        if (xMax === null || x > xMax) xMax = x;
    }

    return {
        seriesNames: Array.from(seriesNamesSet).sort(),
        algorithmNames: Array.from(algorithmNamesSet).sort(),
        mValues: Array.from(mValuesSet).sort((a, b) => {
            // сортировка: сначала null, потом по числу
            if (a === b) return 0;
            if (a === null) return -1;
            if (b === null) return 1;
            return a - b;
        }),
        xMin,
        xMax,
    };
}

/**
 * Применение фильтра к массиву items.
 */
export function applyItemsFilter(
    items: Item[],
    filter: ItemsFilterState,
): Item[] {
    const {
        seriesNames,
        algorithmNames,
        mValues,
        xMin,
        xMax,
        hasError,
    } = filter;

    return items.filter((it) => {
        if (seriesNames.length > 0 && !seriesNames.includes(it.series.seriesName)) {
            return false;
        }

        if (
            algorithmNames.length > 0 &&
            !algorithmNames.includes(it.algorithm.algorithmName)
        ) {
            return false;
        }

        if (mValues.length > 0 && !mValues.includes(it.algorithm.m)) {
            return false;
        }

        if (xMin !== undefined && it.series.x < xMin) {
            return false;
        }

        if (xMax !== undefined && it.series.x > xMax) {
            return false;
        }

        if (hasError === true && !it.error) {
            return false;
        }

        if (hasError === false && it.error) {
            return false;
        }

        return true;
    });
}
