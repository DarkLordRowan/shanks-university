// src/widgets/AlgorithmSeriesErrorStatsTable/AlgorithmSeriesErrorStatsTable.tsx

import React, { useCallback, useMemo, useState } from "react";
import type {
    Accel,
    Experiment,
    Series,
    SeriesAccel,
} from "@/entities/experiment/model/experiment";
import { Matrix, type MatrixAxisItem } from "@/shared/ui/Matrix/Matrix";
import { MatrixExportWrapper } from "@/shared/ui/Matrix/MatrixExportWrapper";
import { computeErrorStats, type ErrorStats } from "./model/errorStats";
import { ErrorStatsCell } from "./ui/ErrorStatsCell";
import * as XLSX from "xlsx-js-style";

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

    const buildWorkbook = useCallback((): XLSX.WorkBook => {
        const wb = XLSX.utils.book_new();

        // header
        const header: (string | number)[] = ["Алгоритм \\ Ряд"];
        for (const s of cols) {
            header.push(s.meta?.name ?? s.id);
        }

        const data: (string | number | null)[][] = [];
        data.push(header);

        // rows: algorithms
        for (const a of rows) {
            const row: (string | number | null)[] = [];
            row.push(a.meta?.name ?? a.id);

            for (const s of cols) {
                const st = statsIndex[s.id]?.[a.id] ?? null;
                if (!st) {
                    row.push(null);
                } else {
                    // компактно в одну ячейку: min / max / mean
                    row.push(`min=${st.min}; max=${st.max}; mean=${st.mean}; n=${st.count}`);
                }
            }

            data.push(row);
        }

        const ws = XLSX.utils.aoa_to_sheet(data);
        XLSX.utils.book_append_sheet(wb, ws, "error_stats");

        return wb;
    }, [rows, cols, statsIndex]);

    if (!experiment || rows.length === 0 || cols.length === 0) {
        return null;
    }

    return (
        <MatrixExportWrapper
            fileBaseName="algorithm-series-error-stats"
            buildWorkbook={buildWorkbook}
            enablePng
            enableXlsx
        >
            {({ noInnerScroll, noSticky, captureRef }) => (
                <div ref={captureRef}>
                    <div className="mb-2 flex flex-col gap-1">
                        <span className="text-sm font-semibold text-textDim">
                            Статистика ошибок: алгоритмы × ряды
                        </span>
                        <span className="text-[11px] text-textDim/80">
                            Алгоритмы: {rows.length} · Ряды: {cols.length}
                        </span>
                    </div>

                    <Matrix<Accel, Series>
                        rows={rows}
                        cols={cols}
                        enableInnerScroll={!noInnerScroll}
                        stickyHeaders={!noSticky}
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
                            const active =
                                selected?.accelId === row.id && selected?.seriesId === col.id;

                            return (
                                <ErrorStatsCell
                                    stats={stats}
                                    active={active}
                                    onClick={() =>
                                        setSelected({ accelId: row.id, seriesId: col.id })
                                    }
                                />
                            );
                        }}
                    />
                </div>
            )}
        </MatrixExportWrapper>
    );
};
