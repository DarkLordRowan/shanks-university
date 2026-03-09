// widgets/AlgoRankingTable.tsx

import React, { useCallback, useEffect, useMemo, useState } from "react";
import * as XLSX from "xlsx-js-style";
import type { Experiment, AccelArgs } from "@/entities/experiment/model/experiment";
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";
import type { MatrixAxisItem, MatrixProps } from "@/shared/ui/Matrix/Matrix";

type AlgoKey = string;

interface AlgoStats {
    algoKey: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;

    precision: string | null;

    seriesCount: number;
    bestDeviations: number[];
    stepsToTol: number[]; // Infinity, если не достигли
    reachedTolCount: number;

    avgBestDeviation: number;
    medianBestDeviation: number;
    fracReachedTol: number; // [0,1]
    avgStepsToTol: number; // только по тем, кто достиг; если никто — Infinity

    rankPrecision: number;
    rankSpeed: number;
    rankStability: number;
    totalRankScore: number;
}

export interface AlgoRankingTableProps {
    experiment: Experiment | null;
    /** Допустимая ошибка по |deviation|. По умолчанию 1e-6 */
    epsilon?: number;
    className?: string;
}

type SortKey =
    | "algorithmName"
    | "m"
    | "argsSummary"
    | "precision"
    | "seriesCount"
    | "avgBestDeviation"
    | "fracReachedTol"
    | "avgStepsToTol"
    | "rankPrecision"
    | "rankSpeed"
    | "rankStability"
    | "totalRankScore";

type SortDir = "asc" | "desc";

/** Ключ алгоритма: имя + m + аргументы (для разных конфигураций отдельные строки) */
function makeAlgoKey(
    algorithmName: string,
    m: number | null,
    args: AccelArgs | null | undefined
): AlgoKey {
    const base = `${algorithmName}|m=${m ?? "null"}`;
    if (!args) return base;

    const argEntries = Object.entries(args)
        .filter(([, v]) => v !== undefined && v !== null && v !== "")
        .sort(([a, b]) => a.localeCompare(b));

    if (argEntries.length === 0) return base;

    const suffix = argEntries.map(([k, v]) => `${k}=${v}`).join(";");
    return `${base}|${suffix}`;
}

