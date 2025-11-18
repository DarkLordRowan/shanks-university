import React, { useMemo, useState, useEffect } from "react";
import type { Item } from "../types/item";
import { buildErrorMatrix } from "../analysis/buildErrorMatrix";

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
    items: Item[];
    maxSteps?: number;
    className?: string;
}

export const ErrorMatrixTable: React.FC<ErrorMatrixTableProps> = ({
                                                                      items,
                                                                      maxSteps,
                                                                      className,
                                                                  }) => {
    const { nList, algoList, cellMap, algoStats, totalErrorItems } = useMemo(
        () => buildErrorMatrix(items),
        [items],
    );

    // Хуки ВСЕГДА идут подряд и без условий
    const [page, setPage] = useState(0);

    const pageSize =
        maxSteps && maxSteps > 0 ? maxSteps : nList.length || 1;

    const totalPages = Math.max(
        1,
        Math.ceil(nList.length / pageSize || 1),
    );

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

    // А уже после хуков можно условно рендерить "нет данных"
    if (nList.length === 0 || algoList.length === 0) {
        return (
            <div className={className}>
                <p className="text-xs text-textDim/60">
                    Ошибок с заданным шагом n не обнаружено.
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
                        Алгоритмы: {algoList.length} · Шаги n:{" "}
                        {nListShown.length} из {nList.length} ·
                        Элементов с ошибкой (с n): {totalErrorItems} ·
                        Всего элементов: {items.length}
                    </div>

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
                                        setPage((p) => Math.max(0, p - 1))
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
                                            Math.min(totalPages - 1, p + 1),
                                        )
                                    }
                                    disabled={page >= totalPages - 1}
                                >
                                    ›
                                </button>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() => setPage(totalPages - 1)}
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
                        <th className="sticky left-[140px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[52px]">
                            Всего
                        </th>
                        <th className="sticky left-[192px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[52px]">
                            OK
                        </th>
                        <th className="sticky left-[244px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[52px]">
                            Err
                        </th>
                        <th className="sticky left-[296px] top-0 z-20 border border-border bg-panel/95 px-1 py-1 text-center align-bottom text-[10px] min-w-[60px]">
                            % OK
                        </th>
                        <th className="sticky left-[356px] top-0 z-20 border border-border bg-panel/95 border-r-2 border-r-border px-1 py-1 text-center align-bottom text-[10px] min-w-[60px]">
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
                    {algoList.map((algo) => {
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
                                            lines.push("Аргументы:");
                                            const entries = Object.entries(
                                                algo.algorithmArgs,
                                            ).sort(([a, b]) =>
                                                a.localeCompare(b),
                                            );
                                            for (const [k, v] of entries) {
                                                lines.push(`  ${k}: ${v}`);
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

                                    const title =
                                        count > 0
                                            ? `Алгоритм: ${algo.algorithmName}\nшаг n = ${n}\nколичество ошибок: ${count}`
                                            : `Алгоритм: ${algo.algorithmName}\nшаг n = ${n}\nошибок нет`;

                                    return (
                                        <td
                                            key={cellKey}
                                            title={title}
                                            className={`min-w-[26px] border px-[2px] py-[2px] text-center text-[10px] cursor-default ${getCellClasses(
                                                count,
                                            )}`}
                                        >
                                            {count > 0 ? count : "—"}
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
