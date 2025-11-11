import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader } from "../common";
import { groupByNByX, log10Safe, uniqueXKeys } from "./common";

export function LogAccelDevByN_VaryX({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueXKeys(items), [items]);
    const data = useMemo(
        () => groupByNByX(items, c => log10Safe(c.accel_value_deviation)),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10(|A_n - L|)"
            title={buildSeriesHeader(items)}
            formula={"y(n)=\\log_{10}|A_n - L|"}
        />
    );
}
