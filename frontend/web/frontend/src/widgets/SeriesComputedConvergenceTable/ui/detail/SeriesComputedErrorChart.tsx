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

const isFiniteNumber = (value: unknown): value is number =>
    typeof value === "number" && Number.isFinite(value);

const formatValue = (value: AnyNum): string => {
    if (!isFiniteNumber(value)) return "—";
    if (Math.abs(value) >= 1e5 || Math.abs(value) < 1e-4) return value.toExponential(6);
    return value.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
};

interface ErrorTooltipProps extends TooltipProps<number, string> {
    labelText: string;
}

const ErrorTooltip: React.FC<ErrorTooltipProps> = ({ active, label, payload, labelText }) => {
    if (!active || !payload || payload.length === 0) return null;

    const raw = payload[0]?.value;
    const value = isFiniteNumber(raw) ? raw : null;

    return (
        <div className="rounded-xl border border-border bg-panel/95 p-3 text-xs shadow-lg backdrop-blur">
            <div className="mb-2 font-semibold">n = {label}</div>
            <div className="flex items-center gap-2">
                <span className="text-[11px] text-textDim/80">{labelText} ≈</span>
                <span className="font-mono text-[11px] tabular-nums">{formatValue(value)}</span>
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
                .filter((point) => isFiniteNumber(point.err) && point.err >= 0)
                .map((point) => {
                    const base = point.err as number;
                    const signed =
                        !useAbs && point.sign != null && point.sign !== 0 ? base * point.sign : base;
                    return { n: point.n, value: signed };
                }),
        [points, useAbs]
    );

    const { yMin, yMax } = useMemo(() => {
        if (data.length === 0) return { yMin: 0, yMax: 0 };

        let min = data[0].value;
        let max = data[0].value;

        for (const point of data) {
            if (point.value < min) min = point.value;
            if (point.value > max) max = point.value;
        }

        if (min === max) {
            const pad = (Math.abs(min) || 1) * 0.1;
            return { yMin: min - pad, yMax: min + pad };
        }

        const pad = (max - min) * 0.1;
        return { yMin: min - pad, yMax: max + pad };
    }, [data]);

    const [lineMode, setLineMode] = useState<"smooth" | "sharp">("smooth");
    const [scaleMode, setScaleMode] = useState<"linear" | "symlog">("linear");

    if (data.length === 0) {
        return (
            <div className="mb-3 text-[11px] text-amber-300/80">
                Невозможно построить график ошибки: нет корректных значений |S_n - S|.
            </div>
        );
    }

    const labelY = useAbs ? "|S_n - S|" : "sgn·|S_n - S|";

    return (
        <div className="mb-3">
            <div className="mb-1 flex flex-wrap justify-end gap-2 text-[10px] text-textDim/80">
                <div className="flex items-center gap-1">
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

                <div className="flex items-center gap-1">
                    <span className="mr-1">Y:</span>
                    <button
                        type="button"
                        className={
                            "rounded border px-2 py-[1px]" +
                            (scaleMode === "linear"
                                ? " border-primary bg-primary/20"
                                : " border-border bg-surface")
                        }
                        onClick={() => setScaleMode("linear")}
                    >
                        linear
                    </button>
                    <button
                        type="button"
                        className={
                            "rounded border px-2 py-[1px]" +
                            (scaleMode === "symlog"
                                ? " border-primary bg-primary/20"
                                : " border-border bg-surface")
                        }
                        onClick={() => setScaleMode("symlog")}
                    >
                        symlog
                    </button>
                </div>
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
                            scale={scaleMode}
                            domain={[yMin, yMax]}
                            tick={{ fontSize: 10 }}
                            tickFormatter={(value: number) => formatValue(value)}
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
                        {!useAbs ? (
                            <ReferenceLine
                                y={0}
                                stroke="red"
                                strokeWidth={1.5}
                                strokeDasharray="8 8"
                            />
                        ) : null}
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

            <div className="mt-1 text-[10px] text-textDim/70">
                График {labelY} по n. Режим `symlog` удобен, когда в одном ряду смешаны очень
                маленькие и очень большие порядки.
            </div>
        </div>
    );
};
