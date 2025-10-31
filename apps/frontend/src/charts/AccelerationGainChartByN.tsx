// src/charts/AccelerationGainChartByN.tsx
import { useMemo } from "react";
import type { Item } from "../types/item";
import { BaseChart } from "./BaseChart";
import { buildSeriesHeader, uniqueAlgoKeys, groupByN } from "./common";

interface AccelerationGainChartByNProps {
    items: Item[];
    clamp?: { min: number; max: number } | null;
}

export function AccelerationGainChartByN({
                                             items,
                                             clamp = { min: -12, max: 12 },
                                         }: AccelerationGainChartByNProps) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);

    const data = useMemo(() => {
        const base = groupByN(items, c => {
            const ps = c.partial_sum_deviation;
            const av = c.accel_value_deviation;
            if (!(ps > 0 && av > 0)) return null;
            const v = Math.log10(ps / av);
            return Number.isFinite(v) ? v : null;
        });
        if (!clamp) return base;
        return base.map(row => {
            const r: any = { ...row };
            for (const k of keys) {
                const v = r[k];
                if (v != null) r[k] = Math.max(clamp.min, Math.min(clamp.max, v));
            }
            return r;
        });
    }, [items, keys, clamp]);

    return (
        <BaseChart
            data={data}
            seriesKeys={keys}
            yAxisLabel="log10(ps_dev / accel_dev)"
            title={buildSeriesHeader(items)}
            formula={"G(n) = \\log_{10}\\left(\\dfrac{\\mathrm{partial\\_sum\\_deviation}(n)}{\\mathrm{accel\\_value\\_deviation}(n)}\\right)"}
            refLines={[{ y: 0, label: "G(n)=0" }]}
            tooltipFormatter={(v, name) => [typeof v === "number" ? v.toFixed(3) : v, name]}
        />
    );
}
