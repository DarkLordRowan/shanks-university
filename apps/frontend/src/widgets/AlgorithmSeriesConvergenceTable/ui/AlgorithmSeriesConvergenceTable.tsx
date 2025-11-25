import React, { useEffect, useMemo, useRef, useState } from "react";
import { useConvergenceMatrix } from "../model/useConvergenceMatrix";
import { type SelectedCell, type SelectedDetail, type DetailPoint } from "../model/types";
import { errorNorm, realDiffSign, getPointsSortedByN } from "../model/convergenceUtils";
import { ConvergenceDetailChart } from "./ConvergenceDetailChart";
import { ConvergenceMatrixTable } from "./ConvergenceMatrixTable";

export interface AlgorithmSeriesConvergenceTableProps {
    experiment: import("../model/types").Experiment | null;
    className?: string;
    maxSeries?: number;
}

// убираем отдельный props-файл, чтобы не плодить мусор
// (оставлено определение выше)

export const AlgorithmSeriesConvergenceTable: React.FC<AlgorithmSeriesConvergenceTableProps> = ({
    experiment,
    className,
    maxSeries,
}) => {
    const { matrix, progress } = useConvergenceMatrix(experiment);
    const [selectedCell, setSelectedCell] = useState<SelectedCell | null>(null);
    const chartRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setSelectedCell(null);
    }, [experiment]);

    useEffect(() => {
        if (selectedCell && chartRef.current) {
            chartRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!selectedCell || !experiment || !matrix) return null;

        const { seriesId, accelId } = selectedCell;

        const series =
            (experiment.seriesList ?? []).find((s) => s.id === seriesId) ?? null;
        const accel =
            (experiment.accelList ?? []).find((a) => a.id === accelId) ?? null;
        const sa =
            (experiment.seriesAccelList ?? []).find(
                (x) => x.series_id === seriesId && x.accel_id === accelId
            ) ?? null;

        const seriesInfo = matrix.seriesList.find((s) => s.key === seriesId) ?? null;
        const algoInfo = matrix.algoList.find((a) => a.key === accelId) ?? null;

        const analysis = matrix.cells[`${accelId}::${seriesId}`] ?? null;
        const limit = series?.limit ?? null;

        let prevVal: { re: number | null; im: number | null } | null = null;

        const points: DetailPoint[] = sa
            ? (() => {
                  const sorted = getPointsSortedByN(sa);
                  const pts: DetailPoint[] = [];

                  for (const p of sorted) {
                      const valueRe = p.value?.re ?? null;
                      const valueImRaw = p.value?.im ?? null;
                      const valueIm = valueImRaw ?? 0;
                      const err = errorNorm(p.value, limit);
                      const sign = realDiffSign(p.value, limit);

                      let diffRe: number | null = null;
                      let diffIm: number | null = null;
                      let diffNorm: number | null = null;

                      if (
                          prevVal &&
                          valueRe != null &&
                          Number.isFinite(valueRe) &&
                          prevVal.re != null &&
                          Number.isFinite(prevVal.re)
                      ) {
                          const prevIm = prevVal.im ?? 0;
                          const dRe = valueRe - prevVal.re;
                          const dIm = valueIm - prevIm;
                          const dn = Math.hypot(dRe, dIm);
                          diffRe = dRe;
                          diffIm = dIm;
                          diffNorm = Number.isFinite(dn) ? dn : null;
                      }

                      prevVal = { re: valueRe, im: valueImRaw };

                      pts.push({
                          n: p.n,
                          valueRe,
                          valueIm: valueImRaw,
                          err,
                          sign,
                          diffRe,
                          diffIm,
                          diffNorm,
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
            limit,
            points,
        };
    }, [selectedCell, experiment, matrix]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет данных для анализа (experiment = null).
                </div>
            </div>
        );
    }

    if (!matrix || progress.running) {
        const { current, total } = progress;
        const pct =
            total > 0
                ? Math.max(0, Math.min(100, Math.round((current / total) * 100)))
                : 0;

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
                matrix={matrix}
                maxSeries={maxSeries}
                selectedCell={selectedCell}
                onCellSelect={setSelectedCell}
            />

            <div ref={chartRef}>
                {selectedDetail && selectedDetail.analysis && (
                    <ConvergenceDetailChart detail={selectedDetail} />
                )}
            </div>
        </div>
    );
};
