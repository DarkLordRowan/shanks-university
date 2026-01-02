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
    const { series, accel, analysis, limit, points } = detail;

    // Глобальный переключатель: использовать модуль или знак
    const [useAbs, setUseAbs] = useState<boolean>(true);

    if (!series || !accel || !analysis) {
        return null;
    }

    const shortSide = formatSideShort(analysis.side);
    const shortMon = formatMonotonicityShort(analysis.monotonicity);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 text-xs text-textDim shadow-panel">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">
                        Детальный график сходимости
                    </div>
                    {/*<div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">*/}
                    {/*    <div>*/}
                    {/*        Ряд: {seriesInfo.seriesName}, x={seriesInfo.xLabel}, prec=*/}
                    {/*        {seriesInfo.precision}*/}
                    {/*    </div>*/}
                    {/*    <div>*/}
                    {/*        Алгоритм: {algoInfo.algorithmName}*/}
                    {/*        {algoInfo.m != null ? `, m=${algoInfo.m}` : ""}*/}
                    {/*    </div>*/}
                    {/*    {algoInfo.argsSummary && <div>Аргументы: {algoInfo.argsSummary}</div>}*/}
                    {/*</div>*/}
                </div>
                <div className="space-y-1 text-right text-[11px] text-textDim/80">
                    <div>
                        Тип: {shortSide} | {shortMon}
                    </div>
                    <div>Сравнено шагов (пар): {analysis.stepsAnalyzed}</div>

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
            <ConvergenceErrorChart points={points} limit={limit} useAbs={useAbs} />
            <ConvergenceAnTable points={points} useAbs={useAbs} />
            <ConvergenceDiffTable points={points} useAbs={useAbs} />
        </div>
    );
};
