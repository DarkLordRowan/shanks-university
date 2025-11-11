import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { algoHeader, groupByNBySeries, uniqueSeriesKeys } from "./common";

export function DeltaToLimitPartialSumByN_VarySeries({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueSeriesKeys(items), [items]);
    const data = useMemo(
        () => groupByNBySeries(items, c => c.partial_sum_deviation),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="|S_n - L|"
            title={algoHeader(items)}
            formula={"y(n)=|S_n - L|"}
        />
    );
}
