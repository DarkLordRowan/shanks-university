import React, { useEffect, useState } from "react";
import {
    type ConvergenceMatrix,
    type MonotonicityType,
    type SelectedCell,
    type SideType,
} from "../model/types";
import {
    formatMonotonicityShort,
    formatSideShort,
    nonNullEntries,
} from "../model/convergenceUtils";

interface ConvergenceMatrixTableProps {
    matrix: ConvergenceMatrix;
    maxSeries?: number;
    selectedCell: SelectedCell | null;
    onCellSelect: (cell: SelectedCell) => void;
}

/** Единый способ построить id для DOM-элемента ячейки. */
export function getConvergenceCellDomId(accelId: string, seriesId: string): string {
    return `conv-cell-${accelId}::${seriesId}`;
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
                                                —{" "}
                                            </td>
                                        );
                                    }

                                    // Вверху файла уже есть импорты. Ничего не трогаем.

                                    // Внутри .map по seriesSlice, в месте, где сейчас строится title:

                                    const sideShort = formatSideShort(analysis.side);
                                    const monShort = formatMonotonicityShort(analysis.monotonicity);

                                    // дополнительные поля, если они есть в анализе
                                    type Interval = { fromN: number; toN: number };
                                    type ExtraAnalysis = {
                                        signChangeNs?: number[];
                                        growthIntervals?: Interval[];
                                        flatIntervals?: Interval[];
                                    };

                                    const extra = analysis as unknown as ExtraAnalysis;
                                    const signChangeNs = extra.signChangeNs ?? [];
                                    const growthIntervals = extra.growthIntervals ?? [];
                                    const flatIntervals = extra.flatIntervals ?? [];

                                    const titleLines: string[] = [];

                                    // ====== Блок о ряде ======
                                    titleLines.push("Ряд:");
                                    titleLines.push(`  имя: ${s.seriesName}`);
                                    titleLines.push(`  x: ${s.xLabel}`);
                                    titleLines.push(`  точность: ${s.precision}`);

                                    const sAny = s as any;
                                    if (sAny.seriesArgs) {
                                        const entries = nonNullEntries(
                                            sAny.seriesArgs as Record<string, unknown>
                                        );
                                        if (entries.length > 0) {
                                            titleLines.push("  параметры ряда:");
                                            for (const [k, v] of entries.sort(([a], [b]) =>
                                                a.localeCompare(b)
                                            )) {
                                                titleLines.push(`    ${k}: ${String(v)}`);
                                            }
                                        }
                                    }

                                    titleLines.push("");

                                    // ====== Блок об алгоритме ======
                                    titleLines.push("Алгоритм:");
                                    titleLines.push(`  имя: ${algo.algorithmName}`);
                                    titleLines.push(
                                        `  m: ${algo.m != null ? String(algo.m) : "∅"}`
                                    );

                                    const algoEntries = nonNullEntries(algo.algorithmArgs);
                                    if (algoEntries.length > 0) {
                                        titleLines.push("  параметры алгоритма:");
                                        for (const [k, v] of algoEntries.sort(([a], [b]) =>
                                            a.localeCompare(b)
                                        )) {
                                            titleLines.push(`    ${k}: ${String(v)}`);
                                        }
                                    }
                                    if (algo.argsSummary) {
                                        titleLines.push(`  кратко: ${algo.argsSummary}`);
                                    }

                                    titleLines.push("");

                                    // ====== Направление + монотонность ======
                                    const isMono =
                                        analysis.monotonicity === "strict_decreasing_error" ||
                                        analysis.monotonicity === "non_increasing_error" ||
                                        analysis.monotonicity === "constant_error";

                                    const sideDescr =
                                        analysis.side === "one_sided"
                                            ? "одностороннее приближение"
                                            : analysis.side === "two_sided"
                                              ? "двустороннее приближение"
                                              : analysis.side === "no_limit"
                                                ? "предел не просматривается"
                                                : "тип направления не определён";

                                    const monoDescr = isMono
                                        ? "монотонная ошибка"
                                        : "не монотонная ошибка";

                                    titleLines.push(
                                        `Направление: ${sideDescr}; ${monoDescr} (обозначения в ячейке: ${sideShort} | ${monShort}).`
                                    );

                                    // ====== Смена знака ошибки ======
                                    if (analysis.signChangesCount > 0) {
                                        if (signChangeNs.length > 0) {
                                            const intervalsStr =
                                                formatIntervalsFromNs(signChangeNs);
                                            titleLines.push(
                                                `Смена знака ошибки: ${analysis.signChangesCount} раз; интервалы по n: ${intervalsStr}.`
                                            );
                                        } else {
                                            titleLines.push(
                                                `Смена знака ошибки: ${analysis.signChangesCount} раз.`
                                            );
                                        }
                                    } else {
                                        titleLines.push("Смена знака ошибки не происходит.");
                                    }

                                    // ====== Рост / неизменность ошибки по n ======
                                    if (growthIntervals.length > 0) {
                                        const giStr = formatIntervals(growthIntervals);
                                        titleLines.push(`Интервалы роста ошибки по n: ${giStr}.`);
                                    } else if (analysis.monotonicity === "has_growth") {
                                        // рост есть, но интервалов не дали
                                        titleLines.push(
                                            "Ошибка местами растёт по n (интервалы не указаны)."
                                        );
                                    }

                                    if (flatIntervals.length > 0) {
                                        const fiStr = formatIntervals(flatIntervals);
                                        titleLines.push(
                                            `Интервалы, где ошибка не меняется: ${fiStr}.`
                                        );
                                    } else if (analysis.monotonicity === "constant_error") {
                                        titleLines.push(
                                            "Ошибка не меняется на всем рассмотренном диапазоне n."
                                        );
                                    }

                                    titleLines.push(
                                        `Число проанализированных шагов: ${analysis.stepsAnalyzed}.`
                                    );
                                    titleLines.push("");
                                    titleLines.push(
                                        "Нажмите по ячейке, чтобы посмотреть детальный график."
                                    );

                                    const title = titleLines.join("\n");

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

                                    const domId = getConvergenceCellDomId(algo.key, s.key);

                                    return (
                                        <td
                                            key={key}
                                            id={domId}
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

function isMonotone(mon: MonotonicityType): boolean {
    return (
        mon === "strict_decreasing_error" ||
        mon === "non_increasing_error" ||
        mon === "constant_error"
    );
}

function hasUsableData(mon: MonotonicityType): boolean {
    return mon !== "not_enough_data" && mon !== "no_limit";
}

/**
 * Цвета по правилам:
 *  - односторонний и монотонный  → зелёный
 *  - односторонний и немонотонный → синий
 *  - двусторонний и монотонный   → жёлтый
 *  - двусторонний и немонотонный → красный
 * Остальное (unknown/no_limit/нет данных) — серые.
 */
function getCellColorClass(side: SideType, mon: MonotonicityType, selected: boolean): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (!hasUsableData(mon) || side === "no_limit") {
        return "border-border/60 text-textDim/70 bg-surface/30 hover:bg-surface/40" + sel;
    }

    const mono = isMonotone(mon);

    // односторонний
    if (side === "one_sided" && mono) {
        // зелёный
        return "border-border text-textDim bg-emerald-500/25 hover:bg-emerald-500/35" + sel;
    }
    if (side === "one_sided" && !mono) {
        // синий
        return "border-border text-textDim bg-blue-500/25 hover:bg-blue-500/35" + sel;
    }

    // двусторонний
    if (side === "two_sided" && mono) {
        // жёлтый
        return "border-border text-textDim bg-amber-300/35 hover:bg-amber-300/45" + sel;
    }
    if (side === "two_sided" && !mono) {
        // красный
        return "border-border text-textDim bg-red-500/30 hover:bg-red-500/40" + sel;
    }

    // unknown и прочий мусор — нейтральный
    return "border-border text-textDim bg-surface/40 hover:bg-surface/50" + sel;
}

type Interval = { fromN: number; toN: number };

function formatIntervals(intervals: Interval[]): string {
    if (!intervals.length) return "—";
    return intervals
        .map((iv) => (iv.fromN === iv.toN ? `n = ${iv.fromN}` : `n ∈ [${iv.fromN}; ${iv.toN}]`))
        .join(", ");
}

function formatIntervalsFromNs(ns: number[]): string {
    if (!ns.length) return "—";
    const sorted = [...ns].sort((a, b) => a - b);
    const intervals: Interval[] = [];

    let start = sorted[0];
    let prev = sorted[0];

    for (let i = 1; i < sorted.length; i++) {
        const cur = sorted[i];
        if (cur === prev + 1) {
            prev = cur;
            continue;
        }
        intervals.push({ fromN: start, toN: prev });
        start = cur;
        prev = cur;
    }
    intervals.push({ fromN: start, toN: prev });

    return formatIntervals(intervals);
}
