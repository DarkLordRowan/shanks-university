// PartialSumChartByN.tsx
import React, { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { uniqueAlgoKeys, groupByN, seriesLimit, buildSeriesHeader } from "./common";

export function PartialSumChartByN({ items }: { items: Item[] }) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const lim = seriesLimit(items);
    const data = useMemo(() => groupByN(items, c => c.partial_sum), [items]);
    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="partial_sum"
            title={buildSeriesHeader(items)}
            formula={"y(n)=S_n"}
            refLines={lim != null ? [{ y: lim, label: "lim", dash: "4 4" }] : undefined}
        />
    );
}
