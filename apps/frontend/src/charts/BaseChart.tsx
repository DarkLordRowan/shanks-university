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

    /** Совместимость: если нужно стороннее форматирование значений. */
    tooltipFormatter?: (value: AnyNum, name: string) => [string | number, string];

    /** Панель выбора серий. */
    controlsPlacement?: "side" | "top" | "none";
    controlsWidthPx?: number;
    controlsCollapsible?: boolean;
    controlsDefaultCollapsed?: boolean;
}

/* ======================= utils ======================= */

const isFiniteNumber = (v: unknown): v is number =>
    typeof v === "number" && Number.isFinite(v);

const defaultFormat = (value: AnyNum): string => {
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

const sortKeysByLastValueDesc = (data: DataPoint[], keys: string[]) =>
    [...keys].sort((a, b) => {
        const va = lastDefined(data, a);
        const vb = lastDefined(data, b);
        if (va == null && vb == null) return a.localeCompare(b);
        if (va == null) return 1;
        if (vb == null) return -1;
        return vb - va;
    });

const buildVisibleKeys = (all: string[], hidden: Set<string>): Set<string> => {
    const s = new Set<string>();
    for (const k of all) if (!hidden.has(k)) s.add(k);
    return s;
};

/** Позиционирование тултипа: не перекрывать вертикаль при текущем n. */
const computeTooltipXY = (
    wrapW: number,
    mouseX: number,
    measuredTipW?: number,
    margin = 12
) => {
    const maxW = Math.floor(wrapW * 0.48);
    const w = Math.min(measuredTipW ?? 520, maxW);
    const clamp = (v: number, lo: number, hi: number) => Math.max(lo, Math.min(v, hi));
    const xLeft = clamp(mouseX - w - margin, margin, wrapW - w - margin);
    const xRight = clamp(mouseX + margin, margin, wrapW - w - margin);
    const x = mouseX < wrapW / 2 ? xRight : xLeft;
    const y = margin;
    return { x, y, tooltipW: w };
};

/* ======================= SortedTooltip ======================= */

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
        overflow-auto
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
                aria-hidden
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

/* ======================= ControlsPanel ======================= */

interface ControlsProps {
    data: DataPoint[];
    seriesKeys: string[];
    hidden: Set<string>;
    onToggle: (k: string) => void;
    onShowAll: () => void;
    onHideAll: () => void;
    maxHeight?: number | string;
}

const ControlsPanel: React.FC<ControlsProps> = memo(function ControlsPanel({
                                                                               data,
                                                                               seriesKeys,
                                                                               hidden,
                                                                               onToggle,
                                                                               onShowAll,
                                                                               onHideAll,
                                                                               maxHeight,
                                                                           }) {
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
                                {v == null ? "—" : defaultFormat(v)}
                            </span>
                        </li>
                    );
                })}
            </ul>
        </div>
    );
});

/* ======================= BaseChart ======================= */

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
                              controlsCollapsible = true,
                              controlsDefaultCollapsed = false,
                          }: BaseChartProps) {
    /* видимость серий */
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

    /* панель выбора: сворачивание */
    const [controlsCollapsed, setControlsCollapsed] = useState<boolean>(
        Boolean(controlsDefaultCollapsed)
    );
    const toggleControls = useCallback(() => {
        if (!controlsCollapsible) return;
        setControlsCollapsed((v) => !v);
    }, [controlsCollapsible]);

    /* измерение области и координата курсора */
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

    /* ширина тултипа и позиция */
    const [measuredTipW, setMeasuredTipW] = useState<number | undefined>(undefined);
    const { x: tooltipX, y: tooltipY } = computeTooltipXY(wrapW, mouseX, measuredTipW);

    /* ключи линий (без сортировки; сортировка только в тултипе/панели) */
    const keys = useMemo(() => [...seriesKeys], [seriesKeys]);

    /* части разметки */
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
                        content={
                            <SortedTooltip
                                format={(v, n) =>
                                    tooltipFormatter ? String(tooltipFormatter(v, n)[0]) : defaultFormat(v)
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

                    {keys.map((k) => (
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
            {/* Заголовок + кнопка сворачивания панели */}
            <div className="mb-1 flex items-center gap-2">
                {title ? <div className="text-sm font-medium">{title}</div> : null}
                <div className="ml-auto">
                    {controlsPlacement !== "none" && controlsCollapsible && (
                        <button
                            type="button"
                            onClick={toggleControls}
                            aria-pressed={controlsCollapsed ? "true" : "false"}
                            aria-label={controlsCollapsed ? "Показать панель" : "Скрыть панель"}
                            title={controlsCollapsed ? "Показать панель" : "Скрыть панель"}
                            className="rounded-md border px-2 py-0.5 text-xs"
                        >
                            {controlsCollapsed
                                ? "Панель выбора графиков: показать"
                                : "Панель выбора графиков: скрыть"}
                        </button>
                    )}
                </div>
            </div>

            {formula && (
                <div className="mb-3 rounded-xl px-3 py-2 text-sm border border-gray-200">
                    {typeof formula === "string" ? <Formula latex={formula} /> : formula}
                </div>
            )}

            {controlsPlacement === "top" && !controlsCollapsed && (
                <div className="mb-3">{Controls}</div>
            )}

            {controlsPlacement === "side" ? (
                controlsCollapsed ? (
                    <div className="grid gap-3" style={{ gridTemplateColumns: `1fr` }}>
                        {ChartArea}
                    </div>
                ) : (
                    <div
                        className="grid gap-3"
                        style={{
                            gridTemplateColumns: `1fr minmax(180px, ${controlsWidthPx ?? 420}px)`,
                        }}
                    >
                        {ChartArea}
                        <div>{Controls}</div>
                    </div>
                )
            ) : (
                ChartArea
            )}
        </div>
    );
}
