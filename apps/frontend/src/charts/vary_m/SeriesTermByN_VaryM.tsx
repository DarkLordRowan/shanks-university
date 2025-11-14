import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader } from "../common";
import { groupByNByM, uniqueMKeys } from "./common";

export function SeriesTermByN_VaryM({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueMKeys(items), [items]);
    const data = useMemo(
        () => groupByNByM(items, c => c.series_value),
        [items]
    );

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="a_n"
            title={buildSeriesHeader(items)}
            formula={"y(n)=a_n"}
        />
    );
}
