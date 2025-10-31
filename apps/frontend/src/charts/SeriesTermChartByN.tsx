// SeriesTermChartByN.tsx
import React, { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { uniqueAlgoKeys, groupByN, buildSeriesHeader } from "./common";

export function SeriesTermChartByN({ items }: { items: Item[] }) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const data = useMemo(() => groupByN(items, c => c.series_value), [items]);
    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="series_value"
            title={buildSeriesHeader(items)}
            formula={"y(n)=a_n"}
        />
    );
}
