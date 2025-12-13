// src/widgets/AlgorithmSeriesErrorStatsTable/ui/ErrorStatsCell.tsx

import React from "react";
import type { ErrorStats } from "../model/errorStats";

type HeatClass = "neutral" | "ok" | "warn" | "bad" | "fatal";

function fmt(x: number) {
    return x.toExponential(2);
}

function heatBg(h: HeatClass): string {
    switch (h) {
        case "ok":
            return "bg-emerald-500/14 hover:bg-emerald-500/22";
        case "warn":
            return "bg-amber-300/18 hover:bg-amber-300/26";
        case "bad":
            return "bg-orange-500/18 hover:bg-orange-500/26";
        case "fatal":
            return "bg-red-500/20 hover:bg-red-500/30";
        default:
            return "bg-surface/20 hover:bg-surface/30";
    }
}

interface ErrorStatsCellProps {
    stats: ErrorStats | null;
    active?: boolean;
    heatClass?: HeatClass;
    onClick?: () => void;
}

export const ErrorStatsCell: React.FC<ErrorStatsCellProps> = ({
    stats,
    active = false,
    heatClass = "neutral",
    onClick,
}) => {
    const sel = active ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    // максимально “как в Convergence”: маленькая карточка внутри td
    const base =
        "w-full min-w-[90px] rounded border border-border px-[6px] py-[4px] " +
        "text-[10px] leading-tight text-textDim text-left transition-colors " +
        heatBg(heatClass) +
        sel;

    if (!stats) {
        return (
            <button type="button" className={base} onClick={onClick} title="Нет данных">
                <span className="text-textDim/50">—</span>
            </button>
        );
    }

    const title = [
        `max=${stats.max}`,
        `min=${stats.min}`,
        `mean=${stats.mean}`,
        `n=${stats.count}`,
    ].join("\n");

    return (
        <button type="button" className={base} onClick={onClick} title={title}>
            <div className="flex justify-between gap-2">
                <span className="text-textDim/70">max</span>
                <span className="font-mono tabular-nums text-[11px]">{fmt(stats.max)}</span>
            </div>

            <div className="mt-[2px] flex justify-between gap-2 text-[9px] text-textDim/80">
                <span className="text-textDim/70">mean</span>
                <span className="font-mono tabular-nums">{fmt(stats.mean)}</span>
            </div>

            <div className="mt-[2px] flex justify-between gap-2 text-[9px] text-textDim/70">
                <span>n</span>
                <span className="font-mono tabular-nums">{stats.count}</span>
            </div>
        </button>
    );
};
