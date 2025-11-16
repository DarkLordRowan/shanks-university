import { useEffect, useMemo, useState } from "react";
import type { DiffCell } from "../types/diffCell";

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
    algorithmArgs: DiffCell["algorithmArgs"] | null;
}

function buildSeriesLabel(info: SeriesInfo): string {
    return `${info.seriesName}\n x = ${info.x}`;
}

function buildArgsSummary(args: DiffCell["algorithmArgs"] | null): string {
    if (!args || Object.keys(args).length === 0) return "";
    const entries = Object.entries(args).sort(([a, b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

interface ColorSpec {
    bgClass: string;
    borderClass: string;
    label: string;
}

/**
 * Классификация по шагам ошибок:
 * - ok = !hasError
 * - err на шаге n = hasError && errorN != null
 */
function classifyErrorChange(
    hasPrevErr: boolean,
    nPrev: number | null,
    hasNextErr: boolean,
    nNext: number | null,
): ColorSpec {
    // нет данных вообще
    if (!hasPrevErr && nPrev == null && !hasNextErr && nNext == null) {
        return {
            bgClass: "bg-surface/40",
            borderClass: "border-border/50",
            label: "нет данных",
        };
    }

    const prevOk = !hasPrevErr;
    const nextOk = !hasNextErr;

    // ok → ok
    if (prevOk && nextOk) {
        return {
            bgClass: "bg-yellow-500/90 hover:bg-yellow-400/90",
            borderClass: "border-yellow-200",
            label: "ok → ok (ошибки нет в обоих)",
        };
    }

    // err → ok (исчезла ошибка)
    if (hasPrevErr && !prevOk && nextOk) {
        return {
            bgClass: "bg-green-700/80 hover:bg-green-600/80",
            borderClass: "border-green-300/90",
            label: "ошибка → ok (исчезла ошибка)",
        };
    }

    // ok → err (появилась ошибка)
    if (prevOk && hasNextErr && !nextOk) {
        return {
            bgClass: "bg-purple-800/80 hover:bg-purple-700/80",
            borderClass: "border-purple-400/80",
            label: "ok → ошибка (появилась ошибка)",
        };
    }

    // обе с ошибкой, но шаги неизвестны
    if (nPrev == null || nNext == null) {
        return {
            bgClass: "bg-slate-700/70 hover:bg-slate-600/70",
            borderClass: "border-slate-400/80",
            label: "ошибка без шага n",
        };
    }

    // обе с ошибкой, шаги известны
    if (nPrev === nNext) {
        return {
            bgClass: "bg-yellow-700/70 hover:bg-yellow-600/80",
            borderClass: "border-yellow-300/80",
            label: "ошибка на том же шаге",
        };
    }

    const delta = nNext - nPrev; // >0 — ошибка позже, <0 — раньше
    const absDelta = Math.abs(delta);

    const level =
        absDelta >= 1000 ? "900/80" :
            absDelta >= 100  ? "800/70" :
                absDelta >= 10   ? "700/60" :
                    "600/50";

    if (delta > 0) {
        // ошибка позже (улучшение)
        return {
            bgClass: `bg-green-${level} hover:bg-green-500/60`,
            borderClass: "border-green-400/80",
            label: `ошибка сдвинулась вперёд (Δn = +${delta})`,
        };
    } else {
        // ошибка раньше (ухудшение)
        return {
            bgClass: `bg-red-${level} hover:bg-red-500/60`,
            borderClass: "border-red-400/80",
            label: `ошибка произошла раньше (Δn = ${delta})`,
        };
    }
}

export function AlgorithmSeriesDiffHeatmap({
                                               cells,
                                               maxSeries,
                                           }: {
    cells: DiffCell[];
    maxSeries?: number;
}) {
    const { seriesList, algoList, cellMap } = useMemo(() => {
        const seriesMap = new Map<SeriesKey, SeriesInfo>();
        const algoMap = new Map<AlgoKey, AlgoInfo>();
        const cellMap = new Map<string, DiffCell>();

        for (const c of cells) {
            if (!seriesMap.has(c.seriesKey)) {
                seriesMap.set(c.seriesKey, {
                    key: c.seriesKey,
                    seriesName: c.seriesName,
                    x: c.x,
                });
            }

            if (!algoMap.has(c.algoKey)) {
                const args = c.algorithmArgs ?? null;
                algoMap.set(c.algoKey, {
                    key: c.algoKey,
                    algorithmName: c.algorithmName,
                    m: c.m,
                    argsSummary: buildArgsSummary(args),
                    algorithmArgs: args,
                });
            }

            const cellKey = `${c.algoKey}||${c.seriesKey}`;
            if (!cellMap.has(cellKey)) {
                cellMap.set(cellKey, c);
            }
        }

        const seriesList = Array.from(seriesMap.values()).sort(
            (a, b) =>
                a.seriesName.localeCompare(b.seriesName) || a.x - b.x,
        );

        const algoList = Array.from(algoMap.values()).sort(
            (a, b) =>
                a.algorithmName.localeCompare(b.algorithmName) ||
                (a.m ?? 0) - (b.m ?? 0),
        );

        return { seriesList, algoList, cellMap };
    }, [cells]);

    const [page, setPage] = useState(0);

    const pageSize =
        maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;

    const totalPages = Math.max(
        1,
        Math.ceil(seriesList.length / pageSize || 1),
    );

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
                    Хитмапа разницы по шагам ошибок: алгоритмы × ряды
                </h2>
                <div className="flex items-center gap-3">
                    <div>
                        Алгоритмы: {algoList.length} · Ряды:{" "}
                        {seriesListShown.length} из {seriesList.length}
                    </div>

                    {maxSeries &&
                        maxSeries > 0 &&
                        seriesList.length > maxSeries && (
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
                                className="border border-border px-0 py-0 text-center align-bottom"
                                title={buildSeriesLabel(s)}
                            >
                                <div className="flex h-28 w-[32px] flex-col items-center justify-end gap-1">
                                    <div className="origin-bottom-left -rotate-90 whitespace-nowrap text-[9px] leading-tight">
                                        {s.seriesName}
                                    </div>
                                    <div className="text-[8px] text-textDim/70">
                                        x={s.x}
                                    </div>
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
                                title={(() => {
                                    const lines: string[] = [];
                                    lines.push(
                                        `Алгоритм: ${algo.algorithmName}`,
                                    );
                                    lines.push(
                                        `m = ${
                                            algo.m != null ? algo.m : "∅"
                                        }`,
                                    );
                                    if (
                                        algo.algorithmArgs &&
                                        Object.keys(
                                            algo.algorithmArgs,
                                        ).length > 0
                                    ) {
                                        lines.push("Аргументы:");
                                        for (const [k, v] of Object.entries(
                                            algo.algorithmArgs,
                                        ).sort(([a, b]) =>
                                            a.localeCompare(b),
                                        )) {
                                            lines.push(`  ${k}: ${v}`);
                                        }
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

                            {seriesListShown.map((s) => {
                                const cellKey = `${algo.key}||${s.key}`;
                                const cell = cellMap.get(cellKey) ?? null;

                                const hasPrevErr = !!cell?.hasErrorPrev;
                                const hasNextErr = !!cell?.hasErrorNext;
                                const nPrev = cell?.errorNPrev ?? null;
                                const nNext = cell?.errorNNext ?? null;

                                const { bgClass, borderClass, label } =
                                    classifyErrorChange(
                                        hasPrevErr,
                                        nPrev,
                                        hasNextErr,
                                        nNext,
                                    );

                                let deltaText: string | null = null;
                                if (!hasPrevErr && !hasNextErr) {
                                    deltaText = "ok → ok";
                                } else if (hasPrevErr && !hasNextErr) {
                                    deltaText = "err → ok";
                                } else if (!hasPrevErr && hasNextErr) {
                                    deltaText = "ok → err";
                                } else if (nPrev != null && nNext != null) {
                                    const delta = nNext - nPrev;
                                    deltaText = `n: ${nPrev} → ${nNext} (Δ=${delta >= 0 ? "+" : ""}${delta})`;
                                } else {
                                    deltaText = `n: ${
                                        nPrev ?? "∅"
                                    } → ${nNext ?? "∅"}`;
                                }

                                const tooltipLines: string[] = [];

                                tooltipLines.push(
                                    `Ряд: ${s.seriesName}, x=${s.x}`,
                                );
                                tooltipLines.push(
                                    `Алгоритм: ${algo.algorithmName}, m=${algo.m ?? "∅"}`,
                                );
                                tooltipLines.push("");

                                tooltipLines.push(
                                    `Старая ошибка: ${
                                        hasPrevErr
                                            ? nPrev != null
                                                ? `n = ${nPrev}`
                                                : "есть, n неизвестно"
                                            : "нет ошибки"
                                    }`,
                                );
                                tooltipLines.push(
                                    `Новая ошибка: ${
                                        hasNextErr
                                            ? nNext != null
                                                ? `n = ${nNext}`
                                                : "есть, n неизвестно"
                                            : "нет ошибки"
                                    }`,
                                );

                                tooltipLines.push("");
                                tooltipLines.push(label);

                                const title = tooltipLines.join("\n");

                                return (
                                    <td
                                        key={cellKey}
                                        title={title}
                                        className={`min-w-[30px] border px-[2px] py-[2px] text-center text-[10px] cursor-default ${borderClass} ${bgClass}`}
                                    >
                                        <div className="leading-tight">
                                            <div className="text-[9px]">
                                                {deltaText}
                                            </div>
                                        </div>
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
