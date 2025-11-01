import React, { useMemo } from "react";
import {
    CartesianGrid,
    Line,
    LineChart,
    ReferenceLine,
    ResponsiveContainer,
    XAxis,
    YAxis,
    Tooltip as RechartsTooltip,
} from "recharts";
import type { TooltipProps } from "recharts";
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
    formula?: string | React.ReactNode;
    /** Не используется стандартным Tooltip; для совместимости можно передать,
     *  но форматирование выполняет кастомный tooltip ниже. */
    tooltipFormatter?: (value: any, name: string) => [string | number, string];
}

/* ---------- Вспомогательные функции ---------- */

type AnyNum = number | null | undefined;

function defaultFormat(value: AnyNum): string {
    if (value == null || !Number.isFinite(value as number)) return "—";
    const v = value as number;
    // компактное представление
    if (Math.abs(v) >= 1e5 || Math.abs(v) < 1e-4) return v.toExponential(6);
    return v.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
}

/* ---------- Кастомный Tooltip с сортировкой ---------- */

type SortedTooltipProps = {
    format?: (value: AnyNum, name: string) => string;
};

const SortedTooltip: React.FC<TooltipProps<number, string> & SortedTooltipProps> = ({
                                                                                        active,
                                                                                        label,
                                                                                        payload,
                                                                                        format,
                                                                                    }) => {
    if (!active || !payload || payload.length === 0) return null;

    const items = payload
        .map(p => ({
            name: String(p.name ?? p.dataKey ?? ""),
            value: typeof p.value === "number" && Number.isFinite(p.value) ? (p.value as number) : null,
            color: (p.color as string) || colorByKey(String(p.name ?? p.dataKey ?? "")),
        }))
        .sort((a, b) => {
            if (a.value == null && b.value == null) return a.name.localeCompare(b.name);
            if (a.value == null) return 1;
            if (b.value == null) return -1;
            return b.value - a.value; // по убыванию
        });

    return (
        <div
            className="
        z-[9999] max-h-[70vh] w-[min(560px,48vw)] overflow-auto
        rounded-xl border border-gray-200 bg-white/95 p-3 shadow-lg backdrop-blur
        dark:border-neutral-700 dark:bg-neutral-900/95
      "
            style={{ pointerEvents: "none" }}
        >
            <div className="mb-2 text-xs font-semibold">n = {label}</div>
            <ul className="space-y-1 text-xs leading-5">
                {items.map(it => (
                    <li key={it.name} className="flex items-center gap-2">
            <span
                className="inline-block h-2.5 w-2.5 rounded-full"
                style={{ background: it.color }}
                aria-hidden
            />
                        <span className="whitespace-pre-wrap break-words">{it.name}</span>
                        <span className="ml-auto tabular-nums">
              {format ? format(it.value, it.name) : defaultFormat(it.value)}
            </span>
                    </li>
                ))}
            </ul>
        </div>
    );
};

/* ---------- Сам график ---------- */

export function BaseChart({
                              data,
                              seriesKeys,
                              yAxisLabel,
                              refLines,
                              height = 420,
                              title,
                              formula,
                              tooltipFormatter, // совместимость
                          }: BaseChartProps) {
    // ключи можно не сортировать: рендерим линии как есть; тултип сортирует динамически
    const keys = useMemo(() => [...seriesKeys], [seriesKeys]);

    return (
        <div className="w-full">
            {title && <div className="mb-1 text-sm font-medium">{title}</div>}

            {formula && (
                <div className="mb-3 rounded-xl px-3 py-2 text-sm border border-gray-200">
                    {typeof formula === "string" ? <Formula latex={formula} /> : formula}
                </div>
            )}

            <div className="relative w-full" style={{ height }}>
                <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={data}>
                        <CartesianGrid strokeDasharray="3 3" />
                        <XAxis dataKey="n" />
                        <YAxis
                            label={
                                yAxisLabel ? { value: yAxisLabel, angle: -90, position: "insideLeft" } : undefined
                            }
                        />

                        {/* Кастомный тултип с сортировкой и высоким z-index */}
                        <RechartsTooltip
                            content={<SortedTooltip format={(v, n) => (tooltipFormatter ? String(tooltipFormatter(v, n)[0]) : defaultFormat(v))} />}
                            wrapperStyle={{ zIndex: 9999 }}
                        />

                        {/* Оставляем стандартную Legend или уберите при необходимости */}
                        {/* <Legend /> */}

                        {refLines?.map((r, i) => (
                            <ReferenceLine
                                key={i}
                                y={r.y}
                                stroke={r.stroke ?? "#666"}
                                strokeDasharray={r.dash ?? "4 4"}
                                ifOverflow="extendDomain"
                                label={
                                    r.label
                                        ? { value: r.label, position: "right", fill: r.stroke ?? "#666" }
                                        : undefined
                                }
                            />
                        ))}

                        {keys.map(k => (
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
        </div>
    );
}
