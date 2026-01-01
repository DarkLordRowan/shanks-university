// src/widgets/AlgorithmSeriesErrorStatsTable/ui/ErrorStatsCell.tsx

import React from "react";
import type { ErrorStats } from "../model/errorStats";

export type HeatClass = "neutral" | "ok" | "warn" | "bad" | "fatal";

function fmt(x: number | null | undefined): string {
    if (x == null || !Number.isFinite(x)) return "∅";
    return x.toExponential(2);
}

function heatBg(h: HeatClass): string {
    switch (h) {
        case "ok":
            return "bg-emerald-600/45 hover:bg-emerald-600/60";
        case "warn":
            return "bg-lime-500/45 hover:bg-lime-500/60";
        case "bad":
            return "bg-amber-500/50 hover:bg-amber-500/65";
        case "fatal":
            return "bg-red-600/55 hover:bg-red-600/70";
        default:
            return "bg-slate-800/45 hover:bg-slate-800/60";
    }
}

interface ErrorStatsCellProps {
    stats: ErrorStats | null;
    active?: boolean;
    heatClass?: HeatClass;
    onClick?: () => void;
    title?: string;
}

export const ErrorStatsCell: React.FC<ErrorStatsCellProps> = ({
    stats,
    active = false,
    heatClass = "neutral",
    onClick,
    title,
}) => {
    const sel = active ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    const base =
        "w-full h-full min-h-[32px] min-w-[90px] cursor-pointer select-none " +
        "rounded border border-border px-[6px] py-[4px] " +
        "text-[10px] leading-tight text-textDim text-left transition-colors " +
        heatBg(heatClass) +
        sel;

    if (!stats) {
        return (
            <button type="button" className={base} onClick={onClick} title={title ?? "Нет данных"}>
                <span className="text-textDim/50">—</span>
            </button>
        );
    }

    const autoTitle =
        title ??
        [`max=${stats.max}`, `mean=${stats.mean}`, `min=${stats.min}`, `n=${stats.count}`].join(
            "\n"
        );

    return (
        <button type="button" className={base} onClick={onClick} title={autoTitle}>
            {/* max на главном месте */}
            <div className="flex justify-between gap-2">
                <span className="text-textDim/70">max</span>
                <span className="font-mono tabular-nums text-[11px]">{fmt(stats.max)}</span>
            </div>

            <div className="mt-[2px] flex justify-between gap-2 text-[9px] text-textDim/80">
                <span className="text-textDim/70">mean</span>
                <span className="font-mono tabular-nums">{fmt(stats.mean)}</span>
            </div>

            <div className="mt-[2px] flex justify-between gap-2 text-[9px] text-textDim/70">
                <span className="text-textDim/70">min</span>
                <span className="font-mono tabular-nums">{fmt(stats.min)}</span>
            </div>

            <div className="mt-[2px] flex justify-between gap-2 text-[9px] text-textDim/70">
                <span>n</span>
                <span className="font-mono tabular-nums">{stats.count}</span>
            </div>
        </button>
    );
};
