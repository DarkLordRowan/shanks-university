import React, { useEffect, useState } from "react";
import { type ConvergenceMatrix, type SelectedCell } from "../model/types";
import {
    nonNullEntries,
    formatSideShort,
    formatMonotonicityShort,
    formatSideDescription,
    formatMonotonicityDescription,
} from "../model/convergenceUtils";

interface ConvergenceMatrixTableProps {
    matrix: ConvergenceMatrix;
    maxSeries?: number;
    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
}

export const ConvergenceMatrixTable: React.FC<ConvergenceMatrixTableProps> = ({
    matrix,
    maxSeries,
    selectedCell,
    onCellSelect,
}) => {
    const seriesList = matrix.seriesList ?? [];
    const algoList = matrix.algoList ?? [];
    const cells = matrix.cells ?? {};

    const [page, setPage] = useState(0);

    const pageSize = maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;
    const totalPages = Math.max(1, Math.ceil((seriesList.length || 1) / pageSize));

    useEffect(() => {
        setPage(0);
    }, [matrix, maxSeries]);

    useEffect(() => {
        setPage((prev) => {
            if (prev >= totalPages) return totalPages - 1;
            if (prev < 0) return 0;
            return prev;
        });
    }, [totalPages]);

    if (seriesList.length === 0 || algoList.length === 0) {
        return (
            <div className="text-textDim text-sm">
                Нет пар ряд × алгоритм для анализа (seriesList или accelList пусты).
            </div>
        );
    }

    const startIndex = page * pageSize;
    const endIndex = Math.min(startIndex + pageSize, seriesList.length);
    const seriesSlice = seriesList.slice(startIndex, endIndex);

    return (
        <>
            <div className="mb-2 flex items-center justify-between text-xs text-textDim">
                <div className="flex flex-col gap-1">
                    <span className="text-sm font-semibold text-textDim">
                        Монотонность и направление: алгоритмы × ряды
                    </span>
                    <span className="text-[11px] text-textDim/80">
                        Алгоритмы: {algoList.length} · Ряды: {seriesList.length}
                    </span>
                </div>

                {seriesList.length > pageSize && (
                    <div className="flex items-center gap-1 text-[10px]">
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                            onClick={() => setPage(0)}
                            disabled={page === 0}
                        >
                            «
                        </button>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                            onClick={() => setPage((p) => Math.max(0, p - 1))}
                            disabled={page === 0}
                        >
                            ‹
                        </button>
                        <span className="px-1">
                            стр. {page + 1} / {totalPages}
                        </span>
                        <span className="text-textDim/60">
                            колонки {startIndex + 1}–{endIndex}
                        </span>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                            onClick={() => setPage((p) => Math.min(totalPages - 1, p + 1))}
                            disabled={page >= totalPages - 1}
                        >
                            ›
                        </button>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                            onClick={() => setPage(totalPages - 1)}
                            disabled={page >= totalPages - 1}
                        >
                            »
                        </button>
                    </div>
                )}
            </div>

            <div className="overflow-auto rounded-xl2 border border-border bg-panel shadow-panel">
                <table className="min-w-full border-collapse text-[10px] leading-tight text-textDim">
                    <thead className="bg-surface/80">
                        <tr>
                            <th className="sticky left-0 top-0 z-20 border border-border bg-surface/90 px-1 py-1 text-left text-[10px] align-bottom">
                                Алгоритм \ Ряд
                            </th>

                            {seriesSlice.map((s) => (
                                <th
                                    key={s.key}
                                    className="border border-border px-0 py-0 text-center align-bottom"
                                    title={`${s.seriesName}\n x = ${s.xLabel}\n prec = ${s.precision}`}
                                >
                                    <div className="relative flex h-28 w-[44px] items-center justify-center">
                                        <span className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 rotate-[-90deg] whitespace-nowrap text-[9px] leading-tight">
                                            {s.seriesName}
                                        </span>
                                        <span className="absolute bottom-3 text-[8px] text-textDim/70">
                                            x={s.xLabel}
                                        </span>
                                        <span className="absolute bottom-0 text-[8px] text-textDim/60">
                                            prec={s.precision}
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
                                    title={(() => {
                                        const lines: string[] = [];
                                        lines.push(`Алгоритм: ${algo.algorithmName}`);
                                        lines.push(`m = ${algo.m != null ? String(algo.m) : "∅"}`);
                                        const entries = nonNullEntries(algo.algorithmArgs);
                                        if (entries.length > 0) {
                                            lines.push("Аргументы:");
                                            for (const [k, v] of entries.sort(([a], [b]) =>
                                                a.localeCompare(b)
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
                                            {algo.m != null ? `m=${String(algo.m)}` : "m=∅"}
                                        </span>
                                        {algo.argsSummary && (
                                            <div className="mt-[1px] max-w-[150px] truncate text-[8px] text-textDim/60">
                                                {algo.argsSummary}
                                            </div>
                                        )}
                                    </div>
                                </th>

                                {seriesSlice.map((s) => {
                                    const key = `${algo.key}::${s.key}`;
                                    const analysis = cells[key];

                                    if (!analysis) {
                                        return (
                                            <td
                                                key={key}
                                                className="border border-border px-[2px] py-[2px] text-center text-[10px] text-textDim/50"
                                            >
                                                —
                                            </td>
                                        );
                                    }

                                    const sideShort = formatSideShort(analysis.side);
                                    const monShort = formatMonotonicityShort(analysis.monotonicity);

                                    const title = [
                                        `Ряд: ${s.seriesName}, x=${s.xLabel}, prec=${s.precision}`,
                                        `Алгоритм: ${algo.algorithmName}, m=${algo.m ?? "∅"}`,
                                        "",
                                        formatSideDescription(analysis.side),
                                        formatMonotonicityDescription(analysis.monotonicity),
                                        analysis.signChangesCount > 0
                                            ? `Число смен знака: ${analysis.signChangesCount}, первое при n = ${analysis.firstSignChangeN}.`
                                            : "Смен знака A_k - lim не обнаружено.",
                                        analysis.firstGrowthN != null
                                            ? `Первый рост ошибки при n = ${analysis.firstGrowthN}.`
                                            : "Рост ошибки |A_k - lim| не обнаружен или данных недостаточно.",
                                        `Сравнено шагов (пар): ${analysis.stepsAnalyzed}.`,
                                        "",
                                        "Нажмите по ячейке, чтобы посмотреть детальный график.",
                                    ].join("\n");

                                    const isSelected =
                                        selectedCell?.seriesId === s.key &&
                                        selectedCell?.accelId === algo.key;

                                    const baseCell =
                                        "min-w-[30px] border px-[2px] py-[2px] text-center text-[10px] cursor-pointer";

                                    const colorClass = getCellColorClass(
                                        analysis.side,
                                        analysis.monotonicity,
                                        isSelected
                                    );

                                    return (
                                        <td
                                            key={key}
                                            className={baseCell + " " + colorClass}
                                            title={title}
                                            onClick={() =>
                                                onCellSelect({
                                                    seriesId: s.key,
                                                    accelId: algo.key,
                                                })
                                            }
                                        >
                                            <div className="flex select-none flex-col items-center gap-[1px] leading-tight">
                                                <span className="font-mono text-[10px]">
                                                    {sideShort} | {monShort}
                                                </span>
                                                <span className="text-[9px] text-textDim/80">
                                                    k: {analysis.stepsAnalyzed}
                                                </span>
                                            </div>
                                        </td>
                                    );
                                })}
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
        </>
    );
};

function getCellColorClass(
    side: import("../model/types").SideType,
    mon: import("../model/types").MonotonicityType,
    selected: boolean
): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (mon === "strict_decreasing_error" || mon === "non_increasing_error") {
        if (side === "one_sided") {
            return "border-border text-textDim bg-emerald-500/25 hover:bg-emerald-500/35" + sel;
        }
        if (side === "two_sided") {
            return "border-border text-textDim bg-emerald-400/20 hover:bg-emerald-400/30" + sel;
        }
        if (side === "unknown") {
            return "border-border text-textDim bg-emerald-400/10 hover:bg-emerald-400/20" + sel;
        }
        if (side === "no_limit") {
            return "border-border/70 text-textDim/70 bg-surface/40 hover:bg-surface/50" + sel;
        }
    }

    if (mon === "constant_error") {
        if (side === "one_sided") {
            return "border-border text-textDim bg-surface/70 hover:bg-surface/60" + sel;
        }
        if (side === "two_sided") {
            return "border-border text-textDim bg-surface/60 hover:bg-surface/50" + sel;
        }
        return "border-border/70 text-textDim/70 bg-surface/50 hover:bg-surface/40" + sel;
    }

    if (mon === "has_growth") {
        if (side === "one_sided") {
            return "border-amber-500 text-textDim bg-amber-500/25 hover:bg-amber-500/35" + sel;
        }
        if (side === "two_sided") {
            return "border-red-500 text-textDim bg-red-500/30 hover:bg-red-500/40" + sel;
        }
        if (side === "unknown") {
            return "border-amber-500/80 text-textDim bg-amber-500/20 hover:bg-amber-500/30" + sel;
        }
        if (side === "no_limit") {
            return "border-red-500/70 text-textDim/80 bg-red-500/20 hover:bg-red-500/30" + sel;
        }
    }

    if (mon === "not_enough_data" || mon === "no_limit") {
        if (side === "no_limit") {
            return "border-border/60 text-textDim/60 bg-surface/30 hover:bg-surface/40" + sel;
        }
        return "border-border/50 text-textDim/60 bg-surface/40 hover:bg-surface/50" + sel;
    }

    return "border-border text-textDim bg-surface/40 hover:bg-surface/50" + sel;
}
