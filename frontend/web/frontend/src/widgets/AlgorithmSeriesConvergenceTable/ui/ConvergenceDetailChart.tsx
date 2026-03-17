import React, { useState } from "react";
import { type SelectedDetail } from "../model/types";
import { buildArgsSummary } from "../model/convergenceUtils";
import {
    formatAmplitudeOrders,
    formatComplexValue,
    formatDeviationValue,
} from "../model/convergenceSummary";
import { ConvergenceErrorChart } from "./detail/ConvergenceErrorChart";
import { ConvergenceAnTable } from "./detail/ConvergenceAnTable";
import { ConvergenceDiffTable } from "./detail/ConvergenceDiffTable";

interface ConvergenceDetailChartProps {
    detail: SelectedDetail;
}

export const ConvergenceDetailChart: React.FC<ConvergenceDetailChartProps> = ({ detail }) => {
    const { series, accel, analysis, limit, points, classInfo, dev } = detail;
    const [useAbs, setUseAbs] = useState<boolean>(true);

    if (!series || !accel || !analysis || !classInfo) {
        return null;
    }

    const algoArgs = buildArgsSummary(accel.args ?? null);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 text-xs text-textDim shadow-panel">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">Алгоритм × ряд</div>
                    <div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">
                        <div>Ряд: {series.name}</div>
                        <div>precision: {series.precision}</div>
                        <div>limit: {formatComplexValue(limit ?? series.limit ?? null)}</div>
                        <div>
                            Алгоритм: {accel.name}
                            {accel.m != null ? `, m=${accel.m}` : ""}
                        </div>
                        {algoArgs ? <div>args: {algoArgs}</div> : null}
                    </div>
                </div>

                <div className="space-y-1 text-right text-[11px] text-textDim/80">
                    <div>
                        Класс: {classInfo.label} ({classInfo.title})
                    </div>
                    <div>Шагов в анализе: {analysis.stepsAnalyzed}</div>
                    <div>
                        min: {formatDeviationValue(dev.min)} @ n={dev.minN ?? "—"}
                    </div>
                    <div>
                        last-min: {formatDeviationValue(dev.lastMinusMin)} | amp:{" "}
                        {formatAmplitudeOrders(dev.amplitudeOrders)}
                    </div>

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

            <div className="mb-3 rounded border border-border/60 bg-surface/40 px-3 py-2 text-[11px] text-textDim/80">
                {classInfo.description}
            </div>

            <ConvergenceErrorChart points={points} useAbs={useAbs} />
            <ConvergenceAnTable points={points} useAbs={useAbs} />
            <ConvergenceDiffTable points={points} useAbs={useAbs} />
        </div>
    );
};
