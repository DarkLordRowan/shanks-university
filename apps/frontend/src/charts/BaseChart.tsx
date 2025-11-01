import React, {
    memo,
    useCallback,
    useLayoutEffect,
    useMemo,
    useRef,
    useState,
} from "react";
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
import type { TooltipProps } from "recharts/types/component/Tooltip";
import { colorByKey } from "./common";
import { Formula } from "../components/Formula";

/* ======================= types ======================= */

type AnyNum = number | null | undefined;
type DataPoint = Record<string, number | null>;

type RefLine = {
    y: number;
    label?: string;
    stroke?: string;
    dash?: string;
};

interface BaseChartProps {
    data: DataPoint[];
    seriesKeys: string[];
    yAxisLabel?: string;
    refLines?: RefLine[];
    height?: number;
    title?: string;
    formula?: string | React.ReactNode;
    tooltipFormatter?: (value: AnyNum, name: string) => [string | number, string];
    controlsPlacement?: "side" | "top" | "none";
    controlsWidthPx?: number;
}

/* ======================= utils ======================= */

const isFiniteNumber = (v: unknown): v is number =>
    typeof v === "number" && Number.isFinite(v);

const formatNumber = (value: AnyNum): string => {
    if (!isFiniteNumber(value)) return "—";
    const v = value as number;
    if (Math.abs(v) >= 1e5 || Math.abs(v) < 1e-4) return v.toExponential(6);
    return v.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
};

const lastDefined = (data: DataPoint[], key: string): number | null => {
    for (let i = data.length - 1; i >= 0; i--) {
        const v = data[i][key];
        if (isFiniteNumber(v)) return v;
    }
    return null;
};

const buildVisibleKeys = (all: string[], hidden: Set<string>): Set<string> => {
    const s = new Set<string>();
    for (const k of all) if (!hidden.has(k)) s.add(k);
    return s;
};

const sortKeysByLastValueDesc = (data: DataPoint[], keys: string[]): string[] =>
    [...keys].sort((a, b) => {
        const va = lastDefined(data, a);
        const vb = lastDefined(data, b);
        if (va == null && vb == null) return a.localeCompare(b);
        if (va == null) return 1;
        if (vb == null) return -1;
        return vb - va;
    });

/** Фиксированная оценка положения тултипа справа/слева. */
const computeTooltipXY = (wrapW: number, mouseX: number) => {
    const tooltipW = Math.min(520, Math.floor(wrapW * 0.48));
    const gutter = 12;
    const x =
        mouseX < wrapW * 0.55
            ? Math.max(gutter, wrapW - tooltipW - gutter)
            : gutter;
    const y = gutter;
    return { x, y, tooltipW };
};

/* ======================= Tooltip (sorted) ======================= */

type SortedTooltipExtra = {
    format?: (value: AnyNum, name: string) => string;
    visibleKeys: Set<string>;
};

const SortedTooltip = memo(function SortedTooltip(
    props: TooltipProps<number, string> & SortedTooltipExtra
) {
    const { active, label, payload, format, visibleKeys } = props;
    if (!active || !payload || payload.length === 0) return null;

    const items = payload
        .map((p) => {
            const name = String(p.name ?? p.dataKey ?? "");
            const value = isFiniteNumber(p.value) ? (p.value as number) : null;
            const color = (p.color as string) || colorByKey(name);
            return { name, value, color };
        })
        .filter((it) => visibleKeys.has(it.name))
        .sort((a, b) => {
            if (a.value == null && b.value == null) return a.name.localeCompare(b.name);
            if (a.value == null) return 1;
            if (b.value == null) return -1;
            return b.value - a.value;
        });

    if (items.length === 0) return null;

    return (
        <div
            className="
        z-[9999] max-h-[70vh] w-[min(520px,48vw)] overflow-auto
        rounded-xl border border-gray-200 bg-white/95 p-3 shadow-lg backdrop-blur
        dark:border-neutral-700 dark:bg-neutral-900/95
      "
            style={{ pointerEvents: "none" }}
        >
            <div className="mb-2 text-xs font-semibold">n = {label}</div>
            <ul className="space-y-1 text-xs leading-5">
                {items.map((it) => (
                    <li key={it.name} className="flex items-center gap-2">
            <span
                className="inline-block h-2.5 w-2.5 rounded-full"
                style={{ background: it.color }}
            />
                        <span className="whitespace-pre-wrap break-words">{it.name}</span>
                        <span className="ml-auto tabular-nums">
              {format ? format(it.value, it.name) : formatNumber(it.value)}
            </span>
                    </li>
                ))}
            </ul>
        </div>
    );
});

/* ======================= Controls panel ======================= */

interface ControlsProps {
    data: DataPoint[];
    seriesKeys: string[];
    hidden: Set<string>;
    onToggle: (k: string) => void;
    onShowAll: () => void;
    onHideAll: () => void;
    maxHeight?: number | string;
}

