// src/widgets/AlgorithmSeriesErrorStatsTable/AlgorithmSeriesErrorStatsTable.tsx

import React, { useMemo, useState } from "react";
import type {
    Accel,
    Experiment,
    Series,
    SeriesAccel,
} from "@/entities/experiment/model/experiment";
import { Matrix, type MatrixAxisItem } from "@/shared/ui/Matrix/Matrix";
import { computeErrorStats, type ErrorStats } from "./model/errorStats";
import { ErrorStatsCell } from "./ui/ErrorStatsCell";

interface AlgorithmSeriesErrorStatsTableProps {
    experiment: Experiment | null;
}

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

type Row = MatrixAxisItem<Accel>;
type Col = MatrixAxisItem<Series>;

export const AlgorithmSeriesErrorStatsTable: React.FC<AlgorithmSeriesErrorStatsTableProps> = ({
    experiment,
}) => {
    const [selected, setSelected] = useState<{ accelId: string; seriesId: string } | null>(null);

    const rows: Row[] = useMemo(() => {
        const list = experiment?.accelList ?? [];
        return list.map((a) => ({ id: a.id, meta: a }));
    }, [experiment?.accelList]);

    const cols: Col[] = useMemo(() => {
        const list = experiment?.seriesList ?? [];
        return list.map((s) => ({ id: s.id, meta: s }));
    }, [experiment?.seriesList]);

    const statsIndex = useMemo(() => {
        return buildStatsIndex(experiment?.seriesAccelList);
    }, [experiment?.seriesAccelList]);

    if (!experiment || rows.length === 0 || cols.length === 0) {
        return null;
    }

    return (
        <div className="relative">
            <div>
                <div className="flex flex-col gap-1">
                    <span className="text-sm font-semibold text-textDim">
                        Статистика ошибок: алгоритмы × ряды
                    </span>
                    <span className="text-[11px] text-textDim/80">
                        Алгоритмы: {rows.length} · Ряды: {cols.length}
                    </span>
                </div>
            </div>

            <Matrix<Accel, Series>
                rows={rows}
                cols={cols}
                enableInnerScroll
                maxBodyHeight="80vh"
                rowWidth={180}
                colWidth={100}
                renderCorner={() => (
                    <div className="text-[10px] text-textDim/80">Алгоритм \ Ряд</div>
                )}
                renderRowHeader={(row) => (
                    <div className="whitespace-pre leading-tight">
                        <span className="block max-w-[160px] truncate text-textDim">
                            {row.meta?.name ?? row.id}
                        </span>
                        <span className="text-[9px] text-textDim/60">{row.id}</span>
                    </div>
                )}
                renderColHeader={(col) => (
                    <div
                        className="relative flex h-28 w-[56px] items-center justify-center"
                        title={`${col.meta?.name ?? col.id}\n${col.id}`}
                    >
                        <span className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 rotate-[-90deg] whitespace-nowrap text-[9px] leading-tight text-textDim">
                            {col.meta?.name ?? col.id}
                        </span>
                        <span className="absolute bottom-1 text-[8px] text-textDim/60">
                            {col.id}
                        </span>
                    </div>
                )}
                renderCell={(row, col) => {
                    const stats = statsIndex[col.id]?.[row.id] ?? null;
                    const active = selected?.accelId === row.id && selected?.seriesId === col.id;

                    return (
                        <ErrorStatsCell
                            stats={stats}
                            active={active}
                            onClick={() => setSelected({ accelId: row.id, seriesId: col.id })}
                        />
                    );
                }}
            />
        </div>
    );
};
