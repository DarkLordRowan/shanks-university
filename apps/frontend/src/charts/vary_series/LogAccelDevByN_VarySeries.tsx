import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { algoHeader, groupByNBySeries, log10Safe, uniqueSeriesKeys } from "./common";

export function LogAccelDevByN_VarySeries({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueSeriesKeys(items), [items]);
    const data = useMemo(
        () => groupByNBySeries(items, c => log10Safe(c.accel_value_deviation)),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10(|A_n - L|)"
            title={algoHeader(items)}
            formula={"y(n)=\\log_{10}|A_n - L|"}
        />
    );
}
