import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import { useProfilingMatrix } from "../model/useProfilingMatrix";
import type { ProfilingPoint, SelectedCell, SelectedDetail } from "../model/types";
import type { SeriesAccelComputedPoint, Profiling } from "@/entities/experiment/model/experiment";
import { getProfilingCellDomId } from "../model/profilingUtils";
import { ProfilingMatrixTable } from "./ProfilingMatrixTable";
import { ProfilingDetailView } from "./ProfilingDetailView";
import { buildExperimentIndex, buildSeriesAccelPairKey } from "@/shared/lib/experimentIndex";

export interface AlgorithmSeriesProfilingTableProps {
    experiment: import("../model/types").Experiment | null;
    className?: string;
    maxSeries?: number;
}

const getComputedPointsSortedByN = (sa: { computed: SeriesAccelComputedPoint[] }): SeriesAccelComputedPoint[] => {
    const raw = sa.computed ?? [];
    if (raw.length <= 1) return raw;

    // cheap check already sorted
    let sorted = true;
    for (let i = 1; i < raw.length; i++) {
        if (raw[i - 1].n > raw[i].n) {
            sorted = false;
            break;
        }
    }
    return sorted ? raw : [...raw].sort((a, b) => a.n - b.n);
};

const profilingToKinds = (pr: Profiling | null | undefined): Record<string, number | null> => {
    if (!pr) return {};
    return {
        add: Number.isFinite(pr.add) ? pr.add : null,
        mul: Number.isFinite(pr.mul) ? pr.mul : null,
        div: Number.isFinite(pr.div) ? pr.div : null,
        special: Number.isFinite(pr.special) ? pr.special : null,
    };
};

const sumKinds = (kinds: Record<string, number | null>): number | null => {
    const vals = Object.values(kinds).filter((v): v is number => typeof v === "number" && Number.isFinite(v));
    if (vals.length === 0) return null;
    return vals.reduce((a, b) => a + b, 0);
};

export const AlgorithmSeriesProfilingTable: React.FC<AlgorithmSeriesProfilingTableProps> = ({
                                                                                                experiment,
                                                                                                className,
                                                                                                maxSeries,
                                                                                            }) => {
    const { matrix, progress } = useProfilingMatrix(experiment);
    const [selectedCell, setSelectedCell] = useState<SelectedCell | null>(null);
    const chartRef = useRef<HTMLDivElement | null>(null);
    const experimentIndex = useMemo(() => buildExperimentIndex(experiment), [experiment]);
    const matrixSeriesById = useMemo(
        () => new Map((matrix?.seriesList ?? []).map((s) => [s.key, s])),
        [matrix?.seriesList]
    );
    const matrixAlgoById = useMemo(
        () => new Map((matrix?.algoList ?? []).map((a) => [a.key, a])),
        [matrix?.algoList]
    );

    useEffect(() => {
        setSelectedCell(null);
    }, [experiment]);

    useEffect(() => {
        if (selectedCell && chartRef.current) {
            chartRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedCell]);

    const scrollBackToSelectedCell = useCallback(() => {
        if (!selectedCell) return;
        const domId = getProfilingCellDomId(selectedCell.accelId, selectedCell.seriesId);
        document.getElementById(domId)?.scrollIntoView({ behavior: "smooth", block: "center", inline: "center" });
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!selectedCell || !matrix) return null;

        const { seriesId, accelId } = selectedCell;

        const series = experimentIndex.seriesById.get(seriesId) ?? null;
        const accel = experimentIndex.accelById.get(accelId) ?? null;

        const sa = experimentIndex.getSeriesAccel(seriesId, accelId);

        const seriesInfo = matrixSeriesById.get(seriesId) ?? null;
        const algoInfo = matrixAlgoById.get(accelId) ?? null;

        const analysis = matrix.cells[buildSeriesAccelPairKey(accelId, seriesId)] ?? null;

        const points: ProfilingPoint[] = sa
            ? (() => {
                const sorted = getComputedPointsSortedByN(sa);

                let prevTotal: number | null = null;
                const prevByKind: Record<string, number | null> = {};

                const pts: ProfilingPoint[] = [];

                for (const p of sorted) {
                    const kinds = profilingToKinds(p.profiling);
                    const totalOps = sumKinds(kinds);

                    const diffTotalOps =
                        prevTotal != null && totalOps != null ? totalOps - prevTotal : null;

                    const diffByKind: Record<string, number | null> = {};
                    for (const [k, v] of Object.entries(kinds)) {
                        const pv = prevByKind[k];
                        diffByKind[k] = pv != null && v != null ? v - pv : null;
                        prevByKind[k] = v;
                    }

                    prevTotal = totalOps;

                    pts.push({
                        n: p.n,
                        totalOps,
                        opsByKind: kinds,
                        diffTotalOps,
                        diffByKind,
                    });
                }

                return pts;
            })()
            : [];

        return {
            seriesInfo,
            algoInfo,
            series,
            accel,
            analysis,
            points,
        };
    }, [selectedCell, experimentIndex, matrix, matrixSeriesById, matrixAlgoById]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">Нет данных (experiment = null).</div>
            </div>
        );
    }

    if (!matrix || progress.running) {
        const { current, total } = progress;
        const pct = total > 0 ? Math.max(0, Math.min(100, Math.round((current / total) * 100))) : 0;

        return (
            <div className={className}>
                <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
                    <div className="mb-2 flex items-center justify-between text-sm text-textDim">
                        <span>Подсчёт profiling-метрик по парам алгоритм×ряд.</span>
                        <span>
                            {current} / {total} ({pct}%)
                        </span>
                    </div>
                    <div className="h-2 w-full overflow-hidden rounded bg-surface/60">
                        <div className="h-2 bg-primary transition-[width]" style={{ width: `${pct}%` }} />
                    </div>
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            <ProfilingMatrixTable
                experiment={experiment}
                matrix={matrix}
                maxSeries={maxSeries}
                selectedCell={selectedCell}
                onCellSelect={setSelectedCell}
            />

            <div ref={chartRef}>
                {selectedDetail && selectedDetail.analysis && (
                    <div className="mt-4">
                        <div className="mb-2 flex justify-end">
                            <button
                                type="button"
                                className="rounded border border-border bg-surface px-2 py-[2px] text-[10px] text-textDim hover:bg-panel"
                                onClick={scrollBackToSelectedCell}
                            >
                                Вернуться к выбранной ячейке
                            </button>
                        </div>

                        <ProfilingDetailView detail={selectedDetail} />
                    </div>
                )}
            </div>
        </div>
    );
};
