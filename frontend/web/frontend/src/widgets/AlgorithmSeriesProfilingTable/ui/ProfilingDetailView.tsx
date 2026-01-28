import React, { useMemo, useState } from "react";
import type { SelectedDetail, ProfilingPoint } from "../model/types";
import { formatTrendShort } from "../model/profilingUtils";
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

/* ======================= utils ======================= */

const isNum = (v: unknown): v is number => typeof v === "number" && Number.isFinite(v);

const getAllKinds = (points: ProfilingPoint[]): string[] => {
    const s = new Set<string>();
    for (const p of points) for (const k of Object.keys(p.opsByKind ?? {})) s.add(k);
    return Array.from(s);
};

const toChartData = (points: ProfilingPoint[], kinds: string[]): Array<Record<string, number | null>> => {
    return points.map((p) => {
        const row: Record<string, number | null> = {
            n: p.n,
            totalOps: isNum(p.totalOps) ? p.totalOps : null,
        };
        for (const k of kinds) row[k] = isNum(p.opsByKind?.[k]) ? (p.opsByKind[k] as number) : null;
        return row;
    });
};

const toDiffChartData = (points: ProfilingPoint[], kinds: string[]): Array<Record<string, number | null>> => {
    return points.map((p) => {
        const row: Record<string, number | null> = {
            n: p.n,
            diffTotalOps: isNum(p.diffTotalOps) ? p.diffTotalOps : null,
        };
        for (const k of kinds) row[`d:${k}`] = isNum(p.diffByKind?.[k]) ? (p.diffByKind[k] as number) : null;
        return row;
    });
};

/* ======================= tooltip ======================= */

type AnyRow = Record<string, unknown>;

const TooltipBox: React.FC<{ label: string; items: Array<{ name: string; value: unknown }> }> = ({
                                                                                                     label,
                                                                                                     items,
                                                                                                 }) => {
    return (
        <div className="rounded border border-border bg-panel px-2 py-1 shadow-panel">
            <div className="mb-1 text-[11px] text-textDim/80">{label}</div>
            <div className="space-y-[2px] font-mono text-[11px] tabular-nums">
                {items.map((it) => (
                    <div key={it.name} className="flex justify-between gap-3">
                        <span className="text-textDim/80">{it.name}</span>
                        <span>{isNum(it.value) ? it.value : "∅"}</span>
                    </div>
                ))}
            </div>
        </div>
    );
};

const TotalOpsTooltip: React.FC<TooltipProps<number, string>> = ({ active, payload, label }) => {
    if (!active || !payload || payload.length === 0) return null;
    const row = (payload[0]?.payload ?? {}) as AnyRow;

    const n = row["n"];
    const total = row["totalOps"];

    return <TooltipBox label={`n = ${String(n)}`} items={[{ name: "total", value: total }]} />;
};

const BreakdownTooltip: React.FC<TooltipProps<number, string>> = ({ active, payload, label }) => {
    if (!active || !payload || payload.length === 0) return null;

    const row = (payload[0]?.payload ?? {}) as AnyRow;
    const n = row["n"];

    const items = payload
        .map((p) => ({ name: String(p.name ?? p.dataKey), value: (p.payload as AnyRow)?.[String(p.dataKey)] }))
        .filter((x) => x.name !== "n");

    return <TooltipBox label={`n = ${String(n)}`} items={items} />;
};

/* ======================= components ======================= */

const TotalOpsChart: React.FC<{ points: ProfilingPoint[] }> = ({ points }) => {
    const data = useMemo(() => toChartData(points, []), [points]);

    return (
        <div className="rounded-lg border border-border bg-surface/30 p-3">
            <div className="mb-2 text-xs text-textDim/80">График: total operations T(n)</div>
            <div className="h-[260px] w-full">
                <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={data} margin={{ top: 8, right: 16, bottom: 8, left: 0 }}>
                        <CartesianGrid strokeDasharray="3 3" />
                        <XAxis dataKey="n" tick={{ fontSize: 11 }} />
                        <YAxis tick={{ fontSize: 11 }} />
                        <RechartsTooltip content={<TotalOpsTooltip />} />
                        <Line
                            type="monotone"
                            dataKey="totalOps"
                            name="total"
                            dot={false}
                            strokeWidth={2}
                            isAnimationActive={false}
                        />
                    </LineChart>
                </ResponsiveContainer>
            </div>
        </div>
    );
};

const OP_COLOR: Record<string, string> = {
    add: "#22c55e",     // green
    mul: "#3b82f6",     // blue
    div: "#f59e0b",     // amber
    special: "#a855f7", // purple
};

