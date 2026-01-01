// src/widgets/AlgorithmSeriesMatrixTable/ui/AlgorithmSeriesDetailChart.tsx

import React from "react";
import type { Accel, Series } from "@/entities/experiment/model/experiment.ts";
import type { SelectedCell } from "./AlgorithmSeriesMatrixTable";

export interface AlgorithmSeriesDetailChartProps {
    selectedCell: SelectedCell;
    selectedDetail: {
        series: Series | null;
        accel: Accel | null;
    } | null;

    onScrollBackToSelectedCell: () => void;
}

export const AlgorithmSeriesDetailChart: React.FC<AlgorithmSeriesDetailChartProps> = ({
    selectedCell,
    selectedDetail,
    onScrollBackToSelectedCell,
}) => {
    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 shadow-panel">
            <div className="mb-3 flex items-center justify-between">
                <div className="text-sm font-medium">Выбранная ячейка</div>
                <button
                    type="button"
                    className="rounded border border-border bg-surface px-2 py-[2px] text-[10px] text-textDim hover:bg-panel"
                    onClick={onScrollBackToSelectedCell}
                >
                    Вернуться к выбранной ячейке
                </button>
            </div>

            <div className="grid gap-2 text-[12px]">
                <div className="grid grid-cols-[140px_1fr] gap-2">
                    <div className="text-textDim">accelId</div>
                    <div className="font-mono">{selectedCell.accelId}</div>
                </div>

                <div className="grid grid-cols-[140px_1fr] gap-2">
                    <div className="text-textDim">seriesId</div>
                    <div className="font-mono">{selectedCell.seriesId}</div>
                </div>

                <div className="mt-2 grid grid-cols-[140px_1fr] gap-2">
                    <div className="text-textDim">Алгоритм</div>
                    <div className="font-mono">
                        {selectedDetail?.accel?.name ?? selectedDetail?.accel?.id ?? "—"}
                    </div>
                </div>

                <div className="grid grid-cols-[140px_1fr] gap-2">
                    <div className="text-textDim">Ряд</div>
                    <div className="font-mono">
                        {selectedDetail?.series?.name ?? selectedDetail?.series?.id ?? "—"}
                    </div>
                </div>
            </div>
        </div>
    );
};