/** Краткое текстовое описание аргументов алгоритма */
function summarizeArgs(args: AccelArgs | null | undefined): string {
    if (!args) return "";
    const entries = Object.entries(args)
        .filter(([, v]) => v !== undefined && v !== null && v !== "")
        .sort(([a, b]) => a.localeCompare(b));
    if (entries.length === 0) return "";
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

function meanOrInfinity(values: number[]): number {
    if (values.length === 0) return Number.POSITIVE_INFINITY;
    let sum = 0;
    for (const v of values) sum += v;
    return sum / values.length;
}

function medianOrInfinity(values: number[]): number {
    if (values.length === 0) return Number.POSITIVE_INFINITY;
    const sorted = [...values].sort((a, b) => a - b);
    const mid = Math.floor(sorted.length / 2);
    if (sorted.length % 2 === 1) return sorted[mid];
    return (sorted[mid - 1] + sorted[mid]) / 2;
}

function formatNumber(x: number): string {
    if (!Number.isFinite(x)) return "—";
    const ax = Math.abs(x);
    if (ax === 0) return "0";
    if (ax < 1e-4 || ax >= 1e4) return x.toExponential(2);
    return x.toFixed(4);
}

function formatSteps(n: number): string {
    if (!Number.isFinite(n)) return "—";
    return String(Math.round(n));
}

function compareValues(aVal: unknown, bVal: unknown, dir: SortDir): number {
    if (typeof aVal === "string" && typeof bVal === "string") {
        const cmp = aVal.localeCompare(bVal);
        return dir === "asc" ? cmp : -cmp;
    }

    const aNum = typeof aVal === "number" ? aVal : aVal == null ? Number.POSITIVE_INFINITY : 0;
    const bNum = typeof bVal === "number" ? bVal : bVal == null ? Number.POSITIVE_INFINITY : 0;

    if (aNum === bNum) return 0;
    if (dir === "asc") return aNum < bNum ? -1 : 1;
    return aNum > bNum ? -1 : 1;
}

/**
 * Построить агрегированную статистику по алгоритмам из Experiment.
 */
function buildAlgoStatsFromExperiment(
    experiment: Experiment | null,
    epsilon: number,
    precisionFilter: string | null
): AlgoStats[] {
    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return [];
    }

    const seriesById = new Map((experiment.seriesList ?? []).map((s) => [s.id, s]));
    const accelById = new Map((experiment.accelList ?? []).map((a) => [a.id, a]));

    const byAlgo = new Map<AlgoKey, AlgoStats>();

    for (const sa of experiment.seriesAccelList) {
        const series = seriesById.get(sa.series_id);
        if (!series) continue;

        const seriesPrecision = series.precision ?? null;
        if (precisionFilter && seriesPrecision !== precisionFilter) continue;

        const accel = accelById.get(sa.accel_id);
        const algorithmName = accel?.name ?? sa.accel_id;
        const m = accel?.m ?? null;
        const args = accel?.args ?? null;

        const algoKey = makeAlgoKey(algorithmName, m, args);

        let stats = byAlgo.get(algoKey);
        if (!stats) {
            stats = {
                algoKey,
                algorithmName,
                m,
                argsSummary: summarizeArgs(args),

                precision: seriesPrecision,

                seriesCount: 0,
                bestDeviations: [],
                stepsToTol: [],
                reachedTolCount: 0,

                avgBestDeviation: Number.POSITIVE_INFINITY,
                medianBestDeviation: Number.POSITIVE_INFINITY,
                fracReachedTol: 0,
                avgStepsToTol: Number.POSITIVE_INFINITY,

                rankPrecision: 0,
                rankSpeed: 0,
                rankStability: 0,
                totalRankScore: 0,
            };
            byAlgo.set(algoKey, stats);
        } else {
            if (stats.precision !== seriesPrecision) stats.precision = null;
        }

        const computed = sa.computed ?? [];
        if (computed.length === 0) continue;

        let bestDev = Number.POSITIVE_INFINITY;
        let bestNForTol = Number.POSITIVE_INFINITY;

        for (const c of computed) {
            const dev = c.deviation;
            if (dev == null || !Number.isFinite(dev)) continue;

            const absDev = Math.abs(dev);
            if (absDev < bestDev) bestDev = absDev;
            if (absDev <= epsilon && !Number.isFinite(bestNForTol)) bestNForTol = c.n;
        }

        if (!Number.isFinite(bestDev) && !Number.isFinite(bestNForTol)) continue;

        stats.seriesCount += 1;
        if (Number.isFinite(bestDev)) stats.bestDeviations.push(bestDev);

        if (Number.isFinite(bestNForTol)) {
            stats.stepsToTol.push(bestNForTol);
            stats.reachedTolCount += 1;
        } else {
            stats.stepsToTol.push(Number.POSITIVE_INFINITY);
        }
    }

    const list: AlgoStats[] = [];
    for (const stats of byAlgo.values()) {
        if (stats.seriesCount === 0) continue;

        const finiteBest = stats.bestDeviations.filter((v) => Number.isFinite(v));
        const finiteSteps = stats.stepsToTol.filter((v) => Number.isFinite(v));

        stats.avgBestDeviation = meanOrInfinity(finiteBest);
        stats.medianBestDeviation = medianOrInfinity(finiteBest);
        stats.fracReachedTol =
            stats.seriesCount > 0 ? stats.reachedTolCount / stats.seriesCount : 0;
        stats.avgStepsToTol = meanOrInfinity(finiteSteps);

        list.push(stats);
    }

    if (!list.length) return list;

    const byPrecision = [...list].sort((a, b) => a.avgBestDeviation - b.avgBestDeviation);
    byPrecision.forEach((s, idx) => (s.rankPrecision = idx + 1));

    const bySpeed = [...list].sort((a, b) => a.avgStepsToTol - b.avgStepsToTol);
    bySpeed.forEach((s, idx) => (s.rankSpeed = idx + 1));

    const byStability = [...list].sort((a, b) => b.fracReachedTol - a.fracReachedTol);
    byStability.forEach((s, idx) => (s.rankStability = idx + 1));

    for (const s of list) s.totalRankScore = s.rankPrecision + s.rankSpeed + s.rankStability;

    list.sort((a, b) => {
        if (a.totalRankScore !== b.totalRankScore) return a.totalRankScore - b.totalRankScore;
        return a.avgBestDeviation - b.avgBestDeviation;
    });

    return list;
}

