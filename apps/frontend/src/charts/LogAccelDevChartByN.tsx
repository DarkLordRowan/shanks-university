import { useMemo } from "react";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, groupByN, safeLog10, uniqueAlgoKeys } from "./common";

export function LogAccelDevChartByN({items, clamp = null as null | { min: number, max: number }}) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const base = useMemo(() => groupByN(items, c => safeLog10(c.accel_value_deviation)), [items]);
    const data = useMemo(() => {
        if (!clamp) return base;
        return base.map(r => {
            const x: any = {...r};
            for (const k of keys) if (x[k] != null) x[k] = Math.max(clamp.min, Math.min(clamp.max, x[k]));
            return x;
        });
    }, [base, keys, clamp]);
    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10(|A_n - L|)"
            title={buildSeriesHeader(items)}
            formula={"y(n)=\\log_{10}|A_n-L|"}
        />
    );
}
