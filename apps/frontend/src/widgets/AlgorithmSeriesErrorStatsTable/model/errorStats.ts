// src/widgets/AlgorithmSeriesErrorStatsTable/model/errorStats.ts

export interface ErrorStats {
    count: number;
    min: number;
    max: number;
    mean: number;
}

export function computeErrorStats(values: Array<number | null | undefined>): ErrorStats | null {
    const xs: number[] = [];

    for (const v of values) {
        if (v == null) continue;
        const x = Math.abs(v); // если нужен знак: const x = v;
        if (!Number.isFinite(x)) continue;
        xs.push(x);
    }

    if (xs.length === 0) return null;

    let min = xs[0];
    let max = xs[0];
    let sum = 0;

    for (const x of xs) {
        if (x < min) min = x;
        if (x > max) max = x;
        sum += x;
    }

    return {
        count: xs.length,
        min,
        max,
        mean: sum / xs.length,
    };
}
