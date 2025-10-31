// AccelValueChartByN.tsx
import React, { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { uniqueAlgoKeys, groupByN, seriesLimit, buildSeriesHeader } from "./common";

export function AccelValueChartByN({ items }: { items: Item[] }) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const lim = seriesLimit(items);
    const data = useMemo(() => groupByN(items, c => c.accel_value), [items]);
    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="accel_value"
            title={buildSeriesHeader(items)}
            formula={"y(n)=A_n"}
            refLines={lim != null ? [{ y: lim, label: "lim", dash: "4 4" }] : undefined}
        />
    );
}
