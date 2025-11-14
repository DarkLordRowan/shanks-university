import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import {
    buildUniversalHeader,
    groupByNUniversal,
    uniqueUniversalKeys,
    universalSeriesLimit,
} from "./common";

export function AccelValueByN_Universal({ items }: { items: Item[] }) {
    const keys = useMemo(() => uniqueUniversalKeys(items), [items]);
    const lim = useMemo(() => universalSeriesLimit(items), [items]);
    const data = useMemo(
        () => groupByNUniversal(items, (c) => c.accel_value),
        [items],
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="accel_value"
            title={buildUniversalHeader(items)}
            formula={"y(n) = A_n"}
            refLines={
                lim != null
                    ? [{ y: lim, label: "lim", dash: "4 4" }]
                    : undefined
            }
        />
    );
}
