// widgets/ErrorMatrixTable.tsx

import React, { useMemo, useState, useEffect } from "react";
import type { Experiment } from "@/types/experiment";
import { buildErrorMatrixFromExperiment } from "../analysis/buildErrorMatrix";

/** Оформление ячейки по количеству ошибок */
function getCellClasses(count: number): string {
    if (count <= 0) {
        return "bg-surface/40 text-textDim/40 border-border/60";
    }
    if (count === 1) {
        return "bg-primary/20 border-primary/60 text-textDim";
    }
    if (count <= 4) {
        return "bg-primary/40 border-primary/80 text-textDim";
    }
    return "bg-secondary/50 border-secondary text-textDim";
}

export interface ErrorMatrixTableProps {
    experiment: Experiment | null;
    maxSteps?: number;
    className?: string;
}

type SortKey = "total" | "ok" | "err" | "okPct" | "errPct";
type SortDir = "asc" | "desc";

interface SortState {
    key: SortKey;
    dir: SortDir;
}

export const ErrorMatrixTable: React.FC<ErrorMatrixTableProps> = ({
                                                                      experiment,
                                                                      maxSteps,
                                                                      className,
                                                                  }) => {
    // варианты точности из series.precision
    const precisionOptions = useMemo(() => {
        const set = new Set<string>();
        for (const s of experiment?.seriesList ?? []) {
            if (s.precision) {
                set.add(s.precision);
            }
        }
        return Array.from(set).sort((a, b) => a.localeCompare(b));
    }, [experiment]);

    const [precision, setPrecision] = useState<string | null>(null);

    // сбрасываем precision, если такого значения больше нет
    useEffect(() => {
        if (!precision) return;
        if (!precisionOptions.includes(precision)) {
            setPrecision(null);
        }
    }, [precision, precisionOptions]);

    const {
        nList,
        algoList,
        cellMap,
        cellMessagesMap,
        algoStats,
        totalErrorItems,
    } = useMemo(
        () => buildErrorMatrixFromExperiment(experiment, precision),
        [experiment, precision],
    );

    // суммарное число элементов (по агрегатам)
    const totalElements = useMemo(
        () =>
            Object.values(algoStats).reduce(
                (sum, st) => sum + st.total,
                0,
            ),
        [algoStats],
    );

    // хуки идут подряд и без условий
    const [page, setPage] = useState(0);

    const [sort, setSort] = useState<SortState>({
        key: "err",
        dir: "desc",
    });

    const pageSize =
        maxSteps && maxSteps > 0 ? maxSteps : nList.length || 1;

    const totalPages = Math.max(
        1,
        Math.ceil(nList.length / pageSize || 1),
    );

    // сброс страницы при изменении precision
    useEffect(() => {
        setPage(0);
    }, [precision]);

    useEffect(() => {
        if (page > totalPages - 1) {
            setPage(totalPages - 1);
        }
    }, [page, totalPages]);

    const startIndex = page * pageSize;
    const endIndex = startIndex + pageSize;
    const nListShown =
        maxSteps && maxSteps > 0
            ? nList.slice(startIndex, endIndex)
            : nList;

    const sortedAlgoList = useMemo(() => {
        return [...algoList].sort((a, b) => {
            const sa = algoStats[a.key] ?? {
                total: 0,
                success: 0,
                error: 0,
            };
            const sb = algoStats[b.key] ?? {
                total: 0,
                success: 0,
                error: 0,
            };

            const totalA = sa.total;
            const totalB = sb.total;

            const okA = sa.success;
            const okB = sb.success;

            const errA = sa.error;
            const errB = sb.error;

            const okPctA = totalA > 0 ? okA / totalA : 0;
            const okPctB = totalB > 0 ? okB / totalB : 0;

            const errPctA = totalA > 0 ? errA / totalA : 0;
            const errPctB = totalB > 0 ? errB / totalB : 0;

            let diff = 0;

            switch (sort.key) {
                case "total":
                    diff = totalA - totalB;
                    break;
                case "ok":
                    diff = okA - okB;
                    break;
                case "err":
                    diff = errA - errB;
                    break;
                case "okPct":
                    diff = okPctA - okPctB;
                    break;
                case "errPct":
                    diff = errPctA - errPctB;
                    break;
            }

            return sort.dir === "asc" ? diff : -diff;
        });
    }, [algoList, algoStats, sort]);

    // после хуков можно условно рендерить "нет данных"
    if (!experiment || nList.length === 0 || algoList.length === 0) {
        return (
            <div className={className}>
                <p className="text-xs text-textDim/60">
                    Ошибок с заданным шагом n не обнаружено
                    {precision ? ` (precision=${precision})` : ""}.
                </p>
            </div>
        );
    }

    return (
        <div className={`space-y-2 ${className ?? ""}`}>
            <div className="flex items-center justify-between text-xs text-textDim">
                <h2 className="text-sm font-semibold text-textDim">
                    Матрица ошибок: алгоритмы × шаги n
                </h2>
                <div className="flex items-center gap-3">
                    <div>
                        Алгоритмы: {sortedAlgoList.length} · Шаги n:{" "}
                        {nListShown.length} из {nList.length} ·
                        Элементов с ошибкой (с n): {totalErrorItems} ·
                        Всего элементов: {totalElements}
                    </div>

                    {/* выбор precision, если он вообще есть */}
                    {precisionOptions.length > 0 && (
                        <div className="flex items-center gap-1 text-[11px]">
                            <span className="text-textDim/70">
                                precision:
                            </span>
                            <select
                                className="rounded border border-border bg-surface px-1 py-[1px] text-[11px]"
                                value={precision ?? ""}
                                onChange={(e) => {
                                    const v = e.target.value || null;
                                    setPrecision(v);
                                }}
                            >
                                <option value="">
                                    Все
                                </option>
                                {precisionOptions.map((p) => (
                                    <option key={p} value={p}>
                                        {p}
                                    </option>
                                ))}
                            </select>
                        </div>
                    )}

                    {maxSteps &&
                        maxSteps > 0 &&
                        nList.length > maxSteps && (
                            <div className="flex items-center gap-1 text-[10px]">
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() => setPage(0)}
                                    disabled={page === 0}
                                >
                                    «
                                </button>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() =>
                                        setPage((p) =>
                                            Math.max(0, p - 1),
                                        )
                                    }
                                    disabled={page === 0}
                                >
                                    ‹
                                </button>
                                <span className="px-1">
                                    стр. {page + 1} / {totalPages}
                                </span>
                                <span className="text-textDim/60">
                                    шаги {startIndex + 1}–
                                    {Math.min(endIndex, nList.length)}
                                </span>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() =>
                                        setPage((p) =>
                                            Math.min(
                                                totalPages - 1,
                                                p + 1,
                                            ),
                                        )
                                    }
                                    disabled={page >= totalPages - 1}
                                >
                                    ›
                                </button>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() =>
                                        setPage(totalPages - 1)
                                    }
                                    disabled={page >= totalPages - 1}
                                >
                                    »
                                </button>
                            </div>
                        )}
                </div>
            </div>

            {/* Таблица */}
            <div className="overflow-auto rounded-xl2 border border-border bg-panel shadow-panel">
                <table className="border-collapse text-[10px] leading-tight text-textDim">
                    <thead className="bg-surface/80">
                    <tr>
                        {/* Левый верхний угол */}
                        <th className="sticky left-0 top-0 z-30 border border-border bg-surface/90 px-1 py-1 text-left align-bottom text-[10px]">
                            Алгоритм
                        </th>

                        {/* Агрегатные столбцы (отдельный блок слева от n) */}
                        <th
                            className="sticky left-[140px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[52px] cursor-pointer select-none"
                            onClick={() =>
                                setSort((s) =>
                                    s.key === "total"
                                        ? {
                                            key: "total",
                                            dir:
                                                s.dir === "asc"
                                                    ? "desc"
                                                    : "asc",
                                        }
                                        : {
                                            key: "total",
                                            dir: "desc",
                                        },
                                )
                            }
                        >
                            Всего
                        </th>
                        <th
                            className="sticky left-[192px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[52px] cursor-pointer select-none"
                            onClick={() =>
                                setSort((s) =>
                                    s.key === "ok"
                                        ? {
                                            key: "ok",
                                            dir:
                                                s.dir === "asc"
                                                    ? "desc"
                                                    : "asc",
                                        }
                                        : { key: "ok", dir: "desc" },
                                )
                            }
                        >
                            OK
                        </th>
                        <th
                            className="sticky left-[244px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[52px] cursor-pointer select-none"
                            onClick={() =>
                                setSort((s) =>
                                    s.key === "err"
                                        ? {
                                            key: "err",
                                            dir:
                                                s.dir === "asc"
                                                    ? "desc"
                                                    : "asc",
                                        }
                                        : { key: "err", dir: "desc" },
                                )
                            }
                        >
                            Err
                        </th>
                        <th
                            className="sticky left-[296px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[60px] cursor-pointer select-none"
                            onClick={() =>
                                setSort((s) =>
                                    s.key === "okPct"
                                        ? {
                                            key: "okPct",
                                            dir:
                                                s.dir === "asc"
                                                    ? "desc"
                                                    : "asc",
                                        }
                                        : {
                                            key: "okPct",
                                            dir: "desc",
                                        },
                                )
                            }
                        >
                            % OK
                        </th>
                        <th
                            className="sticky left-[356px] top-0 z-20 border border-border bg-panel/95 border-r-2 border-r-border px-1 py-1 text-center align-bottom text-[10px] min-w-[60px] cursor-pointer select-none"
                            onClick={() =>
                                setSort((s) =>
                                    s.key === "errPct"
                                        ? {
                                            key: "errPct",
                                            dir:
                                                s.dir === "asc"
                                                    ? "desc"
                                                    : "asc",
                                        }
                                        : {
                                            key: "errPct",
                                            dir: "desc",
                                        },
                                )
                            }
                        >
                            % Err
                        </th>

                        {/* Столбцы по шагам n */}
                        {nListShown.map((n) => (
                            <th
                                key={n}
                                className="border border-border px-1 py-1 text-center align-bottom"
                                title={`Шаг n = ${n}`}
                            >
                                <div className="w-[32px] text-center text-[10px]">
                                    n={n}
                                </div>
                            </th>
                        ))}
                    </tr>
                    </thead>
                    <tbody>
                    {sortedAlgoList.map((algo) => {
                        const st = algoStats[algo.key] ?? {
                            total: 0,
                            success: 0,
                            error: 0,
                        };
                        const total = st.total;
                        const ok = st.success;
                        const err = st.error;

                        const okPct =
                            total > 0 ? (ok / total) * 100 : null;
                        const errPct =
                            total > 0 ? (err / total) * 100 : null;

                        return (
                            <tr key={algo.key}>
                                {/* Алгоритм (ось Y) */}
                                <th
                                    className="sticky left-0 z-30 border border-border bg-panel px-1 py-[2px] text-left align-top"
                                    title={(() => {
                                        const lines: string[] = [];
                                        lines.push(
                                            `Алгоритм: ${algo.algorithmName}`,
                                        );
                                        lines.push(
                                            `m = ${
                                                algo.m != null
                                                    ? algo.m
                                                    : "∅"
                                            }`,
                                        );
                                        if (
                                            algo.algorithmArgs &&
                                            Object.keys(
                                                algo.algorithmArgs,
                                            ).length > 0
                                        ) {
                                            lines.push(
                                                "Аргументы:",
                                            );
                                            const entries =
                                                Object.entries(
                                                    algo.algorithmArgs,
                                                ).sort(
                                                    ([a, b]) =>
                                                        a.localeCompare(
                                                            b,
                                                        ),
                                                );
                                            for (const [
                                                k,
                                                v,
                                            ] of entries) {
                                                lines.push(
                                                    `  ${k}: ${v}`,
                                                );
                                            }
                                        }
                                        lines.push("");
                                        lines.push(
                                            `Всего: ${total}, OK: ${ok}, Err: ${err}`,
                                        );
                                        if (okPct != null) {
                                            lines.push(
                                                `% OK ≈ ${okPct.toFixed(
                                                    1,
                                                )}%, % Err ≈ ${errPct!.toFixed(
                                                    1,
                                                )}%`,
                                            );
                                        }
                                        if (precision) {
                                            lines.push(
                                                `Фильтр precision = ${precision}`,
                                            );
                                        }
                                        return lines.join("\n");
                                    })()}
                                >
                                    <div className="whitespace-pre leading-tight">
                                            <span className="block max-w-[150px] truncate">
                                                {algo.algorithmName}
                                            </span>
                                        <span className="text-[9px] text-textDim/70">
                                                {algo.m != null
                                                    ? `m=${algo.m}`
                                                    : "m=∅"}
                                            </span>
                                        {algo.argsSummary && (
                                            <div className="mt-[1px] max-w-[150px] truncate text-[8px] text-textDim/60">
                                                {algo.argsSummary}
                                            </div>
                                        )}
                                    </div>
                                </th>

                                {/* Агрегатные столбцы */}
                                <td className="sticky left-[140px] z-20 min-w-[52px] border border-border bg-panel/80 px-[4px] py-[2px] text-center text-[10px]">
                                    {total}
                                </td>
                                <td className="sticky left-[192px] z-20 min-w-[52px] border border-border bg-panel/80 px-[4px] py-[2px] text-center text-[10px] text-emerald-200">
                                    {ok}
                                </td>
                                <td className="sticky left-[244px] z-20 min-w-[52px] border border-border bg-panel/80 px-[4px] py-[2px] text-center text-[10px] text-red-200">
                                    {err}
                                </td>
                                <td className="sticky left-[296px] z-20 min-w-[60px] border border-border bg-panel/80 px-[4px] py-[2px] text-center text-[10px]">
                                    {okPct != null
                                        ? `${okPct.toFixed(1)}%`
                                        : "—"}
                                </td>
                                <td className="sticky left-[356px] z-20 min-w-[60px] border border-border bg-panel/80 border-r-2 border-r-border px-[4px] py-[2px] text-center text-[10px]">
                                    {errPct != null
                                        ? `${errPct.toFixed(1)}%`
                                        : "—"}
                                </td>

                                {/* Столбцы по шагам n */}
                                {nListShown.map((n) => {
                                    const cellKey = `${algo.key}||${n}`;
                                    const count =
                                        cellMap.get(cellKey) ?? 0;
                                    const msgs =
                                        cellMessagesMap.get(cellKey) ??
                                        [];

                                    const tooltipLines: string[] = [];
                                    tooltipLines.push(
                                        `Алгоритм: ${algo.algorithmName}`,
                                    );
                                    tooltipLines.push(
                                        `шаг n = ${n}`,
                                    );
                                    tooltipLines.push(
                                        count > 0
                                            ? `количество ошибок: ${count}`
                                            : "ошибок нет",
                                    );

                                    if (msgs.length > 0) {
                                        tooltipLines.push("");
                                        tooltipLines.push(
                                            "Уникальные ошибки:",
                                        );
                                        for (const msg of msgs) {
                                            tooltipLines.push(
                                                `  • ${msg}`,
                                            );
                                        }
                                    }

                                    const title =
                                        tooltipLines.join("\n");

                                    return (
                                        <td
                                            key={cellKey}
                                            title={title}
                                            className={`min-w-[26px] border px-[2px] py-[2px] text-center text-[10px] cursor-default ${getCellClasses(
                                                count,
                                            )}`}
                                        >
                                            {count > 0
                                                ? count
                                                : "—"}
                                        </td>
                                    );
                                })}
                            </tr>
                        );
                    })}
                    </tbody>
                </table>
            </div>
        </div>
    );
};
