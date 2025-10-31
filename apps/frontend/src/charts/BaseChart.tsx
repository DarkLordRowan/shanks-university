// src/charts/BaseChart.tsx
import React from "react";
import {
    ResponsiveContainer,
    LineChart,
    Line,
    XAxis,
    YAxis,
    Tooltip,
    Legend,
    CartesianGrid,
    ReferenceLine,
} from "recharts";
import { colorByKey } from "./common";
import { Formula } from "../components/Formula.tsx";


type RefLine = {
    y: number;
    label?: string;
    stroke?: string;
    dash?: string;
};

interface BaseChartProps {
    data: Array<Record<string, number | null>>;
    seriesKeys: string[];
    yAxisLabel?: string;
    refLines?: RefLine[];
    height?: number;
    title?: string;
    formula?: string | React.ReactNode; // при наличии KaTeX можно передать узел
    tooltipFormatter?: (value: any, name: string, entry: any) => [string | number, string];
}

export function BaseChart({
                              data,
                              seriesKeys,
                              yAxisLabel,
                              refLines,
                              height = 420,
                              title,
                              formula,
                              tooltipFormatter,
                          }: BaseChartProps) {
    return (
        <div className="w-full">
            {title && <div className="mb-1 text-sm font-medium">{title}</div>}

            {formula && (
                <div
                    className="mb-3 rounded-xl px-3 py-2 text-sm  border border-gray-200"
                >
                    {typeof formula === "string"
                        ? <Formula latex={formula} />
                        : formula}
                </div>
            )}

            <ResponsiveContainer width="100%" height={height}>
                <LineChart data={data}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="n" />
                    <YAxis label={yAxisLabel ? { value: yAxisLabel, angle: -90, position: "insideLeft" } : undefined} />
                    <Tooltip formatter={tooltipFormatter} />
                    <Legend />
                    {refLines?.map((r, i) => (
                        <ReferenceLine
                            key={i}
                            y={r.y}
                            stroke={r.stroke ?? "#666"}
                            strokeDasharray={r.dash ?? "4 4"}
                            ifOverflow="extendDomain"
                            label={r.label ? { value: r.label, position: "right", fill: r.stroke ?? "#666" } : undefined}
                        />
                    ))}
                    {seriesKeys.map(k => (
                        <Line
                            key={k}
                            type="monotone"
                            dataKey={k}
                            name={k}
                            dot={false}
                            strokeWidth={2}
                            stroke={colorByKey(k)}
                            connectNulls
                        />
                    ))}
                </LineChart>
            </ResponsiveContainer>
        </div>
    );
}
