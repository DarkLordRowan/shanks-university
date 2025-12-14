// src/widgets/AlgorithmSeriesErrorStatsTable/ui/ErrorStatsCell.tsx

import React from "react";
import type { ErrorStats } from "../model/errorStats";

function fmt(x: number) {
    // Подстрой под свои precision/формат, пока так.
    return x.toExponential(3);
}

interface ErrorStatsCellProps {
    stats: ErrorStats | null;
    active?: boolean;
    onClick?: () => void;
}

export const ErrorStatsCell: React.FC<ErrorStatsCellProps> = ({ stats, active, onClick }) => {
    const base = "w-full rounded px-2 py-1 text-xs border transition-colors text-left";
    const cls = active
        ? "bg-primary text-primary-foreground border-primary"
        : "border-border hover:bg-surface/60";

    return (
        <button type="button" className={`${base} ${cls}`} onClick={onClick}>
            {stats ? (
                <div className="grid gap-0.5">
                    <div className="flex justify-between gap-2">
                        <span className="text-textDim">min</span>
                        <span className="font-mono">{fmt(stats.min)}</span>
                    </div>
                    <div className="flex justify-between gap-2">
                        <span className="text-textDim">max</span>
                        <span className="font-mono">{fmt(stats.max)}</span>
                    </div>
                    <div className="flex justify-between gap-2">
                        <span className="text-textDim">mean</span>
                        <span className="font-mono">{fmt(stats.mean)}</span>
                    </div>
                    <div className="text-[10px] text-textDim">n={stats.count}</div>
                </div>
            ) : (
                <div className="text-textDim">—</div>
            )}
        </button>
    );
};
