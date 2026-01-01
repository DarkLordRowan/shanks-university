// src/widgets/AlgorithmSeriesMatrixTable/AlgorithmSeriesView.tsx

import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import type {
    Accel,
    Experiment,
    Series,
    SeriesAccel,
} from "@/entities/experiment/model/experiment.ts";
import {
    AlgorithmSeriesMatrixTable,
    getAlgorithmSeriesCellDomId,
    type SelectedCell,
} from "./AlgorithmSeriesMatrixTable";
import { AlgorithmSeriesDetailChart } from "./AlgorithmSeriesDetailChart";

export interface AlgorithmSeriesViewProps {
    experiment: Experiment | null;
    className?: string;
    maxSeries?: number;
}

type SelectedDetail = {
    series: Series | null;
    accel: Accel | null;
    seriesAccel: SeriesAccel | null;
};

export const AlgorithmSeriesView: React.FC<AlgorithmSeriesViewProps> = ({
    experiment,
    className,
    maxSeries,
}) => {
    const [selectedCell, setSelectedCell] = useState<SelectedCell | null>(null);
    const detailRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setSelectedCell(null);
    }, [experiment]);

    useEffect(() => {
        if (selectedCell && detailRef.current) {
            detailRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedCell]);

    const scrollBackToSelectedCell = useCallback(() => {
        if (!selectedCell) return;
        const domId = getAlgorithmSeriesCellDomId(selectedCell.accelId, selectedCell.seriesId);
        const el = document.getElementById(domId);
        if (el) el.scrollIntoView({ behavior: "smooth", block: "center", inline: "center" });
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!experiment || !selectedCell) return null;

        const { accelId, seriesId } = selectedCell;

        const accel = (experiment.accelList ?? []).find((a) => a.id === accelId) ?? null;
        const series = (experiment.seriesList ?? []).find((s) => s.id === seriesId) ?? null;

        const seriesAccel =
            (experiment.seriesAccelList ?? []).find(
                (x) => x.accel_id === accelId && x.series_id === seriesId
            ) ?? null;

        return { accel, series, seriesAccel };
    }, [experiment, selectedCell]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">Нет данных (experiment = null).</div>
            </div>
        );
    }

    const algoCount = experiment.accelList?.length ?? 0;
    const seriesCount = experiment.seriesList?.length ?? 0;

    if (algoCount === 0 || seriesCount === 0) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет данных: accelList или seriesList пусты.
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            <AlgorithmSeriesMatrixTable
                experiment={experiment}
                maxSeries={maxSeries}
                selectedCell={selectedCell}
                onCellSelect={setSelectedCell}
            />

            <div ref={detailRef}>
                {selectedCell && (
                    <AlgorithmSeriesDetailChart
                        selectedCell={selectedCell}
                        selectedDetail={
                            selectedDetail
                                ? { accel: selectedDetail.accel, series: selectedDetail.series }
                                : null
                        }
                        onScrollBackToSelectedCell={scrollBackToSelectedCell}
                    />
                )}
            </div>
        </div>
    );
};
