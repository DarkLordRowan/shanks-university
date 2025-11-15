import { useMemo, useState, useEffect } from "react";
import type { Item } from "../types/item";

type SeriesKey = string;
type AlgoKey = string;

interface SeriesInfo {
    key: SeriesKey;
    seriesName: string;
    x: number;
}

interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
}

function buildSeriesKey(seriesName: string, x: number): SeriesKey {
    return `${seriesName} | x=${x}`;
}

function buildSeriesLabel(info: SeriesInfo): string {
    return `${info.seriesName}\n x = ${info.x}`;
}

function buildAlgoLabel(info: AlgoInfo): string {
    const mPart = info.m != null ? `m = ${info.m}` : "m = ∅";
    return `${info.algorithmName}\n${mPart}`;
}

function buildArgsSummary(args: Item["algorithm"]["algorithmArgs"]): string {
    if (!args || Object.keys(args).length === 0) return "";
    const entries = Object.entries(args).sort(([a], [b]) =>
        a.localeCompare(b),
    );
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

export function AlgorithmSeriesErrorMatrix({
                                               items,
                                               maxSeries, // размер страницы по столбцам; если не задан, показываем все
                                           }: {
    items: Item[];
    maxSeries?: number;
}) {
    const { seriesList, algoList, cellMap } = useMemo(() => {
        const seriesMap = new Map<SeriesKey, SeriesInfo>();
        const algoMap = new Map<AlgoKey, AlgoInfo>();
        const cells = new Map<string, Item>();

        for (const it of items) {
            const sKey = buildSeriesKey(it.series.seriesName, it.series.x);
            if (!seriesMap.has(sKey)) {
                seriesMap.set(sKey, {
                    key: sKey,
                    seriesName: it.series.seriesName,
                    x: it.series.x,
                });
            }

            const algoKey: AlgoKey = it.algorithm.algorithmId;
            if (!algoMap.has(algoKey)) {
                algoMap.set(algoKey, {
                    key: algoKey,
                    algorithmName: it.algorithm.algorithmName,
                    m: it.algorithm.m,
                    argsSummary: buildArgsSummary(it.algorithm.algorithmArgs),
                });
            }

            const cellKey = `${algoKey}||${sKey}`;
            if (!cells.has(cellKey)) {
                cells.set(cellKey, it);
            }
        }

        const seriesList = Array.from(seriesMap.values()).sort((a, b) =>
            a.seriesName.localeCompare(b.seriesName) || a.x - b.x,
        );

        const algoList = Array.from(algoMap.values()).sort((a, b) =>
            a.algorithmName.localeCompare(b.algorithmName) ||
            (a.m ?? 0) - (b.m ?? 0),
        );

        return {
            seriesList,
            algoList,
            cellMap: cells,
        };
    }, [items]);

    // --- постраничный просмотр по столбцам (рядам) ---
    const [page, setPage] = useState(0);

    const pageSize =
        maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;

    const totalPages = Math.max(
        1,
        Math.ceil(seriesList.length / pageSize || 1),
    );

    // при изменении количества рядов/размера страницы сдвигаем текущую страницу, чтобы не вылезти за пределы
    useEffect(() => {
        if (page > totalPages - 1) {
            setPage(totalPages - 1);
        }
    }, [page, totalPages]);

    const startIndex = page * pageSize;
    const endIndex = startIndex + pageSize;
    const seriesListShown =
        maxSeries && maxSeries > 0
            ? seriesList.slice(startIndex, endIndex)
            : seriesList;

    return (
        <div className="space-y-2">
            <div className="flex items-center justify-between text-xs text-textDim">
                <h2 className="text-sm font-semibold text-textDim">
                    Матрица ошибок: алгоритмы × ряды
                </h2>
                <div className="flex items-center gap-3">
                    <div>
                        Алгоритмы: {algoList.length} · Ряды:{" "}
                        {seriesListShown.length} из {seriesList.length} ·
                        Элементов: {items.length}
                    </div>

                    {maxSeries && maxSeries > 0 && seriesList.length > maxSeries && (
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
                                колонки {startIndex + 1}–
                                {Math.min(endIndex, seriesList.length)}
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

            <div className="overflow-auto rounded-xl2 border border-border bg-panel shadow-panel">
                <table className="border-collapse text-[10px] leading-tight text-textDim">
                    <thead className="bg-surface/80">
                    <tr>
                        <th className="sticky left-0 top-0 z-20 border border-border bg-surface/90 px-1 py-1 text-left align-bottom text-[10px]">
                            Алгоритм \ Ряд
                        </th>
                        {seriesListShown.map((s) => (
                            <th
                                key={s.key}
                                className="border border-border px-1 py-[2px] text-center align-bottom"
                                title={buildSeriesLabel(s)}
                            >
                                <div className="whitespace-pre leading-tight">
                                        <span className="block max-w-[110px] truncate">
                                            {s.seriesName}
                                        </span>
                                    <span className="text-[9px] text-textDim/70">
                                            x={s.x}
                                        </span>
                                </div>
                            </th>
                        ))}
                    </tr>
                    </thead>
                    <tbody>
                    {algoList.map((algo) => (
                        <tr key={algo.key}>
                            <th
                                className="sticky left-0 z-10 border border-border bg-panel px-1 py-[2px] text-left align-top"
                                title={
                                    buildAlgoLabel(algo) +
                                    (algo.argsSummary
                                        ? `\n${algo.argsSummary}`
                                        : "")
                                }
                            >
                                <div className="whitespace-pre leading-tight">
                                        <span className="block max-w-[150px] truncate">
                                            {algo.algorithmName}
                                        </span>
                                    <span className="text-[9px] text-textDim/70">
                                            {algo.m != null ? `m=${algo.m}` : "m=∅"}
                                        </span>
                                    {algo.argsSummary && (
                                        <div className="mt-[1px] max-w-[150px] truncate text-[8px] text-textDim/60">
                                            {algo.argsSummary}
                                        </div>
                                    )}
                                </div>
                            </th>

                            {seriesListShown.map((s) => {
                                const cellKey = `${algo.key}||${s.key}`;
                                const item = cellMap.get(cellKey);

                                if (!item) {
                                    return (
                                        <td
                                            key={cellKey}
                                            className="min-w-[28px] border border-border bg-surface/40 px-[2px] py-[2px] text-center text-[9px] text-textDim/40"
                                        >
                                            —
                                        </td>
                                    );
                                }

                                const hasError = !!item.error;
                                const errorN = item.error?.data?.n ?? null;

                                const bgClass = hasError
                                    ? "bg-red-900/40 hover:bg-red-800/60"
                                    : "bg-emerald-900/40 hover:bg-emerald-800/60";

                                const borderClass = hasError
                                    ? "border-red-500/70"
                                    : "border-emerald-500/70";

                                const tooltipLines: string[] = [];

                                tooltipLines.push(
                                    `Ряд: ${item.series.seriesName}`,
                                );
                                tooltipLines.push(`x = ${item.series.x}`);
                                if (item.series.seriesLim != null) {
                                    tooltipLines.push(
                                        `lim = ${item.series.seriesLim}`,
                                    );
                                }

                                tooltipLines.push("");
                                tooltipLines.push(
                                    `Алгоритм: ${item.algorithm.algorithmName}`,
                                );
                                tooltipLines.push(
                                    `m = ${
                                        item.algorithm.m != null
                                            ? item.algorithm.m
                                            : "∅"
                                    }`,
                                );

                                if (
                                    item.algorithm.algorithmArgs &&
                                    Object.keys(
                                        item.algorithm.algorithmArgs,
                                    ).length > 0
                                ) {
                                    tooltipLines.push("Аргументы:");
                                    const entries = Object.entries(
                                        item.algorithm.algorithmArgs,
                                    ).sort(([a], [b]) =>
                                        a.localeCompare(b),
                                    );
                                    for (const [k, v] of entries) {
                                        tooltipLines.push(`  ${k}: ${v}`);
                                    }
                                }

                                if (hasError && item.error) {
                                    tooltipLines.push("");
                                    tooltipLines.push(
                                        `Ошибка: ${item.error.description}`,
                                    );
                                    if (errorN != null) {
                                        tooltipLines.push(
                                            `На шаге n = ${errorN}`,
                                        );
                                    }
                                }

                                const title = tooltipLines.join("\n");

                                return (
                                    <td
                                        key={cellKey}
                                        title={title}
                                        className={`min-w-[28px] border px-[2px] py-[2px] text-center text-[10px] cursor-default ${borderClass} ${bgClass}`}
                                    >
                                        {hasError ? (
                                            <span className="font-semibold text-red-200">
                                                    {errorN != null
                                                        ? `n=${errorN}`
                                                        : "err"}
                                                </span>
                                        ) : (
                                            <span className="font-semibold text-emerald-200">
                                                    ✓
                                                </span>
                                        )}
                                    </td>
                                );
                            })}
                        </tr>
                    ))}
                    </tbody>
                </table>
            </div>
        </div>
    );
}
