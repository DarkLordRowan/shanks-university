// src/widgets/AlgorithmSeriesErrorStatsTable/AlgorithmSeriesErrorStatsTable.tsx

import React, { useMemo, useState } from "react";
import type {
    Experiment,
    Series,
    Accel,
    SeriesAccel,
} from "@/entities/experiment/model/experiment";
import { Matrix, type MatrixAxisItem } from "@/shared/ui/Matrix/Matrix";
import { computeErrorStats, type ErrorStats } from "./model/errorStats";
import { ErrorStatsCell } from "./ui/ErrorStatsCell";

interface AlgorithmSeriesErrorStatsTableProps {
    experiment: Experiment | null;
}

type Row = MatrixAxisItem<Series>;
type Col = MatrixAxisItem<Accel>;

type StatsIndex = Record<string, Record<string, ErrorStats | null>>;
// stats[seriesId][accelId] -> stats|null

function buildStatsIndex(seriesAccelList: SeriesAccel[] | undefined): StatsIndex {
    const index: StatsIndex = {};
    if (!seriesAccelList) return index;

    for (const sa of seriesAccelList) {
        const seriesId = sa.series_id;
        const accelId = sa.accel_id;

        const deviations = (sa.computed ?? []).map((p) => p?.deviation);

        const stats = computeErrorStats(deviations);

        if (!index[seriesId]) index[seriesId] = {};
        index[seriesId][accelId] = stats;
    }

    return index;
}

export const AlgorithmSeriesErrorStatsTable: React.FC<AlgorithmSeriesErrorStatsTableProps> = ({
    experiment,
}) => {
    const [selected, setSelected] = useState<{ seriesId: string; accelId: string } | null>(null);

    const rows: Row[] = useMemo(() => {
        const list = experiment?.seriesList ?? [];
        return list.map((s) => ({ id: s.id, meta: s }));
    }, [experiment?.seriesList]);

    const cols: Col[] = useMemo(() => {
        const list = experiment?.accelList ?? [];
        return list.map((a) => ({ id: a.id, meta: a }));
    }, [experiment?.accelList]);

    const statsIndex = useMemo(() => {
        return buildStatsIndex(experiment?.seriesAccelList);
    }, [experiment?.seriesAccelList]);

    if (!experiment || rows.length === 0 || cols.length === 0) {
        return null;
    }

    return (
        <Matrix<Series, Accel>
            rows={rows}
            cols={cols}
            enableInnerScroll
            maxBodyHeight="80vh"
            renderCorner={() => <div className="text-sm text-textDim">Ряд \ Алгоритм</div>}
            renderRowHeader={(row) => (
                <div className="flex flex-col">
                    <div className="font-medium">{row.meta?.name ?? row.id}</div>
                    <div className="text-xs text-textDim">{row.id}</div>
                </div>
            )}
            renderColHeader={(col) => (
                <div className="flex flex-col items-center">
                    <div className="font-medium">{col.meta?.name ?? col.id}</div>
                    <div className="text-xs text-textDim">{col.id}</div>
                </div>
            )}
            renderCell={(row, col) => {
                const stats = statsIndex[row.id]?.[col.id] ?? null;
                const active = selected?.seriesId === row.id && selected?.accelId === col.id;

                return (
                    <ErrorStatsCell
                        stats={stats}
                        active={active}
                        onClick={() => setSelected({ seriesId: row.id, accelId: col.id })}
                    />
                );
            }}
        />
    );
};