const getOpColor = (k: string): string => OP_COLOR[k] ?? "#94a3b8"; // fallback


const OpsBreakdownChart: React.FC<{
    points: ProfilingPoint[];
    allKinds: string[];
    selectedKinds: string[];
    onSelectedKindsChange: (k: string[]) => void;
}> = ({ points, allKinds, selectedKinds, onSelectedKindsChange }) => {
    const data = useMemo(() => toChartData(points, selectedKinds), [points, selectedKinds]);

    const toggle = (k: string) => {
        if (selectedKinds.includes(k)) onSelectedKindsChange(selectedKinds.filter((x) => x !== k));
        else onSelectedKindsChange([...selectedKinds, k]);
    };

    return (
        <div className="mt-3 rounded-lg border border-border bg-surface/30 p-3">
            <div className="mb-2 flex flex-wrap items-center justify-between gap-2">
                <div className="text-xs text-textDim/80">График: операции по типам Oₖ(n)</div>

                <div className="flex flex-wrap gap-2">
                    {allKinds.map((k) => {
                        const on = selectedKinds.includes(k);
                        const c = getOpColor(k);
                        return (
                            <button
                                key={k}
                                type="button"
                                className={
                                    "rounded border px-2 py-[1px] text-[10px] font-mono " +
                                    (on
                                        ? "border-primary bg-primary/20 text-textDim"
                                        : "border-border bg-surface text-textDim/80 hover:bg-panel")
                                }
                                style={on ? { borderColor: c, backgroundColor: `${c}33` } : { borderColor: c }}
                                onClick={() => toggle(k)}
                            >
                                {k}
                            </button>
                        );
                    })}
                </div>
            </div>

            <div className="h-[260px] w-full">
                <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={data} margin={{ top: 8, right: 16, bottom: 8, left: 0 }}>
                        <CartesianGrid strokeDasharray="3 3" />
                        <XAxis dataKey="n" tick={{ fontSize: 11 }} />
                        <YAxis tick={{ fontSize: 11 }} />
                        <RechartsTooltip content={<BreakdownTooltip />} />

                        {selectedKinds.map((k) => (
                            <Line
                                key={k}
                                type="monotone"
                                dataKey={k}
                                name={k}
                                dot={false}
                                strokeWidth={2}
                                stroke={getOpColor(k)}
                                isAnimationActive={false}
                            />
                        ))}
                    </LineChart>
                </ResponsiveContainer>
            </div>
        </div>
    );
};

const OpsTable: React.FC<{ points: ProfilingPoint[]; kinds: string[] }> = ({ points, kinds }) => {
    return (
        <div className="mt-3 rounded-lg border border-border bg-surface/30 p-3">
            <div className="mb-2 text-xs text-textDim/80">Таблица: T(n) и Oₖ(n) (первые 200)</div>
            <div className="max-h-[360px] overflow-auto">
                <table className="w-full border-collapse text-[11px] text-textDim">
                    <thead className="sticky top-0 bg-panel">
                    <tr>
                        <th className="border-b border-border px-2 py-1 text-left">n</th>
                        <th className="border-b border-border px-2 py-1 text-left">T(n)</th>
                        {kinds.map((k) => (
                            <th key={k} className="border-b border-border px-2 py-1 text-left">
                                {k}
                            </th>
                        ))}
                    </tr>
                    </thead>
                    <tbody>
                    {points.slice(0, 200).map((p) => (
                        <tr key={p.n} className="odd:bg-surface/40 even:bg-surface/20">
                            <td className="border-t border-border px-2 py-1 font-mono">{p.n}</td>
                            <td className="border-t border-border px-2 py-1 font-mono">
                                {isNum(p.totalOps) ? p.totalOps : "∅"}
                            </td>
                            {kinds.map((k) => {
                                const v = p.opsByKind?.[k];
                                return (
                                    <td key={k} className="border-t border-border px-2 py-1 font-mono">
                                        {isNum(v) ? v : "∅"}
                                    </td>
                                );
                            })}
                        </tr>
                    ))}
                    </tbody>
                </table>
            </div>
        </div>
    );
};