const ControlsPanel = memo(function ControlsPanel({
                                                      data,
                                                      seriesKeys,
                                                      hidden,
                                                      onToggle,
                                                      onShowAll,
                                                      onHideAll,
                                                      maxHeight,
                                                  }: ControlsProps) {
    const sortedKeys = useMemo(
        () => sortKeysByLastValueDesc(data, seriesKeys),
        [data, seriesKeys]
    );

    return (
        <div
            className="
        rounded-lg border border-gray-200 bg-white/95 p-3 shadow-md backdrop-blur
        dark:border-gray-700 dark:bg-neutral-900/95
      "
            style={{ maxHeight, overflow: "auto" }}
        >
            <div className="mb-2 flex items-center gap-2">
                <div className="text-xs font-semibold">Алгоритмы</div>
                <div className="ml-auto flex gap-2">
                    <button
                        type="button"
                        onClick={onShowAll}
                        className="rounded-md border px-2 py-0.5 text-xs"
                    >
                        Показать все
                    </button>
                    <button
                        type="button"
                        onClick={onHideAll}
                        className="rounded-md border px-2 py-0.5 text-xs"
                    >
                        Скрыть все
                    </button>
                </div>
            </div>

            <ul className="space-y-1 text-xs leading-5">
                {sortedKeys.map((k) => {
                    const v = lastDefined(data, k);
                    const isHidden = hidden.has(k);
                    return (
                        <li key={k} className="flex items-center gap-2">
                            <input
                                id={`chk-${k}`}
                                type="checkbox"
                                className="h-3.5 w-3.5 cursor-pointer"
                                checked={!isHidden}
                                onChange={() => onToggle(k)}
                            />
                            <span
                                className="inline-block h-2.5 w-2.5 rounded-full"
                                style={{ background: colorByKey(k), opacity: isHidden ? 0.35 : 1 }}
                            />
                            <label
                                htmlFor={`chk-${k}`}
                                className={`flex-1 cursor-pointer whitespace-pre-wrap break-words ${
                                    isHidden ? "opacity-50" : ""
                                }`}
                            >
                                {k}
                            </label>
                            <span className={`ml-2 tabular-nums ${isHidden ? "opacity-40" : ""}`}>
                {v == null ? "—" : formatNumber(v)}
              </span>
                        </li>
                    );
                })}
            </ul>
        </div>
    );
});

/* ======================= component ======================= */

export function BaseChart({
                              data,
                              seriesKeys,
                              yAxisLabel,
                              refLines,
                              height = 420,
                              title,
                              formula,
                              tooltipFormatter,
                              controlsPlacement = "side",
                              controlsWidthPx = 420,
                          }: BaseChartProps) {
    /* скрытие серий */
    const [hidden, setHidden] = useState<Set<string>>(() => new Set());
    const toggleKey = useCallback((k: string) => {
        setHidden((prev) => {
            const next = new Set(prev);
            next.has(k) ? next.delete(k) : next.add(k);
            return next;
        });
    }, []);
    const showAll = useCallback(() => setHidden(new Set()), []);
    const hideAll = useCallback(() => setHidden(new Set(seriesKeys)), [seriesKeys]);

    const visibleKeys = useMemo(
        () => buildVisibleKeys(seriesKeys, hidden),
        [seriesKeys, hidden]
    );

    /* измерение области для позиционирования тултипа */
    const wrapRef = useRef<HTMLDivElement | null>(null);
    const [wrapW, setWrapW] = useState(0);
    const [mouseX, setMouseX] = useState(0);

    useLayoutEffect(() => {
        const el = wrapRef.current;
        if (!el) return;
        const ro = new ResizeObserver((entries) => {
            const r = entries[0]?.contentRect;
            if (r) setWrapW(r.width);
        });
        ro.observe(el);
        return () => ro.disconnect();
    }, []);

    const handleMouseMove = useCallback((e: { chartX?: number }) => {
        if (typeof e?.chartX === "number") setMouseX(e.chartX);
    }, []);
    const handleMouseLeave = useCallback(() => setMouseX(0), []);

    const { x: tooltipX, y: tooltipY } = computeTooltipXY(wrapW, mouseX);

    /* --------- разметка --------- */
    const ChartArea = (
        <div ref={wrapRef} className="relative w-full" style={{ height }}>
            <ResponsiveContainer width="100%" height="100%">
                <LineChart
                    data={data}
                    onMouseMove={handleMouseMove}
                    onMouseLeave={handleMouseLeave}
                >
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="n" />
                    <YAxis
                        label={
                            yAxisLabel
                                ? { value: yAxisLabel, angle: -90, position: "insideLeft" }
                                : undefined
                        }
                    />

                    <RechartsTooltip
                        position={{ x: tooltipX, y: tooltipY }}
                        content={
                            <SortedTooltip
                                format={(v, n) =>
                                    tooltipFormatter ? String(tooltipFormatter(v, n)[0]) : formatNumber(v)
                                }
                                visibleKeys={visibleKeys}
                            />
                        }
                        wrapperStyle={{ zIndex: 9999 }}
                    />

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

                    {seriesKeys.map((k) => (
                        <Line
                            key={k}
                            type="monotone"
                            dataKey={k}
                            name={k}
                            dot={false}
                            strokeWidth={2}
                            stroke={colorByKey(k)}
                            connectNulls
                            hide={hidden.has(k)}
                        />
                    ))}
                </LineChart>
            </ResponsiveContainer>
        </div>
    );

    const Controls = (
        <ControlsPanel
            data={data}
            seriesKeys={seriesKeys}
            hidden={hidden}
            onToggle={toggleKey}
            onShowAll={showAll}
            onHideAll={hideAll}
            maxHeight={height}
        />
    );

    return (
        <div className="w-full">
            {title && <div className="mb-1 text-sm font-medium">{title}</div>}

            {formula && (
                <div className="mb-3 rounded-xl px-3 py-2 text-sm border border-gray-200">
                    {typeof formula === "string" ? <Formula latex={formula} /> : formula}
                </div>
            )}

            {controlsPlacement === "top" && <div className="mb-3">{Controls}</div>}

            {controlsPlacement === "side" ? (
                <div
                    className="grid gap-3"
                    style={{
                        gridTemplateColumns: `1fr minmax(260px, ${controlsWidthPx}px)`,
                    }}
                >
                    {ChartArea}
                    <div>{Controls}</div>
                </div>
            ) : (
                controlsPlacement === "none" ? ChartArea : ChartArea
            )}
        </div>
    );
}
