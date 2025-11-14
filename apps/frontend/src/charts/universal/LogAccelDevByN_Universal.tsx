import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import {
    buildUniversalHeader,
    groupByNUniversal,
    uniqueUniversalKeys,
    universalSeriesLimit,
    log10Safe,
} from "./common";

export function LogAccelDevByN_Universal({ items }: { items: Item[] }) {
    const keys = useMemo(() => uniqueUniversalKeys(items), [items]);
    const lim = useMemo(() => universalSeriesLimit(items), [items]);
    const data = useMemo(
        () =>
            groupByNUniversal(items, (c) =>
                log10Safe(c.accel_value_deviation),
            ),
        [items],
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10 |A_n - lim|"
            title={buildUniversalHeader(items)}
            formula={"\\log_{10}|A_n - \\lim|"}
            refLines={
                lim != null
                    ? [{ y: 0, label: "lim", dash: "4 4" }]
                    : undefined
            }
        />
    );
}
