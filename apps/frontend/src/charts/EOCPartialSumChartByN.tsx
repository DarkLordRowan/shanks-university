import { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, uniqueAlgoKeys } from "./common";

export function EOCPartialSumChartByN({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const byAlgo = new Map<string, { n: number, v: number | null }[]>();

    for (const it of items) {
        const k = it.algorithm.m != null ? `${it.algorithm.algorithmName} (m=${it.algorithm.m})` : it.algorithm.algorithmName;
        const arr = it.computed.slice().sort((a, b) => a.n - b.n);
        const rows: { n: number, v: number | null }[] = [];
        for (let i = 1; i < arr.length; i++) {
            const e = arr[i].partial_sum_deviation;
            const p = arr[i - 1].partial_sum_deviation;
            const val = (e > 0 && p > 0) ? Math.log10(e / p) : null;
            rows.push({n: arr[i].n, v: Number.isFinite(val) ? val : null});
        }
        byAlgo.set(k, rows);
    }

    // объединяем по n
    const byN = new Map<number, any>();
    for (const [k, rows] of byAlgo) {
        for (const r of rows) {
            if (!byN.has(r.n)) byN.set(r.n, {n: r.n});
            byN.get(r.n)[k] = r.v;
        }
    }
    const data = Array.from(byN.values()).sort((a, b) => a.n - b.n);

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="EOC (partial_sum)"
            title={buildSeriesHeader(items)}
            formula={"\\mathrm{EOC}_n=\\log_{10}\\left(\\dfrac{|S_n-L|}{|S_{n-1}-L|}\\right),\\ n\\ge2"}
            refLines={[{y: 0, label: "без улучшения"}]}
        />
    );
}
