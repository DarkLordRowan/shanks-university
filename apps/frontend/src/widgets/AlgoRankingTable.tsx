// widgets/AlgoRankingTable.tsx

import React, { useMemo, useState } from "react";
import type { Experiment, AccelArgs } from "@/types/experiment";

type AlgoKey = string;

interface AlgoStats {
    algoKey: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;

    seriesCount: number;
    bestDeviations: number[];
    stepsToTol: number[];      // Infinity, если не достигли
    reachedTolCount: number;

    avgBestDeviation: number;
    medianBestDeviation: number;
    fracReachedTol: number;    // [0,1]
    avgStepsToTol: number;     // только по тем, кто достиг; если никто — Infinity

    rankPrecision: number;
    rankSpeed: number;
    rankStability: number;
    totalRankScore: number;
}

export interface AlgoRankingTableProps {
    experiment: Experiment | null;
    /** Допустимая ошибка по |deviation|. По умолчанию 1e-6 */
    epsilon?: number;
    /** Фильтр по series.precision; если null/undefined — берем все */
    precision?: string | null;
    className?: string;
}

type SortKey =
    | "algorithmName"
    | "m"
    | "argsSummary"
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
    args: AccelArgs | null | undefined,
): AlgoKey {
    const base = `${algorithmName}|m=${m ?? "null"}`;
    if (!args) return base;

    const argEntries = Object.entries(args)
        .filter(([, v]) => v !== undefined && v !== null && v !== "")
        .sort(([a, b]) => a.localeCompare(b));

    if (argEntries.length === 0) return base;

    const suffix = argEntries
        .map(([k, v]) => `${k}=${v}`)
        .join(";");
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

/** Среднее по массиву; если пустой, возвращает Infinity (чтобы в сортировке были хуже всех) */
function meanOrInfinity(values: number[]): number {
    if (values.length === 0) return Number.POSITIVE_INFINITY;
    let sum = 0;
    for (const v of values) sum += v;
    return sum / values.length;
}

/** Медиана по массиву; если пустой, Infinity */
function medianOrInfinity(values: number[]): number {
    if (values.length === 0) return Number.POSITIVE_INFINITY;
    const sorted = [...values].sort((a, b) => a - b);
    const mid = Math.floor(sorted.length / 2);
    if (sorted.length % 2 === 1) return sorted[mid];
    return (sorted[mid - 1] + sorted[mid]) / 2;
}

/** Форматирование чисел для таблицы */
function formatNumber(x: number): string {
    if (!Number.isFinite(x)) return "—";
    const ax = Math.abs(x);
    if (ax === 0) return "0";
    if (ax < 1e-4 || ax >= 1e4) return x.toExponential(2);
    return x.toFixed(4);
}

/** Форматирование шага n */
function formatSteps(n: number): string {
    if (!Number.isFinite(n)) return "—";
    return String(Math.round(n));
}

/**
 * Построить агрегированную статистику по алгоритмам
 * из структуры Experiment.
 *
 * Для каждого (series_id, accel_id):
 *  - bestDev = min_n |deviation|
 *  - stepsToTol = min n: |deviation| <= epsilon (иначе Infinity)
 */
function buildAlgoStatsFromExperiment(
    experiment: Experiment | null,
    epsilon: number,
    precisionFilter: string | null | undefined,
): AlgoStats[] {
    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return [];
    }

    const seriesById = new Map(
        (experiment.seriesList ?? []).map((s) => [s.id, s]),
    );
    const accelById = new Map(
        (experiment.accelList ?? []).map((a) => [a.id, a]),
    );

    const byAlgo = new Map<AlgoKey, AlgoStats>();

    for (const sa of experiment.seriesAccelList) {
        const series = seriesById.get(sa.series_id);
        if (!series) continue;

        if (precisionFilter && series.precision !== precisionFilter) {
            continue;
        }

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
        }

        const computed = sa.computed ?? [];
        if (computed.length === 0) {
            // можно было бы учитывать чисто ошибки, но тогда нет deviation — пропускаем
            continue;
        }

        let bestDev = Number.POSITIVE_INFINITY;
        let bestNForTol = Number.POSITIVE_INFINITY;

        for (const c of computed) {
            const dev = c.deviation;
            if (dev == null || !Number.isFinite(dev)) continue;

            const absDev = Math.abs(dev);
            if (absDev < bestDev) {
                bestDev = absDev;
            }
            if (absDev <= epsilon && !Number.isFinite(bestNForTol)) {
                bestNForTol = c.n;
            }
        }

        if (!Number.isFinite(bestDev) && !Number.isFinite(bestNForTol)) {
            // полностью "мертвый" (нет нормальных deviation) — не засоряем статистику
            continue;
        }

        stats.seriesCount += 1;

        if (Number.isFinite(bestDev)) {
            stats.bestDeviations.push(bestDev);
        }

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

    // ранги по точности (меньше средняя лучшая ошибка → лучше)
    const byPrecision = [...list].sort(
        (a, b) => a.avgBestDeviation - b.avgBestDeviation,
    );
    byPrecision.forEach((s, idx) => {
        s.rankPrecision = idx + 1;
    });

    // ранги по скорости (меньше шаг до ε → лучше)
    const bySpeed = [...list].sort(
        (a, b) => a.avgStepsToTol - b.avgStepsToTol,
    );
    bySpeed.forEach((s, idx) => {
        s.rankSpeed = idx + 1;
    });

    // ранги по стабильности (больше доля достигших ε → лучше)
    const byStability = [...list].sort(
        (a, b) => b.fracReachedTol - a.fracReachedTol,
    );
    byStability.forEach((s, idx) => {
        s.rankStability = idx + 1;
    });

    // итоговый ранг: сумма трёх рангов
    for (const s of list) {
        s.totalRankScore = s.rankPrecision + s.rankSpeed + s.rankStability;
    }

    // базовый порядок: по итоговому рангу, затем по точности
    list.sort((a, b) => {
        if (a.totalRankScore !== b.totalRankScore) {
            return a.totalRankScore - b.totalRankScore;
        }
        return a.avgBestDeviation - b.avgBestDeviation;
    });

    return list;
}