const DiffTable: React.FC<{ points: ProfilingPoint[]; kinds: string[] }> = ({ points, kinds }) => {
    const rows = points.filter((p) => p.diffTotalOps != null || Object.values(p.diffByKind ?? {}).some((v) => v != null));

    return (
        <div className="mt-3 rounded-lg border border-border bg-surface/30 p-3">
            <div className="mb-2 text-xs text-textDim/80">Таблица разностей: ΔT и ΔOₖ (первые 200)</div>
            <div className="max-h-[360px] overflow-auto">
                <table className="w-full border-collapse text-[11px] text-textDim">
                    <thead className="sticky top-0 bg-panel">
                    <tr>
                        <th className="border-b border-border px-2 py-1 text-left">n</th>
                        <th className="border-b border-border px-2 py-1 text-left">ΔT</th>
                        {kinds.map((k) => (
                            <th key={k} className="border-b border-border px-2 py-1 text-left">
                                Δ{k}
                            </th>
                        ))}
                    </tr>
                    </thead>
                    <tbody>
                    {rows.slice(0, 200).map((p) => {
                        const dt = p.diffTotalOps;
                        const dtNeg = isNum(dt) && dt < 0;
                        return (
                            <tr key={p.n} className="odd:bg-surface/40 even:bg-surface/20">
                                <td className="border-t border-border px-2 py-1 font-mono">{p.n}</td>
                                <td
                                    className={
                                        "border-t border-border px-2 py-1 font-mono " +
                                        (dtNeg ? "text-red-300" : "")
                                    }
                                >
                                    {isNum(dt) ? dt : "∅"}
                                </td>

                                {kinds.map((k) => {
                                    const dv = p.diffByKind?.[k];
                                    const neg = isNum(dv) && dv < 0;
                                    return (
                                        <td
                                            key={k}
                                            className={
                                                "border-t border-border px-2 py-1 font-mono " +
                                                (neg ? "text-red-300" : "")
                                            }
                                        >
                                            {isNum(dv) ? dv : "∅"}
                                        </td>
                                    );
                                })}
                            </tr>
                        );
                    })}
                    </tbody>
                </table>
            </div>
        </div>
    );
};

/* ======================= main view ======================= */

export const ProfilingDetailView: React.FC<{ detail: SelectedDetail }> = ({ detail }) => {
    const { seriesInfo, algoInfo, series, accel, analysis, points } = detail;

    const allKinds = useMemo(() => {
        // если analysis уже содержит порядок (topKinds / maxByKind), используем его
        if (analysis?.maxByKind) {
            return Object.keys(analysis.maxByKind).sort((a, b) => {
                const ma = analysis.maxByKind[a]?.max ?? 0;
                const mb = analysis.maxByKind[b]?.max ?? 0;
                return mb - ma;
            });
        }
        return getAllKinds(points);
    }, [analysis, points]);

    const [selectedKinds, setSelectedKinds] = useState<string[]>(
        analysis?.topKinds?.length ? analysis.topKinds : allKinds
    );

    // если пришли новые analysis/points (смена ячейки), привести selection к актуальному множеству
    useMemo(() => {
        setSelectedKinds((prev) => {
            const allowed = new Set(allKinds);
            const filtered = prev.filter((k) => allowed.has(k));
            if (filtered.length > 0) return filtered;
            return analysis?.topKinds?.length ? analysis.topKinds : allKinds;
        });
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [analysis?.accelId, analysis?.seriesId, allKinds.join("|")]);

    if (!analysis) return null;

    const maxText =
        analysis.maxTotalOps != null && analysis.maxTotalOpsN != null
            ? `${analysis.maxTotalOps}`
            : "∅";

    const trendShort = formatTrendShort(analysis.trendTotal);

    const seriesLabel =
        seriesInfo?.seriesName ??
        series?.name ??
        "—";

    const algoLabel =
        algoInfo?.algorithmName ??
        accel?.name ??
        "—";

    return (
        <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
            <div className="mb-3 flex items-start justify-between gap-4">
                <div className="min-w-0">
                    <div className="text-sm font-semibold text-textDim">Profiling: операции</div>
                    <div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">
                        <div>Ряд: {seriesLabel}</div>
                        <div>
                            Алгоритм: {algoLabel}
                            {(algoInfo?.m ?? accel?.m) != null ? `, m=${String(algoInfo?.m ?? accel?.m)}` : ""}
                        </div>
                        {algoInfo?.argsSummary ? <div>Аргументы: {algoInfo.argsSummary}</div> : null}
                    </div>
                </div>

                <div className="space-y-1 text-right text-[11px] text-textDim/80">
                    <div>Trend(T): {trendShort}</div>
                    <div>Max(T): {maxText}</div>
                    <div>Сравнено шагов (пар): {analysis.stepsAnalyzed}</div>
                </div>
            </div>

            <TotalOpsChart points={points} />
            <OpsBreakdownChart
                points={points}
                allKinds={allKinds}
                selectedKinds={selectedKinds}
                onSelectedKindsChange={setSelectedKinds}
            />
            <OpsTable points={points} kinds={selectedKinds} />
            <DiffTable points={points} kinds={selectedKinds} />
        </div>
    );
};
