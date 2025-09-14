import React from "react";
import { type ChartGroup } from "../types";
import { fmt } from "../utils/chartUtils";
import { SimpleLineChart } from "./SimpleLineChart";

export const ChartsSection: React.FC<{ groups: ChartGroup[] }> = ({groups}) => {
    if (!groups.length) return null;

    return (
        <div className="mt-6 space-y-4">
            <h3 className="text-lg font-semibold">Графики по сериям</h3>
            <div className="space-y-4">
                {groups.map((g) => (
                    <div key={g.id} className="rounded-xl border border-border/60 bg-panel/60 p-3">
                        <div className="mb-2 text-sm text-textDim">{g.caption}</div>
                        <div className="grid gap-3 md:grid-cols-2 xl:grid-cols-3">
                            {/* 1) Значения методов + предел */}
                            <SimpleLineChart
                                title="Сравнение значений методов"
                                xLabel="Порядок (n)"
                                series={[
                                    {label: "Частичные суммы", data: g.partial, color: "#3b82f6"},
                                    {label: "Ускоренный метод", data: g.accel, color: "#f59e0b"},
                                ]}
                                horizontalLines={g.limit != null ? [{
                                    y: g.limit,
                                    label: `Предел: ${g.limit}`,
                                    color: "#ef4444",
                                    dashed: true
                                }] : []}
                            />

                            {/* 2) Абсолютное отклонение (лог-шкала) */}
                            <SimpleLineChart
                                title="Абсолютное отклонение (log scale)"
                                xLabel="Порядок (n)"
                                yLabel="|значение − предел|"
                                logY
                                series={[
                                    {label: "Частичные суммы", data: g.errPartial, color: "#3b82f6"},
                                    {label: "Ускоренный метод", data: g.errAccel, color: "#f59e0b"},
                                ]}
                            />

                            {/* 3) Коэффициент ускорения: |Sₙ−L| / |Aₙ−L| */}
                            <SimpleLineChart
                                title="Коэффициент ускорения (Частичная / Ускоренная)"
                                xLabel="Порядок (n)"
                                series={[{label: "Коэффициент ускорения", data: g.ratio, color: "#10b981"}]}
                                logY
                                annotateMaxText={(v) => `Max: ${fmt(v)}x`}
                            />
                        </div>
                    </div>
                ))}
            </div>
        </div>
    );
};
