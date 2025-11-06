import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader } from "../common";
import { groupByNByM, uniqueMKeys } from "./common";

export function DeltaToLimitPartialSumByN_VaryM({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueMKeys(items), [items]);
    const data = useMemo(
        () => groupByNByM(items, c => c.partial_sum_deviation),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="|S_n - L|"
            title={buildSeriesHeader(items)}
            formula={"y(n)=|S_n - L|"}
        />
    );
}
