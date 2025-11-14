import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader, seriesLimit } from "../common";
import { groupByNByM, uniqueMKeys } from "./common";

export function AccelValueByN_VaryM({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueMKeys(items), [items]);
    const lim = seriesLimit(items);
    const data = useMemo(
        () => groupByNByM(items, c => c.accel_value),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="accel_value"
            title={buildSeriesHeader(items)}
            formula={"y(n)=A_n"}
            refLines={lim != null ? [{y: lim, label: "lim", dash: "4 4"}] : undefined}
        />
    );
}
