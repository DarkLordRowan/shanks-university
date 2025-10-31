// StepsToToleranceBar.tsx
import React, { useMemo } from "react";
import type { Item } from "../types/item";
import { colorByKey, uniqueAlgoKeys } from "./common";
import { ResponsiveContainer, BarChart, XAxis, YAxis, Tooltip, Legend, CartesianGrid, Bar } from "recharts";

export function StepsToToleranceBar({ items, eps = 1e-8 }: { items: Item[], eps?: number }) {
    const keys = useMemo(() => uniqueAlgoKeys(items), [items]);
    const data = useMemo(() => {
        const rows: { algo: string; n: number | null }[] = [];
        for (const it of items) {
            const k = it.algorithm.m != null ? `${it.algorithm.algorithmName} (m=${it.algorithm.m})` : it.algorithm.algorithmName;
            let nHit: number | null = null;
            for (const c of it.computed) {
                if (c.accel_value_deviation >= 0 && c.accel_value_deviation <= eps) { nHit = c.n; break; }
            }
            rows.push({ algo: k, n: nHit });
        }
        return rows;
    }, [items, eps]);

    return (
        <ResponsiveContainer width="100%" height={360}>
            <BarChart data={data} layout="vertical">
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis type="number" dataKey="n" />
                <YAxis type="category" dataKey="algo" width={220} />
                <Tooltip />
                <Legend />
                <Bar dataKey="n" name={`n\u2091 (ε=${eps})`} fill="#777" >
                    {data.map((entry, idx) => (
                        <rect key={idx} />
                    ))}
                </Bar>
            </BarChart>
        </ResponsiveContainer>
    );
}