function renderSortIcon(
    columnKey: SortKey,
    sortKey: SortKey,
    sortDir: SortDir,
): React.ReactNode {
    if (columnKey !== sortKey) return null;
    return (
        <span className="ml-1 text-[9px]">
            {sortDir === "asc" ? "▲" : "▼"}
        </span>
    );
}

function compareValues(aVal: unknown, bVal: unknown, dir: SortDir): number {
    // строки
    if (typeof aVal === "string" && typeof bVal === "string") {
        const cmp = aVal.localeCompare(bVal);
        return dir === "asc" ? cmp : -cmp;
    }

    // числа / null / undefined
    const aNum =
        typeof aVal === "number"
            ? aVal
            : aVal == null
                ? Number.POSITIVE_INFINITY
                : 0;
    const bNum =
        typeof bVal === "number"
            ? bVal
            : bVal == null
                ? Number.POSITIVE_INFINITY
                : 0;

    if (aNum === bNum) return 0;
    if (dir === "asc") {
        return aNum < bNum ? -1 : 1;
    }
    return aNum > bNum ? -1 : 1;
}

export const AlgoRankingTable: React.FC<AlgoRankingTableProps> = ({
                                                                      experiment,
                                                                      epsilon = 1e-6,
                                                                      precision,
                                                                      className,
                                                                  }) => {
    const stats = useMemo(
        () => buildAlgoStatsFromExperiment(experiment, epsilon, precision),
        [experiment, epsilon, precision],
    );

    const [sortKey, setSortKey] = useState<SortKey>("totalRankScore");
    const [sortDir, setSortDir] = useState<SortDir>("asc");

    const minTotalRank = useMemo(
        () =>
            stats.length
                ? Math.min(...stats.map((s) => s.totalRankScore))
                : Number.POSITIVE_INFINITY,
        [stats],
    );

    const sortedStats = useMemo(() => {
        const copy = [...stats];
        copy.sort((a, b) => {
            const aVal = a[sortKey];
            const bVal = b[sortKey];
            return compareValues(aVal, bVal, sortDir);
        });
        return copy;
    }, [stats, sortKey, sortDir]);

    function handleSort(nextKey: SortKey, defaultDir: SortDir = "asc") {
        if (nextKey === sortKey) {
            setSortDir((prev) => (prev === "asc" ? "desc" : "asc"));
        } else {
            setSortKey(nextKey);
            setSortDir(defaultDir);
        }
    }

    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim/80">
                    Нет данных: Experiment пуст или не содержит seriesAccelList.
                </div>
            </div>
        );
    }

    if (!stats.length) {
        return (
            <div className={className}>
                <div className="text-sm text-textDim/80">
                    Не удалось построить статистику по алгоритмам
                    {precision ? ` (precision=${precision})` : ""}: нет валидных рядов с deviation.
                </div>
            </div>
        );
    }

    return (
        <div className={className}>
            <div className="flex items-baseline justify-between mb-2">
                <h2 className="text-base font-semibold text-text">
                    Рейтинг алгоритмов
                </h2>
                <div className="text-xs text-textDim/80">
                    ε = <span className="font-mono">{epsilon}</span>{" "}
                    по |deviation|
                    {precision ? (
                        <span className="ml-2">
                            · precision ={" "}
                            <span className="font-mono">{precision}</span>
                        </span>
                    ) : null}
                </div>
            </div>

            <div className="overflow-x-auto rounded-xl border border-border/60 bg-surface/60">
                <table className="min-w-full text-xs">
                    <thead className="bg-panel/80 text-textDim uppercase tracking-wide">
                    <tr>
                        <th className="px-2 py-1 text-left">
                            <button
                                type="button"
                                className="flex items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("totalRankScore", "asc")
                                }
                            >
                                Место
                                {renderSortIcon(
                                    "totalRankScore",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-left">
                            <button
                                type="button"
                                className="flex items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("algorithmName", "asc")
                                }
                            >
                                Алгоритм
                                {renderSortIcon(
                                    "algorithmName",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-left">
                            <button
                                type="button"
                                className="flex items-center gap-1 select-none"
                                onClick={() => handleSort("m", "asc")}
                            >
                                m (порядок)
                                {renderSortIcon("m", sortKey, sortDir)}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-left">
                            <button
                                type="button"
                                className="flex items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("argsSummary", "asc")
                                }
                            >
                                Аргументы алгоритма
                                {renderSortIcon(
                                    "argsSummary",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("seriesCount", "desc")
                                }
                            >
                                Число рядов
                                {renderSortIcon(
                                    "seriesCount",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>

                        {/* Пара: ранг точности + значение точности */}
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("rankPrecision", "asc")
                                }
                            >
                                Ранг по точности
                                {renderSortIcon(
                                    "rankPrecision",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("avgBestDeviation", "asc")
                                }
                            >
                                Средняя лучшая ошибка
                                {renderSortIcon(
                                    "avgBestDeviation",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>

                        {/* Пара: ранг скорости + значение скорости */}
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("rankSpeed", "asc")
                                }
                            >
                                Ранг по скорости
                                {renderSortIcon(
                                    "rankSpeed",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("avgStepsToTol", "asc")
                                }
                            >
                                Средний шаг до ε
                                {renderSortIcon(
                                    "avgStepsToTol",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>

                        {/* Пара: ранг стабильности + значение стабильности */}
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("rankStability", "asc")
                                }
                            >
                                Ранг по стабильности
                                {renderSortIcon(
                                    "rankStability",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("fracReachedTol", "desc")
                                }
                            >
                                Доля рядов с |deviation| ≤ ε
                                {renderSortIcon(
                                    "fracReachedTol",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>

                        {/* Итоговая сумма рангов */}
                        <th className="px-2 py-1 text-right">
                            <button
                                type="button"
                                className="flex w-full justify-end items-center gap-1 select-none"
                                onClick={() =>
                                    handleSort("totalRankScore", "asc")
                                }
                            >
                                Итоговый ранг
                                {renderSortIcon(
                                    "totalRankScore",
                                    sortKey,
                                    sortDir,
                                )}
                            </button>
                        </th>
                    </tr>
                    </thead>
                    <tbody className="divide-y divide-border/40">
                    {sortedStats.map((s, idx) => {
                        const isBest = s.totalRankScore === minTotalRank;

                        const precisionGroupRankClass =
                            "px-2 py-1 text-right font-mono text-textDim bg-primary/5";
                        const precisionGroupValueClass =
                            "px-2 py-1 text-right font-mono bg-primary/5";

                        const speedGroupRankClass =
                            "px-2 py-1 text-right font-mono text-textDim bg-secondary/5";
                        const speedGroupValueClass =
                            "px-2 py-1 text-right font-mono bg-secondary/5";

                        const stabilityGroupRankClass =
                            "px-2 py-1 text-right font-mono text-textDim bg-amber-900/10";
                        const stabilityGroupValueClass =
                            "px-2 py-1 text-right font-mono bg-amber-900/10";

                        const baseRowClass = isBest
                            ? "bg-primary/5"
                            : "bg-surface/40";

                        return (
                            <tr key={s.algoKey} className={baseRowClass}>
                                <td className="px-2 py-1 text-left font-mono text-textDim">
                                    {idx + 1}
                                </td>
                                <td className="px-2 py-1 text-left">
                                    <div className="font-medium text-text">
                                        {s.algorithmName}
                                    </div>
                                </td>
                                <td className="px-2 py-1 text-left font-mono text-textDim">
                                    {s.m ?? "—"}
                                </td>
                                <td className="px-2 py-1 text-left text-textDim">
                                    {s.argsSummary || "—"}
                                </td>
                                <td className="px-2 py-1 text-right font-mono text-textDim">
                                    {s.seriesCount}
                                </td>

                                {/* группа: точность */}
                                <td className={precisionGroupRankClass}>
                                    {s.rankPrecision}
                                </td>
                                <td className={precisionGroupValueClass}>
                                    {formatNumber(s.avgBestDeviation)}
                                </td>

                                {/* группа: скорость */}
                                <td className={speedGroupRankClass}>
                                    {s.rankSpeed}
                                </td>
                                <td className={speedGroupValueClass}>
                                    {formatSteps(s.avgStepsToTol)}
                                </td>

                                {/* группа: стабильность */}
                                <td className={stabilityGroupRankClass}>
                                    {s.rankStability}
                                </td>
                                <td className={stabilityGroupValueClass}>
                                    {s.seriesCount > 0
                                        ? (s.fracReachedTol * 100).toFixed(
                                        1,
                                    ) + "%"
                                        : "—"}
                                </td>

                                <td className="px-2 py-1 text-right font-mono font-semibold">
                                    {s.totalRankScore}
                                </td>
                            </tr>
                        );
                    })}
                    </tbody>
                </table>
            </div>

            <div className="mt-1 text-[10px] text-textDim/70">
                Средняя лучшая ошибка = среднее по рядам от{" "}
                <span className="font-mono">min_n |deviation_n|</span>;{" "}
                средний шаг до ε = среднее по тем рядам, где нашёлся n с
                |deviation_n| ≤ ε.
            </div>
        </div>
    );
};
