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

/** 0 = лучше, больше = хуже/неопределённо */
function classScore(side: SideType, mon: MonotonicityType): number {
    if (side === "unknown" || mon === "unknown") return 4;
    const mono = isMonotone(mon);
    if (side === "one_sided" && mono) return 0; // зелёный
    if (side === "one_sided" && !mono) return 1; // синий
    if (side === "two_sided" && mono) return 2; // жёлтый
    if (side === "two_sided" && !mono) return 3; // красный
    return 4;
}

type SortKey = "name" | "precision" | "args" | "class" | "k" | "sign" | "viol";
type SortDir = "asc" | "desc";

export const SeriesComputedConvergenceTable: React.FC<SeriesComputedConvergenceTableProps> = ({
    experiment,
    className,
}) => {
    const { seriesList, analysisBySeriesId, progress } = useSeriesComputedConvergence(experiment);

    const [selectedSeriesId, setSelectedSeriesId] = useState<string | null>(null);

    const [maxSignChangesForOneSided, setMaxSignChangesForOneSided] = useState<number>(0);
    const [maxViolationsForMonotone, setMaxViolationsForMonotone] = useState<number>(0);

    const [sortKey, setSortKey] = useState<SortKey | null>(null);
    const [sortDir, setSortDir] = useState<SortDir>("asc");

    const detailRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setSelectedSeriesId(null);
        setMaxSignChangesForOneSided(0);
        setMaxViolationsForMonotone(0);
        setSortKey(null);
        setSortDir("asc");
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

    type SortKey = "name" | "precision" | "args" | "class" | "k" | "sign" | "viol";
    type SortDir = "asc" | "desc";

    const [sort, setSort] = useState<{ key: SortKey; dir: SortDir } | null>(null);

    const toggleSort = useCallback((key: SortKey) => {
        setSort((prev) => {
            if (!prev || prev.key !== key) return { key, dir: "asc" };
            return { key, dir: prev.dir === "asc" ? "desc" : "asc" };
        });
    }, []);

    const sortMark = useCallback(
        (key: SortKey) => {
            if (!sort || sort.key !== key) return "";
            return sort.dir === "asc" ? "▲" : "▼";
        },
        [sort]
    );

    const rows = useMemo(() => {
        return seriesList
            .map((s) => {
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

                const sideShort = formatSideShort(side);
                const monShort = formatMonotonicityShort(mon);
                const argsSummary = buildArgsSummary((s.args ?? null) as any);

                return {
                    series: s,
                    analysis: a,
                    side,
                    mon,
                    sideShort,
                    monShort,
                    argsSummary,
                    clsScore: classScore(side, mon),
                };
            })
            .filter(Boolean) as Array<{
            series: Series;
            analysis: any;
            side: SideType;
            mon: MonotonicityType;
            sideShort: string;
            monShort: string;
            argsSummary: string;
            clsScore: number;
        }>;
    }, [seriesList, analysisBySeriesId, maxSignChangesForOneSided, maxViolationsForMonotone]);

    const sortedRows = useMemo(() => {
        if (!sort) return rows;

        const mul = sort.dir === "asc" ? 1 : -1;

        const cmpStr = (a: string, b: string) => a.localeCompare(b, "ru");
        const cmpNum = (a: number, b: number) => a - b;

        const isNumKey =
            sort.key === "class" || sort.key === "k" || sort.key === "sign" || sort.key === "viol";

        const get = (r: (typeof rows)[number]) => {
            switch (sort.key) {
                case "name":
                    return r.series.name ?? "";
                case "precision":
                    return r.series.precision ?? "";
                case "args":
                    return r.argsSummary ?? "";
                case "class":
                    return r.clsScore;
                case "k":
                    return r.analysis.stepsAnalyzed ?? 0;
                case "sign":
                    return r.analysis.signChangesCount ?? 0;
                case "viol":
                    return r.analysis.incCount ?? 0;
            }
        };

        const arr = [...rows];
        arr.sort((ra, rb) => {
            const va = get(ra) as any;
            const vb = get(rb) as any;

            const c = isNumKey ? cmpNum(Number(va), Number(vb)) : cmpStr(String(va), String(vb));

            if (c !== 0) return mul * c;

            // tie-break
            return cmpStr(ra.series.name ?? "", rb.series.name ?? "");
        });

        return arr;
    }, [rows, sort]);

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
            argsSummary: buildArgsSummary((series.args ?? null) as any),
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
                    Клик по строке открывает детальный график и таблицы. Клик по заголовку включает
                    сортировку.
                </div>
            </div>

            <div
                className="rounded-xl border border-border bg-surface/40 overflow-auto"
                style={{ maxHeight: "55vh" }}
            >
                <table className="min-w-full border-collapse text-[10px]">
                    <thead className="bg-surface/80 sticky top-0 z-10">
                        <tr>
                            <th
                                className="border-b border-border px-2 py-2 text-left cursor-pointer select-none"
                                onClick={() => toggleSort("name")}
                                title="Сортировать по имени ряда"
                            >
                                Ряд{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("name")}
                                </span>
                            </th>
                            <th
                                className="border-b border-border px-2 py-2 text-left cursor-pointer select-none"
                                onClick={() => toggleSort("precision")}
                                title="Сортировать по precision"
                            >
                                prec{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("precision")}
                                </span>
                            </th>
                            <th
                                className="border-b border-border px-2 py-2 text-left cursor-pointer select-none"
                                onClick={() => toggleSort("args")}
                                title="Сортировать по аргументам ряда (строка)"
                            >
                                args{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("args")}
                                </span>
                            </th>
                            <th
                                className="border-b border-border px-2 py-2 text-left cursor-pointer select-none"
                                onClick={() => toggleSort("class")}
                                title="Сортировать по классу (лучше → хуже)"
                            >
                                класс{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("class")}
                                </span>
                            </th>
                            <th
                                className="border-b border-border px-2 py-2 text-right cursor-pointer select-none"
                                onClick={() => toggleSort("k")}
                                title="Сортировать по числу пар (n−1,n) в анализе"
                            >
                                k{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("k")}
                                </span>
                            </th>
                            <th
                                className="border-b border-border px-2 py-2 text-right cursor-pointer select-none"
                                onClick={() => toggleSort("sign")}
                                title="Сортировать по числу смен знака"
                            >
                                sign{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("sign")}
                                </span>
                            </th>
                            <th
                                className="border-b border-border px-2 py-2 text-right cursor-pointer select-none"
                                onClick={() => toggleSort("viol")}
                                title="Сортировать по числу нарушений монотонности ||Sₙ−S||"
                            >
                                viol{" "}
                                <span className="ml-1 text-[9px] text-textDim/70">
                                    {sortMark("viol")}
                                </span>
                            </th>
                        </tr>
                    </thead>

                    <tbody>
                        {sortedRows.map((r) => {
                            const s = r.series;
                            const a = r.analysis;

                            const isSelected = selectedSeriesId === s.id;
                            const rowCls = getRowColorClass(r.side, r.mon, isSelected);

                            const signNsText = a.signChangeNs.length
                                ? formatIntervals(a.signChangeNs)
                                : "—";
                            const violNsText = a.violationsNs.length
                                ? formatIntervals(a.violationsNs)
                                : "—";

                            const titleLines: string[] = [];
                            titleLines.push(`Ряд: ${s.name}`);
                            titleLines.push(`prec: ${s.precision}`);
                            titleLines.push(`args: ${r.argsSummary || "—"}`);
                            titleLines.push(
                                `limit: ${s.limit ? `(${s.limit.re}, ${s.limit.im ?? 0})` : "∅"}`
                            );
                            titleLines.push("");
                            titleLines.push(
                                `Класс: ${describeClass(r.side, r.mon)} (${r.sideShort} | ${r.monShort})`
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
                                    <td
                                        className="border-t border-border px-2 py-2 font-mono max-w-[240px] truncate"
                                        title={r.argsSummary || "—"}
                                    >
                                        {r.argsSummary || "—"}
                                    </td>
                                    <td className="border-t border-border px-2 py-2 font-mono">
                                        {r.sideShort} | {r.monShort}
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
                        <div className="mb-2 flex items-start justify-between gap-2">
                            <div className="text-[10px] text-textDim">
                                <div className="font-semibold text-textDim/90">
                                    {selected.series.name}
                                </div>
                                <div className="font-mono">
                                    prec: {selected.series.precision}{" "}
                                    <span className="text-textDim/70">|</span> args:{" "}
                                    {selected.argsSummary || "—"}
                                </div>
                            </div>

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
