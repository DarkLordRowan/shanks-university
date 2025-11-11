import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader, seriesLimit } from "../common";
import { groupByNByM, uniqueMKeys } from "./common";

export function PartialSumByN_VaryM({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueMKeys(items), [items]);
    const lim = seriesLimit(items);
    const data = useMemo(
        () => groupByNByM(items, c => c.partial_sum),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="partial_sum"
            title={buildSeriesHeader(items)}
            formula={"y(n)=S_n"}
            refLines={lim != null ? [{y: lim, label: "lim", dash: "4 4"}] : undefined}
        />
    );
}
