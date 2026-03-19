import React, { useState } from "react";
import type { Series } from "@/entities/experiment/model/experiment";
import type { DetailPoint } from "../model/types";
import type {
    SeriesComputedClassInfo,
    SeriesComputedDevStats,
} from "../model/seriesComputedSummary";
import {
    formatAmplitudeOrders,
    formatComplexValue,
    formatDeviationValue,
} from "../model/seriesComputedSummary";
import { SeriesComputedErrorChart } from "./detail/SeriesComputedErrorChart";
import { SeriesComputedAnTable } from "./detail/SeriesComputedAnTable";
import { SeriesComputedDiffTable } from "./detail/SeriesComputedDiffTable";

interface SeriesComputedDetailChartProps {
    series: Series;
    classInfo: SeriesComputedClassInfo;
    stepsAnalyzed: number;
    points: DetailPoint[];
    dev: SeriesComputedDevStats;
}

export const SeriesComputedDetailChart: React.FC<SeriesComputedDetailChartProps> = ({
    series,
    classInfo,
    stepsAnalyzed,
    points,
    dev,
}) => {
    const [useAbs, setUseAbs] = useState<boolean>(true);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 text-xs text-textDim shadow-panel">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">Частичные суммы</div>
                    <div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">
                        <div>Ряд: {series.name}</div>
                        <div>precision: {series.precision}</div>
                        <div>limit: {formatComplexValue(series.limit ?? null)}</div>
                    </div>
                </div>

                <div className="space-y-1 text-right text-[11px] text-textDim/80">
                    <div>
                        Класс: {classInfo.label} ({classInfo.title})
                    </div>
                    <div>Шагов в анализе: {stepsAnalyzed}</div>
                    <div>
                        min: {formatDeviationValue(dev.min)} @ n={dev.minN ?? "-"}
                    </div>
                    <div>
                        last: {formatDeviationValue(dev.last)} @ n={dev.lastN ?? "-"}
                    </div>
                    <div>
                        last-min: {formatDeviationValue(dev.lastMinusMin)} | last/min amp:{" "}
                        {formatAmplitudeOrders(dev.amplitudeOrders)} | max/min amp:{" "}
                        {formatAmplitudeOrders(dev.maxAmplitudeOrders)}
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

            <SeriesComputedErrorChart points={points} useAbs={useAbs} />
            <SeriesComputedAnTable points={points} useAbs={useAbs} />
            <SeriesComputedDiffTable points={points} useAbs={useAbs} />
        </div>
    );
};
