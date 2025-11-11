import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { algoHeader, groupByNBySeries, uniqueSeriesKeys } from "./common";

export function SeriesTermByN_VarySeries({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueSeriesKeys(items), [items]);
    const data = useMemo(
        () => groupByNBySeries(items, c => c.series_value),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="a_n"
            title={algoHeader(items)}
            formula={"y(n)=a_n"}
        />
    );
}
