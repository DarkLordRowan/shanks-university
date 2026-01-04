import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import type { Experiment, Series } from "@/entities/experiment/model/experiment";
import type { MonotonicityType, SideType } from "../model/types";
import { useSeriesComputedConvergence } from "../model/useSeriesComputedConvergence";
import {
    applyMonotonicityThreshold,
    applySideThreshold,
    buildArgsSummary,
    buildDetailPoints,
    describeClass,
    formatIntervals,
    formatMonotonicityShort,
    formatSideShort,
    getSeriesRowDomId,
} from "../model/seriesComputedConvergenceUtils";
import { SeriesComputedDetailChart } from "./SeriesComputedDetailChart";

export interface SeriesComputedConvergenceTableProps {
    experiment: Experiment | null;
    className?: string;
}

function isMonotone(mon: MonotonicityType): boolean {
    return (
        mon === "strict_decreasing_error" ||
        mon === "non_increasing_error" ||
        mon === "constant_error"
    );
}

function getRowColorClass(side: SideType, mon: MonotonicityType, selected: boolean): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (side === "unknown" || mon === "unknown") {
        return "border-border/60 text-textDim/70 bg-surface/30 hover:bg-surface/40" + sel;
    }

    const mono = isMonotone(mon);

    if (side === "one_sided" && mono)
        return "border-border text-textDim bg-emerald-500/25 hover:bg-emerald-500/35" + sel;
    if (side === "one_sided" && !mono)
        return "border-border text-textDim bg-sky-500/25 hover:bg-sky-500/35" + sel;

    if (side === "two_sided" && mono)
        return "border-border text-textDim bg-amber-300/35 hover:bg-amber-300/45" + sel;
    if (side === "two_sided" && !mono)
        return "border-border text-textDim bg-red-500/30 hover:bg-red-500/40" + sel;

    return "border-border text-textDim bg-surface/40 hover:bg-surface/50" + sel;
}

