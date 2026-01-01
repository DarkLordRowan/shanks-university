import React, { useEffect, useMemo, useRef, useState, useCallback } from "react";
import {
    type SelectedCell,
    type SelectedDetail,
    type DetailPoint,
    type ConvergenceAnalysis,
} from "../model/types";
import {
    errorNorm,
    realDiffSign,
    getPointsSortedByN,
    getConvergenceCellDomId,
    analyzeSeriesAccelConvergence,
} from "../model/convergenceUtils";
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

    const scrollBackToSelectedCell = useCallback(() => {
        if (!selectedCell) return;
        const domId = getConvergenceCellDomId(selectedCell.accelId, selectedCell.seriesId);
        const el = document.getElementById(domId);
        if (el) {
            el.scrollIntoView({ behavior: "smooth", block: "center", inline: "center" });
        }
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!selectedCell || !experiment) return null;

        const { seriesId, accelId } = selectedCell;

        const series = (experiment.seriesList ?? []).find((s) => s.id === seriesId);
        const algo = (experiment.accelList ?? []).find((a) => a.id === accelId);
        const seriesAccelData = (experiment.seriesAccelList ?? []).find(
            (x) => x.series_id === seriesId && x.accel_id === accelId
        );

        if (!series || !algo || !seriesAccelData) return null;

        const analysis: ConvergenceAnalysis = analyzeSeriesAccelConvergence(
            series,
            algo,
            seriesAccelData
        );
        const limit = series?.limit ?? null;

        let prevVal: { re: number | null; im: number | null } | null = null;

        const points: DetailPoint[] = seriesAccelData
            ? (() => {
                  const sorted = getPointsSortedByN(seriesAccelData);
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
            series: series,
            accel: algo,
            analysis: analysis,
            limit: limit,
            points: points,
        };
    }, [selectedCell, experiment]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет данных для анализа (experiment = null).
                </div>
            </div>
        );
    }

    if (experiment.seriesList.length === 0 || experiment.accelList.length === 0) {
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
                        <ConvergenceDetailChart detail={selectedDetail} />
                    </div>
                )}
            </div>
        </div>
    );
};
