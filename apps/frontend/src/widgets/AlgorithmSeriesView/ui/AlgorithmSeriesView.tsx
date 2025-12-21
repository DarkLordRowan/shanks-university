// src/widgets/AlgorithmSeriesErrorStatsTable/AlgorithmSeriesErrorStatsTable.tsx

import React, { useMemo, useState } from "react";
import type {
    Accel,
    Experiment,
    Series,
    SeriesAccel,
} from "@/entities/experiment/model/experiment.ts";
import type { MatrixAxisItem } from "@/shared/ui/Matrix/Matrix.tsx";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries.tsx";

interface AlgorithmSeriesViewProps {
    experiment: Experiment | null;
    maxSeries?: number;
}

type Row = MatrixAxisItem<Accel>;
type Col = MatrixAxisItem<Series>;

export const AlgorithmSeriesView: React.FC<AlgorithmSeriesViewProps> = ({
    experiment,
    maxSeries = 20,
}) => {
    const [selected, setSelected] = useState<{ accelId: string; seriesId: string } | null>(null);

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
            accelList={experiment?.accelList ?? []}
            seriesList={experiment?.seriesList ?? []}
            maxColsPerPage={maxSeries}
            // colWidth={90}
            // minCellHeightPx={64}
            renderTitle={() => "Результаты: алгоритмы × ряды"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {rows.length} · Ряды: {cols.length} · global max:{" "}
                </>
            )}
            export={{
                fileBaseName: "algorithm-series-view",
                enablePng: true,
            }}
            renderCell={(row, col) => {
                return (
                    <div>
                        {row.name}
                        {col.name}
                    </div>
                );
            }}
        />
    );
};
