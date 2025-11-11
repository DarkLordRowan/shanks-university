import { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, groupByN, seriesLimit, uniqueAlgoKeys } from "./common";

export function DeltaToLimitPartialSumChart({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const lim = seriesLimit(items);
    const data = useMemo(() => {
        if (lim == null) return [];
        return groupByN(items, c => Math.abs(c.partial_sum - lim));
    }, [items, lim]);
    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="|S_n - L|"
            title={buildSeriesHeader(items)}
            formula={"y(n)=|S_n-L|"}
            refLines={[{y: 0, label: "совпадение с L"}]}
        />
    );
}
