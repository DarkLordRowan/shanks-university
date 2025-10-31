import { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, groupByN, uniqueAlgoKeys } from "./common";

export function AccelerationGainChartByN({items, clamp = {min: -12, max: 12}}: {
    items: Item[],
    clamp?: { min: number, max: number }
}) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const data = useMemo(() => {
        const base = groupByN(items, c => {
            const ps = c.partial_sum_deviation, av = c.accel_value_deviation;
            if (!(ps > 0 && av > 0)) return null;
            const v = Math.log10(ps / av);
            return Number.isFinite(v) ? Math.max(clamp.min, Math.min(clamp.max, v)) : null;
        });
        return base;
    }, [items, clamp.min, clamp.max]);
    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10(|S_n-L|/|A_n-L|)"
            title={buildSeriesHeader(items)}
            formula={"G(n)=\\log_{10}\\left(\\dfrac{|S_n-L|}{|A_n-L|}\\right)"}
            refLines={[{y: 0, label: "G=0"}]}
            tooltipFormatter={(v, name) => [typeof v === "number" ? v.toFixed(3) : v, name]}
        />
    );
}
