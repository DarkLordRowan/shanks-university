import React, { useState } from "react";
import { type SelectedDetail } from "../model/types";
import { formatMonotonicityShort, formatSideShort } from "../model/convergenceUtils";
import { ConvergenceErrorChart } from "./detail/ConvergenceErrorChart";
import { ConvergenceAnTable } from "./detail/ConvergenceAnTable";
import { ConvergenceDiffTable } from "./detail/ConvergenceDiffTable";

interface ConvergenceDetailChartProps {
    detail: SelectedDetail;
}

export const ConvergenceDetailChart: React.FC<ConvergenceDetailChartProps> = ({ detail }) => {
    const [useAbs, setUseAbs] = useState<boolean>(true);

    if (!detail.series || !detail.analysis) {
        return null;
    }

    const shortSide = formatSideShort(detail.analysis.side);
    const shortMon = formatMonotonicityShort(detail.analysis.monotonicity);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 text-xs text-textDim shadow-panel">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">
                        Детальный график сходимости
                    </div>
                    {/*<div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">*/}
                    {/*    <div>*/}
                    {/*        Ряд: {series.seriesName}, x={series.xLabel}, prec=*/}
                    {/*        {series.precision}*/}
                    {/*    </div>*/}
                    {/*    <div>*/}
                    {/*        Алгоритм: {accel.algorithmName}*/}
                    {/*        {accel.m != null ? `, m=${accel.m}` : ""}*/}
                    {/*    </div>*/}
                    {/*    {accel.argsSummary && <div>Аргументы: {accel.argsSummary}</div>}*/}
                    {/*</div>*/}
                </div>
                <div className="space-y-1 text-right text-[11px] text-textDim/80">
                    <div>
                        Тип: {shortSide} | {shortMon}
                    </div>
                    <div>Сравнено шагов (пар): {detail.analysis.stepsAnalyzed}</div>

                    {/* Глобальный переключатель "модуль / со знаком" для всех трёх элементов */}
                    <div className="pt-1">
                        <span className="mr-1 text-[10px] text-textDim/70">Ошибка:</span>
                        <button
                            type="button"
                            className={
                                "mr-1 rounded border px-2 py-[1px] text-[10px]" +
                                (useAbs
                                    ? " border-primary bg-primary/20"
                                    : " border-border bg-surface")
                            }
                            onClick={() => setUseAbs(true)}
                        >
                            модуль
                        </button>
                        <button
                            type="button"
                            className={
                                "rounded border px-2 py-[1px] text-[10px]" +
                                (!useAbs
                                    ? " border-primary bg-primary/20"
                                    : " border-border bg-surface")
                            }
                            onClick={() => setUseAbs(false)}
                        >
                            со знаком
                        </button>
                    </div>
                </div>
            </div>

            {/* useAbs уходит во все три компонента */}
            <ConvergenceErrorChart points={detail.points} limit={detail.limit} useAbs={useAbs} />
            <ConvergenceAnTable points={detail.points} useAbs={useAbs} />
            <ConvergenceDiffTable points={detail.points} useAbs={useAbs} />
        </div>
    );
};
