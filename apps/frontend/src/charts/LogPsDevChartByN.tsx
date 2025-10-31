// src/charts/LogPsDevChartByN.tsx
import  { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, groupByN, safeLog10, uniqueAlgoKeys } from "./common";

interface LogPsDevChartByNProps {
    items: Item[];                 // одинаковая series, разные algorithm
    clamp?: { min: number; max: number } | null;  // напр. {min: -12, max: 6}
}

export function LogPsDevChartByN({ items, clamp = null }: LogPsDevChartByNProps) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);

    const data = useMemo(() => {
        const base = groupByN(items, c => safeLog10(c.partial_sum_deviation));
        if (!clamp) return base;
        return base.map(row => {
            const r: any = { ...row };
            for (const k of keys) {
                const v = r[k];
                if (v != null) {
                    if (v < clamp.min) r[k] = clamp.min;
                    else if (v > clamp.max) r[k] = clamp.max;
                }
            }
            return r;
        });
    }, [items, keys, clamp]);

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10(partial_sum_deviation)"
            title={buildSeriesHeader(items)}
            formula={"y(n) = \\log_{10}\\big(\\mathrm{partial\\_sum\\_deviation}(n)\\big)"}
        />
    );
}
