import React, { useMemo } from "react";
import { type DetailPoint, type Complex } from "../../model/types";
import {
    CartesianGrid,
    Line,
    LineChart,
    ResponsiveContainer,
    Tooltip as RechartsTooltip,
    XAxis,
    YAxis,
} from "recharts";
import type { TooltipProps } from "recharts/types/component/Tooltip";

/* ======================= types ======================= */

type AnyNum = number | null | undefined;

interface ErrorChartPoint {
    n: number;
    err: number;
}

interface ConvergenceErrorChartProps {
    points: DetailPoint[];
    limit: Complex | null;
}

/* ======================= utils ======================= */

const isFiniteNumber = (v: unknown): v is number => typeof v === "number" && Number.isFinite(v);

/**
 * Форматирование числа:
 * - экспоненциальный формат для очень больших/малых;
 * - фиксированный с обрезкой хвоста нулей для остальных.
 */
const formatValue = (value: AnyNum): string => {
    if (!isFiniteNumber(value)) return "—";
    const v = value as number;
    if (Math.abs(v) >= 1e5 || Math.abs(v) < 1e-4) return v.toExponential(6);
    return v.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
};

/* ======================= Tooltip ======================= */

const ErrorTooltip: React.FC<TooltipProps<number, string>> = (props) => {
    const { active, label, payload } = props;
    if (!active || !payload || payload.length === 0) return null;

    const first = payload[0];
    const raw = first?.value;
    const val = isFiniteNumber(raw) ? (raw as number) : null;

    return (
        <div className="rounded-xl border border-border bg-panel/95 p-3 text-xs shadow-lg backdrop-blur">
            <div className="mb-2 font-semibold">n = {label}</div>
            <div className="flex items-center gap-2">
                <span className="text-[11px] text-textDim/80">|Aₙ − lim| ≈</span>
                <span className="font-mono text-[11px] tabular-nums">{formatValue(val)}</span>
            </div>
        </div>
    );
};

/* ======================= Chart ======================= */

export const ConvergenceErrorChart: React.FC<ConvergenceErrorChartProps> = ({ points, limit }) => {
    const data: ErrorChartPoint[] = useMemo(
        () =>
            points
                .filter((p) => isFiniteNumber(p.err) && (p.err as number) >= 0)
                .map((p) => ({
                    n: p.n,
                    err: p.err as number,
                })),
        [points]
    );

    // Диапазон по оси Y в пределах разброса ошибок + небольшой паддинг
    const { yMin, yMax } = useMemo(() => {
        if (data.length === 0) {
            return { yMin: 0, yMax: 0 };
        }

        let min = data[0].err;
        let max = data[0].err;

        for (const d of data) {
            if (d.err < min) min = d.err;
            if (d.err > max) max = d.err;
        }

        if (min === max) {
            // все точки одинаковы: делаем маленький симметричный интервал вокруг значения
            const base = Math.abs(min) || 1;
            const pad = base * 0.1;
            return { yMin: min - pad, yMax: min + pad };
        }

        const span = max - min;
        const pad = span * 0.1;
        return { yMin: min - pad, yMax: max + pad };
    }, [data]);

    if (!limit || data.length === 0) {
        return (
            <div className="mb-3 text-[11px] text-amber-300/80">
                Невозможно построить график ошибки: нет предела или нет корректных значений |Aₙ −
                lim|.
            </div>
        );
    }

    return (
        <div className="mb-3">
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
                                value: "|Aₙ − lim|",
                                angle: -90,
                                position: "insideLeft",
                                fontSize: 10,
                            }}
                        />
                        <RechartsTooltip content={<ErrorTooltip />} wrapperStyle={{ zIndex: 50 }} />
                        <Line
                            type="monotone"
                            dataKey="err"
                            name="|Aₙ − lim|"
                            stroke="#22c55e"
                            strokeWidth={2}
                            dot={{ r: 4 }}
                            connectNulls
                        />
                    </LineChart>
                </ResponsiveContainer>
            </div>
            <div className="mt-1 text-[10px] text-textDim/70">
                График |Aₙ − lim| по n. При наведении видно номер шага и значение ошибки.
            </div>
        </div>
    );
};
