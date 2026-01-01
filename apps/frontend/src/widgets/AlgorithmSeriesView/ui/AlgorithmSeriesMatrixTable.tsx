// src/widgets/AlgorithmSeriesMatrixTable/AlgorithmSeriesMatrixTable.tsx

import React, { useMemo } from "react";
import type { Accel, Experiment, Series } from "@/entities/experiment/model/experiment.ts";
import type { MatrixAxisItem } from "@/shared/ui/Matrix/Matrix.tsx";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries.tsx";

export type SelectedCell = { accelId: string; seriesId: string };

/** Единый способ построить id для DOM-элемента ячейки. */
export function getAlgorithmSeriesCellDomId(accelId: string, seriesId: string): string {
    return `algo-series-cell-${accelId}::${seriesId}`;
}

interface AlgorithmSeriesMatrixTableProps {
    experiment: Experiment | null;
    maxSeries?: number;

    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
}

type Row = MatrixAxisItem<Accel>;
type Col = MatrixAxisItem<Series>;

export const AlgorithmSeriesMatrixTable: React.FC<AlgorithmSeriesMatrixTableProps> = ({
    experiment,
    maxSeries = 20,
    selectedCell,
    onCellSelect,
}) => {
    const rows: Row[] = useMemo(
        () => (experiment?.accelList ?? []).map((a) => ({ id: a.id, meta: a })),
        [experiment?.accelList]
    );

    const cols: Col[] = useMemo(
        () => (experiment?.seriesList ?? []).map((s) => ({ id: s.id, meta: s })),
        [experiment?.seriesList]
    );

    if (!experiment || rows.length === 0 || cols.length === 0) return null;

    return (
        <MatrixAlgorithmSeries
            accelList={experiment.accelList ?? []}
            seriesList={experiment.seriesList ?? []}
            maxColsPerPage={maxSeries}
            renderTitle={() => "Результаты: алгоритмы × ряды"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {rows.length} · Ряды: {cols.length}
                </>
            )}
            export={{
                fileBaseName: "algorithm-series-view",
                enablePng: true,
            }}
            renderCell={(row, col) => {
                const accelId = row.id;
                const seriesId = col.id;

                const isSelected =
                    selectedCell?.accelId === accelId && selectedCell?.seriesId === seriesId;

                return (
                    <div
                        id={getAlgorithmSeriesCellDomId(accelId, seriesId)}
                        onClick={() => onCellSelect({ accelId, seriesId })}
                        className={[
                            "relative w-full h-full min-h-[32px] cursor-pointer",
                            "border border-border/50",
                            "transition-colors duration-150",
                            !isSelected && "hover:bg-surface/60",
                            isSelected &&
                                ["bg-accent/10", "border-accent/60", "ring-1 ring-accent/60"].join(
                                    " "
                                ),
                        ]
                            .filter(Boolean)
                            .join(" ")}
                    >
                        {/* акцентная полоса слева */}
                        {isSelected && (
                            <div className="absolute left-0 top-0 h-full w-[3px] bg-accent" />
                        )}

                        {/* контент */}
                        <div className="relative z-10 flex h-full items-center justify-center text-[10px] text-textDim">
                            {isSelected ? "выбрано" : ""}
                        </div>
                    </div>
                );
            }}
        />
    );
};
