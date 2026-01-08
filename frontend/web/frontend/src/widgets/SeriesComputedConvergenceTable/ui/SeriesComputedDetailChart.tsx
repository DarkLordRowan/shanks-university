import React, { useMemo, useState } from "react";
import type { Series } from "@/entities/experiment/model/experiment";
import type { DetailPoint, MonotonicityType, SideType } from "../model/types";
import { formatMonotonicityShort, formatSideShort } from "../model/seriesComputedConvergenceUtils";
import { SeriesComputedErrorChart } from "./detail/SeriesComputedErrorChart";
import { SeriesComputedAnTable } from "./detail/SeriesComputedAnTable";
import { SeriesComputedDiffTable } from "./detail/SeriesComputedDiffTable";

interface SeriesComputedDetailChartProps {
    series: Series;
    side: SideType;
    monotonicity: MonotonicityType;
    stepsAnalyzed: number;
    points: DetailPoint[];
}

export const SeriesComputedDetailChart: React.FC<SeriesComputedDetailChartProps> = ({
    series,
    side,
    monotonicity,
    stepsAnalyzed,
    points,
}) => {
    const [useAbs, setUseAbs] = useState<boolean>(true);

    const sideShort = useMemo(() => formatSideShort(side), [side]);
    const monShort = useMemo(() => formatMonotonicityShort(monotonicity), [monotonicity]);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 text-xs text-textDim shadow-panel">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">Частичные суммы</div>
                    <div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">
                        <div>Ряд: {series.name}</div>
                        <div>precision: {series.precision}</div>
                    </div>
                </div>

                <div className="space-y-1 text-right text-[11px] text-textDim/80">
                    <div>
                        Тип: {sideShort} | {monShort}
                    </div>
                    <div>Сравнено шагов (пар): {stepsAnalyzed}</div>

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
