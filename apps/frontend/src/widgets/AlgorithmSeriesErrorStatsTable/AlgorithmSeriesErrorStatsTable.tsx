// src/widgets/AlgorithmSeriesErrorStatsTable/AlgorithmSeriesErrorStatsTable.tsx

import React, { useCallback, useMemo, useState } from "react";
import * as XLSX from "xlsx-js-style";
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

interface AlgorithmSeriesErrorStatsTableProps {
    experiment: Experiment | null;
}

type Row = MatrixAxisItem<Accel>;
type Col = MatrixAxisItem<Series>;

type StatsIndex = Record<string, Record<string, ErrorStats | null>>;

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

type HeatClass = "neutral" | "ok" | "warn" | "bad" | "fatal";

function getGlobalMax(statsIndex: StatsIndex): number {
    let g = 0;
    for (const seriesId of Object.keys(statsIndex)) {
        for (const accelId of Object.keys(statsIndex[seriesId])) {
            const st = statsIndex[seriesId][accelId];
            if (st && Number.isFinite(st.max) && st.max > g) g = st.max;
        }
    }
    return g;
}

function classifyByMax(st: ErrorStats | null, globalMax: number): HeatClass {
    if (!st || !Number.isFinite(st.max) || st.count <= 0) return "neutral";
    if (!(globalMax > 0)) return "neutral";

    const x = Math.log10(st.max);
    const g = Math.log10(globalMax);

    // нормируем в диапазон ~ [g-6, g]
    const t = Math.min(1, Math.max(0, (x - (g - 6)) / 6));

    if (t < 0.25) return "ok";
    if (t < 0.5) return "warn";
    if (t < 0.75) return "bad";
    return "fatal";
}

export const AlgorithmSeriesErrorStatsTable: React.FC<AlgorithmSeriesErrorStatsTableProps> = ({
    experiment,
}) => {
    const [selected, setSelected] = useState<{
        accelId: string;
        seriesId: string;
    } | null>(null);

    const rows: Row[] = useMemo(() => {
        return (experiment?.accelList ?? []).map((a) => ({
            id: a.id,
            meta: a,
        }));
    }, [experiment?.accelList]);

    const cols: Col[] = useMemo(() => {
        return (experiment?.seriesList ?? []).map((s) => ({
            id: s.id,
            meta: s,
        }));
    }, [experiment?.seriesList]);

    const statsIndex = useMemo(
        () => buildStatsIndex(experiment?.seriesAccelList),
        [experiment?.seriesAccelList]
    );

    const globalMax = useMemo(() => getGlobalMax(statsIndex), [statsIndex]);

    const buildWorkbook = useCallback((): XLSX.WorkBook => {
        const wb = XLSX.utils.book_new();

        const header: (string | number)[] = ["Алгоритм \\ Ряд"];
        for (const c of cols) {
            header.push(c.meta?.name ?? c.id);
        }

        const data: (string | number | null)[][] = [];
        data.push(header);

        for (const r of rows) {
            const row: (string | number | null)[] = [];
            row.push(r.meta?.name ?? r.id);

            for (const c of cols) {
                const st = statsIndex[c.id]?.[r.id];
                row.push(
                    st ? `max=${st.max}; mean=${st.mean}; min=${st.min}; n=${st.count}` : null
                );
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
        <>
            <div
                className="
                            sticky top-0 z-40 mb-2
                            -mx-4 px-4 py-2
                            bg-surface/95 backdrop-blur-sm
                            flex items-center justify-between
                        "
            >
                <div className="flex flex-col gap-1">
                    <span className="text-sm font-semibold text-textDim">
                        Статистика ошибок: алгоритмы × ряды
                    </span>
                    <span className="text-[11px] text-textDim/80">
                        Алгоритмы: {rows.length} · Ряды: {cols.length} · global max:{" "}
                        <span className="font-mono tabular-nums">
                            {globalMax > 0 ? globalMax.toExponential(2) : "—"}
                        </span>
                    </span>
                </div>
            </div>
            <MatrixExportWrapper
                fileBaseName="algorithm-series-error-stats"
                buildWorkbook={buildWorkbook}
                enablePng
                enableXlsx
            >
                {({ noInnerScroll, noSticky, captureRef }) => (
                    <div ref={captureRef}>
                        <Matrix<Accel, Series>
                            rows={rows}
                            cols={cols}
                            enableInnerScroll={!noInnerScroll}
                            stickyHeaders={!noSticky}
                            maxBodyHeight="80vh"
                            rowWidth={220}
                            colWidth={100}
                            renderCorner={() => (
                                <div className="text-[10px] text-textDim/80">Алгоритм \ Ряд</div>
                            )}
                            renderRowHeader={(row) => (
                                <div className="leading-tight">
                                    <div className="max-w-[200px] truncate text-textDim">
                                        {row.meta?.name ?? row.id}
                                    </div>
                                    <div className="text-[9px] text-textDim/60">{row.id}</div>
                                </div>
                            )}
                            renderColHeader={(col) => (
                                <div
                                    className="relative flex h-28 w-[64px] items-center justify-center"
                                    title={`${col.meta?.name ?? col.id}\n${col.id}`}
                                >
                                    <span className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 rotate-[-90deg] whitespace-nowrap text-[9px] leading-tight text-textDim">
                                        {col.meta?.name ?? col.id}
                                    </span>
                                </div>
                            )}
                            renderCell={(row, col) => {
                                const stats = statsIndex[col.id]?.[row.id] ?? null;
                                const active =
                                    selected?.accelId === row.id && selected?.seriesId === col.id;

                                const heatClass = classifyByMax(stats, globalMax);

                                return (
                                    <ErrorStatsCell
                                        stats={stats}
                                        active={active}
                                        heatClass={heatClass}
                                        onClick={() =>
                                            setSelected({
                                                accelId: row.id,
                                                seriesId: col.id,
                                            })
                                        }
                                    />
                                );
                            }}
                        />
                    </div>
                )}
            </MatrixExportWrapper>
        </>
    );
};