/* ---------------- MatrixPaged wiring ---------------- */

type RowMeta = AlgoStats & { place: number; precisionLabel: string };

type ColId =
    | "precision"
    | "m"
    | "argsSummary"
    | "seriesCount"
    | "avgBestDeviation"
    | "avgStepsToTol"
    | "fracReachedTol"
    | "rankPrecision"
    | "rankSpeed"
    | "rankStability"
    | "totalRankScore";

type ColMeta = { id: ColId; title: string; sortKey: SortKey; defaultDir: SortDir };

const COLUMNS: ColMeta[] = [
    { id: "precision", title: "precision", sortKey: "precision", defaultDir: "asc" },
    { id: "m", title: "m", sortKey: "m", defaultDir: "asc" },
    { id: "argsSummary", title: "args", sortKey: "argsSummary", defaultDir: "asc" },
    { id: "seriesCount", title: "series", sortKey: "seriesCount", defaultDir: "desc" },
    {
        id: "avgBestDeviation",
        title: "avg best |dev|",
        sortKey: "avgBestDeviation",
        defaultDir: "asc",
    },
    { id: "avgStepsToTol", title: "avg steps to ε", sortKey: "avgStepsToTol", defaultDir: "asc" },
    { id: "fracReachedTol", title: "reached ε, %", sortKey: "fracReachedTol", defaultDir: "desc" },
    { id: "rankPrecision", title: "rank precision", sortKey: "rankPrecision", defaultDir: "asc" },
    { id: "rankSpeed", title: "rank speed", sortKey: "rankSpeed", defaultDir: "asc" },
    { id: "rankStability", title: "rank stability", sortKey: "rankStability", defaultDir: "asc" },
    { id: "totalRankScore", title: "total rank", sortKey: "totalRankScore", defaultDir: "asc" },
];

