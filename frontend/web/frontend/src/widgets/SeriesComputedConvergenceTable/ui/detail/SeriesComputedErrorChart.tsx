import React, { useMemo, useState } from "react";
import type { DetailPoint } from "../../model/types";
import {
    CartesianGrid,
    Line,
    LineChart,
    ReferenceLine,
    ResponsiveContainer,
    Tooltip as RechartsTooltip,
    XAxis,
    YAxis,
} from "recharts";
import type { TooltipProps } from "recharts/types/component/Tooltip";

type AnyNum = number | null | undefined;

interface ErrorChartPoint {
    n: number;
    value: number;
}

interface SeriesComputedErrorChartProps {
    points: DetailPoint[];
    useAbs: boolean;
}

const isFiniteNumber = (v: unknown): v is number => typeof v === "number" && Number.isFinite(v);

const formatValue = (value: AnyNum): string => {
    if (!isFiniteNumber(value)) return "—";
    const v = value as number;
    if (Math.abs(v) >= 1e5 || Math.abs(v) < 1e-4) return v.toExponential(6);
    return v.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
};

interface ErrorTooltipProps extends TooltipProps<number, string> {
    labelText: string;
}

const ErrorTooltip: React.FC<ErrorTooltipProps> = (props) => {
    const { active, label, payload, labelText } = props;
    if (!active || !payload || payload.length === 0) return null;

    const first = payload[0];
    const raw = first?.value;
    const val = isFiniteNumber(raw) ? (raw as number) : null;

    return (
        <div className="rounded-xl border border-border bg-panel/95 p-3 text-xs shadow-lg backdrop-blur">
            <div className="mb-2 font-semibold">n = {label}</div>
            <div className="flex items-center gap-2">
                <span className="text-[11px] text-textDim/80">{labelText} ≈</span>
                <span className="font-mono text-[11px] tabular-nums">{formatValue(val)}</span>
            </div>
        </div>
    );
};

export const SeriesComputedErrorChart: React.FC<SeriesComputedErrorChartProps> = ({
    points,
    useAbs,
}) => {
    const data: ErrorChartPoint[] = useMemo(
        () =>
            points
                .filter((p) => isFiniteNumber(p.err) && (p.err as number) >= 0)
                .map((p) => {
                    const base = p.err as number;
                    const signed = !useAbs && p.sign != null && p.sign !== 0 ? base * p.sign : base;
                    return { n: p.n, value: signed };
                }),
        [points, useAbs]
    );

    const { yMin, yMax } = useMemo(() => {
        if (data.length === 0) return { yMin: 0, yMax: 0 };

        let min = data[0].value;
        let max = data[0].value;

        for (const d of data) {
            if (d.value < min) min = d.value;
            if (d.value > max) max = d.value;
        }

        if (min === max) {
            const base = Math.abs(min) || 1;
            const pad = base * 0.1;
            return { yMin: min - pad, yMax: min + pad };
        }

        const span = max - min;
        const pad = span * 0.1;
        return { yMin: min - pad, yMax: max + pad };
    }, [data]);

    const [lineMode, setLineMode] = useState<"smooth" | "sharp">("smooth");

    if (data.length === 0) {
        return (
            <div className="mb-3 text-[11px] text-amber-300/80">
                Невозможно построить график ошибки: нет корректных значений ||Sₙ − S||.
            </div>
        );
    }

    const labelY = useAbs ? "|Sₙ − S|" : "sgn·|Sₙ − S|";

    return (
        <div className="mb-3">
            <div className="mb-1 flex justify-end gap-1 text-[10px] text-textDim/80">
                <span className="mr-1">Соединение:</span>
                <button
                    type="button"
                    className={
                        "rounded border px-2 py-[1px]" +
                        (lineMode === "smooth"
                            ? " border-primary bg-primary/20"
                            : " border-border bg-surface")
                    }
                    onClick={() => setLineMode("smooth")}
                >
                    плавное
                </button>
                <button
                    type="button"
                    className={
                        "rounded border px-2 py-[1px]" +
                        (lineMode === "sharp"
                            ? " border-primary bg-primary/20"
                            : " border-border bg-surface")
                    }
                    onClick={() => setLineMode("sharp")}
                >
                    резкое
                </button>
            </div>

            <div className="relative w-full" style={{ height: 220 }}>
                <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={data}>
                        <CartesianGrid strokeDasharray="3 3" />
                        <XAxis
                            dataKey="n"
                            tick={{ fontSize: 10 }}
                            label={{
                                value: "n",
                                position: "insideBottomRight",
                                offset: -4,
                                fontSize: 10,
                            }}
                        />
                        <YAxis
                            domain={[yMin, yMax]}
                            tick={{ fontSize: 10 }}
                            tickFormatter={(v: number) => formatValue(v)}
                            label={{
                                value: labelY,
                                angle: -90,
                                position: "insideLeft",
                                fontSize: 10,
                            }}
                        />
                        <RechartsTooltip
                            content={<ErrorTooltip labelText={labelY} />}
                            wrapperStyle={{ zIndex: 50 }}
                        />
                        {!useAbs && (
                            <ReferenceLine
                                y={0}
                                stroke="red"
                                strokeWidth={1.5}
                                strokeDasharray="8 8"
                            />
                        )}
                        <Line
                            type={lineMode === "smooth" ? "monotone" : "linear"}
                            dataKey="value"
                            name={labelY}
                            stroke="#22c55e"
                            strokeWidth={2}
                            dot={{ r: 4 }}
                            connectNulls
                        />
                    </LineChart>
                </ResponsiveContainer>
            </div>

            <div className="mt-1 text-[10px] text-textDim/70">График {labelY} по n.</div>
        </div>
    );
};
