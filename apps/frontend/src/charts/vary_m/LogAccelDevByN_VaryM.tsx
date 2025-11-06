import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader } from "../common";
import { groupByNByM, log10Safe, uniqueMKeys } from "./common";

export function LogAccelDevByN_VaryM({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueMKeys(items), [items]);
    const data = useMemo(
        () => groupByNByM(items, c => log10Safe(c.accel_value_deviation)),
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
