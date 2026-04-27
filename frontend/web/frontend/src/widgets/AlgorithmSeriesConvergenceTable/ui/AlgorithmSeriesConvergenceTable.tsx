import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import {
    buildExperimentSessionStateKey,
    useInMemorySessionState,
} from "@/shared/lib/inMemorySessionState";
import { buildExperimentIndex, buildSeriesAccelPairKey } from "@/shared/lib/experimentIndex";
import { useConvergenceMatrix } from "../model/useConvergenceMatrix";
import { type SelectedCell, type SelectedDetail } from "../model/types";
import {
    getConvergenceCellDomId,
    formatMonotonicityWithMax,
    formatSideWithMax,
} from "../model/convergenceUtils";
import {
    buildConvergenceDetailPoints,
    computeConvergenceDevStats,
    getConvergenceClassInfo,
} from "../model/convergenceSummary";
import { ConvergenceDetailChart } from "./ConvergenceDetailChart";
import { ConvergenceMatrixTable } from "./ConvergenceMatrixTable";

export interface AlgorithmSeriesConvergenceTableProps {
    experiment: import("../model/types").Experiment | null;
    className?: string;
    maxSeries?: number;
}

export const AlgorithmSeriesConvergenceTable: React.FC<AlgorithmSeriesConvergenceTableProps> = ({
    experiment,
    className,
    maxSeries,
}) => {
    const { matrix, progress } = useConvergenceMatrix(experiment);
    const [selectedCell, setSelectedCell] = useState<SelectedCell | null>(null);
    const viewSessionKey = experiment
        ? buildExperimentSessionStateKey(experiment.id, "view:series-convergence")
        : undefined;
    const [viewState, setViewState] = useInMemorySessionState({
        key: viewSessionKey,
        initialValue: {
            maxSignChangesForOneSided: 0,
            maxViolationsForMonotone: 0,
        },
    });
    const chartRef = useRef<HTMLDivElement | null>(null);
    const experimentIndex = useMemo(() => buildExperimentIndex(experiment), [experiment]);
    const { maxSignChangesForOneSided, maxViolationsForMonotone } = viewState;

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
        const domId = getConvergenceCellDomId(selectedCell.accelId, selectedCell.seriesId);
        const el = document.getElementById(domId);
        if (el) {
            el.scrollIntoView({ behavior: "smooth", block: "center", inline: "center" });
        }
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!selectedCell || !matrix) return null;

        const { seriesId, accelId } = selectedCell;
        const series = experimentIndex.seriesById.get(seriesId) ?? null;
        const accel = experimentIndex.accelById.get(accelId) ?? null;
        const sa = experimentIndex.getSeriesAccel(seriesId, accelId);

        const analysis = matrix.cells[buildSeriesAccelPairKey(accelId, seriesId)] ?? null;
        const limit = series?.limit ?? null;
        const points = buildConvergenceDetailPoints(sa, limit);
        const dev = computeConvergenceDevStats(points);
        const side = analysis
            ? formatSideWithMax(analysis, maxSignChangesForOneSided)
            : "unknown";
        const monotonicity = analysis
            ? formatMonotonicityWithMax(analysis, maxViolationsForMonotone)
            : "unknown";
        const classInfo = analysis ? getConvergenceClassInfo(side, monotonicity, dev) : null;

        return {
            series,
            accel,
            analysis,
            limit,
            points,
            classInfo,
            dev,
        };
    }, [
        selectedCell,
        experimentIndex,
        matrix,
        maxSignChangesForOneSided,
        maxViolationsForMonotone,
    ]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">Нет данных для анализа (experiment = null).</div>
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
                        <span>Подсчёт монотонности и направления...</span>
                        <span>
                            {current} / {total} ({pct}%)
                        </span>
                    </div>
                    <div className="h-2 w-full overflow-hidden rounded bg-surface/60">
                        <div
                            className="h-2 bg-primary transition-[width]"
                            style={{ width: `${pct}%` }}
                        />
                    </div>
                </div>
            </div>
        );
    }

    const seriesList = matrix.seriesList ?? [];
    const algoList = matrix.algoList ?? [];

    if (seriesList.length === 0 || algoList.length === 0) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет пар ряд × алгоритм для анализа (seriesList или accelList пусты).
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            <ConvergenceMatrixTable
                experiment={experiment}
                matrix={matrix}
                maxSeries={maxSeries}
                maxSignChangesForOneSided={maxSignChangesForOneSided}
                maxViolationsForMonotone={maxViolationsForMonotone}
                onMaxSignChangesForOneSidedChange={(value) =>
                    setViewState((current) => ({
                        ...current,
                        maxSignChangesForOneSided: value,
                    }))
                }
                onMaxViolationsForMonotoneChange={(value) =>
                    setViewState((current) => ({
                        ...current,
                        maxViolationsForMonotone: value,
                    }))
                }
                selectedCell={selectedCell}
                onCellSelect={setSelectedCell}
            />

            <div ref={chartRef}>
                {selectedDetail && selectedDetail.analysis && selectedDetail.classInfo && (
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
                        <ConvergenceDetailChart detail={selectedDetail} />
                    </div>
                )}
            </div>
        </div>
    );
};