export const SeriesComputedConvergenceTable: React.FC<SeriesComputedConvergenceTableProps> = ({
    experiment,
    className,
}) => {
    const { seriesList, analysisBySeriesId, progress } = useSeriesComputedConvergence(experiment);

    const [selectedSeriesId, setSelectedSeriesId] = useState<string | null>(null);

    const [maxSignChangesForOneSided, setMaxSignChangesForOneSided] = useState<number>(0);
    const [maxViolationsForMonotone, setMaxViolationsForMonotone] = useState<number>(0);

    const detailRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setSelectedSeriesId(null);
        setMaxSignChangesForOneSided(0);
        setMaxViolationsForMonotone(0);
    }, [experiment?.id]);

    useEffect(() => {
        if (selectedSeriesId && detailRef.current) {
            detailRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedSeriesId]);

    const scrollBackToSelected = useCallback(() => {
        if (!selectedSeriesId) return;
        const el = document.getElementById(getSeriesRowDomId(selectedSeriesId));
        if (el) el.scrollIntoView({ behavior: "smooth", block: "center" });
    }, [selectedSeriesId]);

    const selected = useMemo(() => {
        if (!experiment || !selectedSeriesId) return null;

        const series = (experiment.seriesList ?? []).find((s) => s.id === selectedSeriesId) ?? null;
        const analysis = analysisBySeriesId[selectedSeriesId] ?? null;

        if (!series || !analysis) return null;

        const side = applySideThreshold(
            analysis.sideRaw,
            analysis.signChangesCount,
            maxSignChangesForOneSided
        );
        const monotonicity = applyMonotonicityThreshold(
            analysis.monotonicityRaw,
            analysis.incCount,
            analysis.decCount,
            analysis.eqCount,
            maxViolationsForMonotone
        );

        return {
            series,
            analysis,
            side,
            monotonicity,
            points: buildDetailPoints(series),
        };
    }, [
        experiment,
        selectedSeriesId,
        analysisBySeriesId,
        maxSignChangesForOneSided,
        maxViolationsForMonotone,
    ]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет данных для анализа (experiment = null).
                </div>
            </div>
        );
    }

    if (progress.running) {
        const { current, total } = progress;
        const pct = total > 0 ? Math.max(0, Math.min(100, Math.round((current / total) * 100))) : 0;

        return (
            <div className={className}>
                <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
                    <div className="mb-2 flex items-center justify-between text-sm text-textDim">
                        <span>Подсчёт свойств частичных сумм...</span>
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

    if (seriesList.length === 0) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет рядов с рассчитанными частичными суммами (series.computed пуст).
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            <div className="mb-2 rounded-xl border border-border bg-panel p-3 text-[11px] text-textDim shadow-panel">
                <div className="mb-2 text-sm font-semibold">Частичные суммы: анализ по рядам</div>

                <div className="flex flex-col gap-2">
                    <div className="flex items-center gap-2">
                        <span
                            className="whitespace-nowrap"
                            title="Максимальное число смен знака Re(Sₙ − S), при котором ряд считается односторонним."
                        >
                            max sign changes:
                        </span>
                        <input
                            type="range"
                            min={0}
                            max={50}
                            value={maxSignChangesForOneSided}
                            onChange={(e) => setMaxSignChangesForOneSided(Number(e.target.value))}
                            className="h-[4px] w-40 cursor-pointer"
                        />
                        <span className="w-8 text-right tabular-nums">
                            {maxSignChangesForOneSided}
                        </span>
                    </div>

                    <div className="flex items-center gap-2">
                        <span
                            className="whitespace-nowrap"
                            title="Максимальное число увеличений ||Sₙ − S||, при котором ряд классифицируется как (почти) монотонный по ошибке."
                        >
                            max violations:
                        </span>
                        <input
                            type="range"
                            min={0}
                            max={50}
                            value={maxViolationsForMonotone}
                            onChange={(e) => setMaxViolationsForMonotone(Number(e.target.value))}
                            className="h-[4px] w-40 cursor-pointer"
                        />
                        <span className="w-8 text-right tabular-nums">
                            {maxViolationsForMonotone}
                        </span>
                    </div>
                </div>

                <div className="mt-2 text-[10px] text-textDim/70">
                    Клик по строке открывает детальный график и таблицы.
                </div>
            </div>

            <div
                className="rounded-xl border border-border bg-surface/40 overflow-auto"
                style={{ maxHeight: "55vh" }}
            >
                <table className="min-w-full border-collapse text-[10px]">
                    <thead className="bg-surface/80 sticky top-0 z-10">
                        <tr>
                            <th className="border-b border-border px-2 py-2 text-left">Ряд</th>
                            <th className="border-b border-border px-2 py-2 text-left">prec</th>
                            <th className="border-b border-border px-2 py-2 text-left">класс</th>
                            <th className="border-b border-border px-2 py-2 text-right">k</th>
                            <th className="border-b border-border px-2 py-2 text-right">sign</th>
                            <th className="border-b border-border px-2 py-2 text-right">viol</th>
                        </tr>
                    </thead>

                    <tbody>
                        {seriesList.map((s: Series) => {
                            const a = analysisBySeriesId[s.id];
                            if (!a) return null;

                            const side = applySideThreshold(
                                a.sideRaw,
                                a.signChangesCount,
                                maxSignChangesForOneSided
                            );
                            const mon = applyMonotonicityThreshold(
                                a.monotonicityRaw,
                                a.incCount,
                                a.decCount,
                                a.eqCount,
                                maxViolationsForMonotone
                            );

                            const isSelected = selectedSeriesId === s.id;
                            const rowCls = getRowColorClass(side, mon, isSelected);

                            const sideShort = formatSideShort(side);
                            const monShort = formatMonotonicityShort(mon);

                            const argsSummary = buildArgsSummary((s.args ?? null) as any);

                            const signNsText = a.signChangeNs.length
                                ? formatIntervals(a.signChangeNs)
                                : "—";
                            const violNsText = a.violationsNs.length
                                ? formatIntervals(a.violationsNs)
                                : "—";

                            const titleLines: string[] = [];
                            titleLines.push(`Ряд: ${s.name}`);
                            titleLines.push(`prec: ${s.precision}`);
                            titleLines.push(`args: ${argsSummary || "—"}`);
                            titleLines.push(
                                `limit: ${s.limit ? `(${s.limit.re}, ${s.limit.im ?? 0})` : "∅"}`
                            );
                            titleLines.push("");
                            titleLines.push(
                                `Класс: ${describeClass(side, mon)} (${sideShort} | ${monShort})`
                            );
                            titleLines.push(`Пар (n−1,n) в анализе: ${a.stepsAnalyzed}`);
                            titleLines.push(`Смен знака: ${a.signChangesCount}, ns: ${signNsText}`);
                            titleLines.push(`Нарушений ||Sₙ−S||: ${a.incCount}, ns: ${violNsText}`);

                            return (
                                <tr
                                    key={s.id}
                                    id={getSeriesRowDomId(s.id)}
                                    title={titleLines.join("\n")}
                                    className={"cursor-pointer " + rowCls}
                                    onClick={() => setSelectedSeriesId(s.id)}
                                >
                                    <td className="border-t border-border px-2 py-2 font-medium">
                                        {s.name}
                                    </td>
                                    <td className="border-t border-border px-2 py-2 font-mono">
                                        {s.precision}
                                    </td>
                                    <td className="border-t border-border px-2 py-2 font-mono">
                                        {sideShort} | {monShort}
                                    </td>
                                    <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                        {a.stepsAnalyzed}
                                    </td>
                                    <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                        {a.signChangesCount}
                                    </td>
                                    <td className="border-t border-border px-2 py-2 text-right font-mono tabular-nums">
                                        {a.incCount}
                                    </td>
                                </tr>
                            );
                        })}
                    </tbody>
                </table>
            </div>

            <div ref={detailRef}>
                {selected && (
                    <div className="mt-4">
                        <div className="mb-2 flex justify-end">
                            <button
                                type="button"
                                className="rounded border border-border bg-surface px-2 py-[2px] text-[10px] text-textDim hover:bg-panel"
                                onClick={scrollBackToSelected}
                            >
                                Вернуться к выбранному ряду
                            </button>
                        </div>

                        <SeriesComputedDetailChart
                            series={selected.series}
                            side={selected.side}
                            monotonicity={selected.monotonicity}
                            stepsAnalyzed={selected.analysis.stepsAnalyzed}
                            points={selected.points}
                        />
                    </div>
                )}
            </div>
        </div>
    );
};
