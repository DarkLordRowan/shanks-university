// src/charts/DeltaToLimitPartialSumChart.tsx
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, groupByN, seriesLimit, uniqueAlgoKeys } from "./common";
import { useMemo } from "react";

interface DeltaToLimitPartialSumChartProps {
    items: Item[];
}

export function DeltaToLimitPartialSumChart({ items }: DeltaToLimitPartialSumChartProps) {
    const lim = seriesLimit(items);
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);

    const data = useMemo(() => {
        // |partial_sum - lim| если лимит известен; иначе просто partial_sum
        return groupByN(items, c => {
            if (lim == null) return c.partial_sum;
            const d = Math.abs(c.partial_sum - lim);
            return Number.isFinite(d) ? d : null;
        });
    }, [items, lim]);

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel={lim == null ? "partial_sum" : "|partial_sum - lim|"}
            title={buildSeriesHeader(items)}
            formula={
                lim == null
                    ? "y(n) = S_n"
                    : "y(n) = |S_n - \\lim_{n\\to\\infty} S_n|"
            }
            refLines={lim != null ? [{ y: 0, label: "совпадение с лимитом" }] : undefined}
        />
    );
}
