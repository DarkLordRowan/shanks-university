import { useMemo } from "react";
import type { Item } from "../../types/item";
import { BaseChart } from "../BaseChart";
import { buildSeriesHeader } from "../common";
import { groupByNByX, uniqueXKeys } from "./common";

export function SeriesTermByN_VaryX({items}: { items: Item[] }) {
    const keys = useMemo(() => uniqueXKeys(items), [items]);
    const data = useMemo(
        () => groupByNByX(items, c => c.series_value),
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