export const AlgoRankingTable: React.FC<AlgoRankingTableProps> = ({ experiment, className }) => {
    const [epsilonExp, setEpsilonExp] = useState(-6);
    const epsilon = useMemo(() => Math.pow(10, epsilonExp), [epsilonExp]);

    const [precisionFilter, setPrecisionFilter] = useState<string | null>(null);

    const precisionsOrder = useMemo(() => {
        if (!experiment || !experiment.seriesList) return [];
        const set = new Set<string>();
        for (const s of experiment.seriesList) if (s.precision) set.add(s.precision);
        return Array.from(set).sort();
    }, [experiment]);

    useEffect(() => {
        if (precisionFilter && !precisionsOrder.includes(precisionFilter)) setPrecisionFilter(null);
    }, [precisionFilter, precisionsOrder]);

    const baseStats = useMemo(
        () => buildAlgoStatsFromExperiment(experiment, epsilon, precisionFilter),
        [experiment, epsilon, precisionFilter]
    );

    const [sortKey, setSortKey] = useState<SortKey>("totalRankScore");
    const [sortDir, setSortDir] = useState<SortDir>("asc");

    const sortedStats = useMemo(() => {
        const copy = [...baseStats];
        copy.sort((a, b) => compareValues(a[sortKey], b[sortKey], sortDir));
        return copy;
    }, [baseStats, sortKey, sortDir]);

    const rowsAxis: MatrixAxisItem<RowMeta>[] = useMemo(() => {
        return sortedStats.map((s, idx) => ({
            id: s.algoKey,
            meta: {
                ...s,
                place: idx + 1,
                precisionLabel: s.precision ?? (precisionFilter ? precisionFilter : "—"),
            },
        }));
    }, [sortedStats, precisionFilter]);

    const colsAxis: MatrixAxisItem<ColMeta>[] = useMemo(
        () => COLUMNS.map((c) => ({ id: c.id, meta: c })),
        []
    );

    const handleSort = useCallback((nextKey: SortKey, defaultDir: SortDir) => {
        setSortKey((cur) => {
            if (cur === nextKey) {
                setSortDir((d) => (d === "asc" ? "desc" : "asc"));
                return cur;
            }
            setSortDir(defaultDir);
            return nextKey;
        });
    }, []);

    const renderColHeader: MatrixProps<RowMeta, ColMeta>["renderColHeader"] = (col) => {
        const c = col.meta!;
        const active = c.sortKey === sortKey;
        const icon = active ? (sortDir === "asc" ? "▲" : "▼") : "";

        return (
            <button
                type="button"
                className="w-full px-1 py-1 text-[10px] text-left select-none"
                onClick={() => handleSort(c.sortKey, c.defaultDir)}
                title={`sort: ${c.sortKey}`}
            >
                <span className="truncate">{c.title}</span>
                {icon ? <span className="ml-1 text-[9px] text-textDim/70">{icon}</span> : null}
            </button>
        );
    };

    const renderRowHeader: MatrixProps<RowMeta, ColMeta>["renderRowHeader"] = (row) => {
        const s = row.meta!;
        return (
            <div className="leading-tight">
                <div className="flex items-baseline justify-between gap-2">
                    <div className="max-w-[170px] truncate text-textDim">{s.algorithmName}</div>
                    <div className="font-mono text-[10px] text-textDim/70">#{s.place}</div>
                </div>
                <div className="text-[9px] text-textDim/60">
                    {s.m != null ? `m=${String(s.m)}` : "m=∅"} · prec={s.precisionLabel}
                </div>
                {s.argsSummary ? (
                    <div className="mt-[1px] max-w-[210px] truncate text-[8px] text-textDim/60">
                        {s.argsSummary}
                    </div>
                ) : null}
            </div>
        );
    };

    const renderCell: MatrixProps<RowMeta, ColMeta>["renderCell"] = (row, col) => {
        const s = row.meta!;
        const c = col.meta!.id;

        let text = "—";
        let title = "";

        switch (c) {
            case "precision":
                text = s.precisionLabel;
                break;
            case "m":
                text = s.m != null ? String(s.m) : "—";
                break;
            case "argsSummary":
                text = s.argsSummary || "—";
                break;
            case "seriesCount":
                text = String(s.seriesCount);
                break;
            case "avgBestDeviation":
                text = formatNumber(s.avgBestDeviation);
                break;
            case "avgStepsToTol":
                text = formatSteps(s.avgStepsToTol);
                break;
            case "fracReachedTol":
                text = s.seriesCount > 0 ? (s.fracReachedTol * 100).toFixed(1) + "%" : "—";
                break;
            case "rankPrecision":
                text = String(s.rankPrecision);
                break;
            case "rankSpeed":
                text = String(s.rankSpeed);
                break;
            case "rankStability":
                text = String(s.rankStability);
                break;
            case "totalRankScore":
                text = String(s.totalRankScore);
                break;
        }

        title = `${s.algorithmName}\n${c} = ${text}`;

        return (
            <div
                title={title}
                className="w-full h-full px-2 py-[2px] text-[10px] text-textDim font-mono tabular-nums"
            >
                <span className="block truncate">{text}</span>
            </div>
        );
    };

    const buildWorkbook = useCallback(
        ({
            rows,
            cols,
        }: {
            rows: MatrixAxisItem<RowMeta>[];
            cols: MatrixAxisItem<ColMeta>[];
            pager: { startIndex: number; endIndex: number };
        }): XLSX.WorkBook => {
            const wb = XLSX.utils.book_new();

            const header: (string | number)[] = ["place", "algorithm"];
            for (const c of cols) header.push(c.meta?.id ?? c.id);

            const data: (string | number | null)[][] = [header];

            for (const r of rows) {
                const s = r.meta!;
                const line: (string | number | null)[] = [s.place, s.algorithmName];

                for (const c of cols) {
                    const id = c.meta!.id;

                    switch (id) {
                        case "precision":
                            line.push(s.precisionLabel);
                            break;
                        case "m":
                            line.push(s.m != null ? Number(s.m) || String(s.m) : null);
                            break;
                        case "argsSummary":
                            line.push(s.argsSummary || null);
                            break;
                        case "seriesCount":
                            line.push(s.seriesCount);
                            break;
                        case "avgBestDeviation":
                            line.push(
                                Number.isFinite(s.avgBestDeviation) ? s.avgBestDeviation : null
                            );
                            break;
                        case "avgStepsToTol":
                            line.push(Number.isFinite(s.avgStepsToTol) ? s.avgStepsToTol : null);
                            break;
                        case "fracReachedTol":
                            line.push(Number.isFinite(s.fracReachedTol) ? s.fracReachedTol : null);
                            break;
                        case "rankPrecision":
                            line.push(s.rankPrecision);
                            break;
                        case "rankSpeed":
                            line.push(s.rankSpeed);
                            break;
                        case "rankStability":
                            line.push(s.rankStability);
                            break;
                        case "totalRankScore":
                            line.push(s.totalRankScore);
                            break;
                    }
                }

                data.push(line);
            }

            const ws = XLSX.utils.aoa_to_sheet(data);
            XLSX.utils.book_append_sheet(wb, ws, "algo_ranking");
            return wb;
        },
        []
    );

    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim/80">
                    Нет данных: Experiment пуст или не содержит seriesAccelList.
                </div>
            </div>
        );
    }

    if (!baseStats.length) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim/80">
                    Не удалось построить статистику по алгоритмам
                    {precisionFilter ? ` (precision=${precisionFilter})` : ""}: нет валидных рядов с
                    deviation.
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            {/* controls */}
            <div className="flex items-center gap-4 text-xs text-textDim mb-2">
                <div className="flex flex-col gap-1">
                    <label className="font-medium text-text">Порог точности ε</label>
                    <div className="flex items-baseline gap-2 font-mono">
                        <span>
                            ε = 10<sup>{epsilonExp}</sup>
                        </span>
                        <span className="text-textDim/80">≈ {epsilon.toExponential(2)}</span>
                    </div>
                </div>

                <div className="flex-1">
                    <input
                        type="range"
                        min={-100}
                        max={-1}
                        step={1}
                        value={epsilonExp}
                        onChange={(e) => setEpsilonExp(parseInt(e.target.value, 10))}
                        className="w-full"
                    />
                    <div className="flex justify-between text-[10px] mt-1">
                        <span>10^-100</span>
                        <span>10^-50</span>
                        <span>10^-1</span>
                    </div>
                </div>

                <div className="flex items-center gap-2 text-[10px]">
                    <span>precision:</span>
                    <select
                        className="rounded border border-border bg-surface px-1 py-[1px]"
                        value={precisionFilter ?? ""}
                        onChange={(e) =>
                            setPrecisionFilter(e.target.value === "" ? null : e.target.value)
                        }
                    >
                        <option value="">все</option>
                        {precisionsOrder.map((p) => (
                            <option key={p} value={p}>
                                {p}
                            </option>
                        ))}
                    </select>
                </div>
            </div>

            <MatrixPaged<RowMeta, ColMeta>
                resetKey={`${experiment.id}::${epsilonExp}::${precisionFilter ?? "ALL"}::${sortKey}:${sortDir}`}
                rows={rowsAxis}
                cols={colsAxis}
                maxColsPerPage={0} // фиксированное число колонок, пагинация не нужна
                // layout
                enableInnerScroll
                maxBodyHeight="70vh"
                stickyHeaders
                rowWidth={260}
                colWidth={60}
                className="rounded-xl2 border border-border bg-panel shadow-panel"
                tableClassName="border-separate border-spacing-0"
                thClassName="bg-surface"
                tdClassName="p-0"
                // header
                renderTitle={() => "Рейтинг алгоритмов"}
                renderSubtitle={() =>
                    precisionFilter
                        ? `ε=${epsilon.toExponential(2)} · precision=${precisionFilter} · N=${rowsAxis.length}`
                        : `ε=${epsilon.toExponential(2)} · precision=all · N=${rowsAxis.length}`
                }
                renderHeaderRight={() => (
                    <div className="text-[10px] text-textDim/70 whitespace-nowrap">
                        sort: {sortKey} ({sortDir})
                    </div>
                )}
                // export
                export={{
                    fileBaseName: "algo-ranking",
                    enablePng: true,
                    enableXlsx: true,
                    buildWorkbook,
                }}
                // matrix
                renderCorner={() => <span className="text-left">Алгоритм</span>}
                renderRowHeader={renderRowHeader}
                renderColHeader={renderColHeader}
                renderCell={renderCell}
                emptyFallback={
                    <div className="rounded-xl2 border border-border bg-panel p-3 text-[11px] text-textDim/70">
                        Нет данных
                    </div>
                }
            />
        </div>
    );
};
