import React, { useEffect, useMemo, useRef, useState } from "react";
import { useConvergenceMatrix } from "../model/useConvergenceMatrix";
import { type SelectedCell, type SelectedDetail, type DetailPoint } from "../model/types";
import {
    errorNorm,
    realDiffSign,
    getPointsSortedByN,
    nonNullEntries,
    formatSideShort,
    formatMonotonicityShort,
    formatSideDescription,
    formatMonotonicityDescription,
} from "../model/convergenceUtils";
import { ConvergenceDetailChart } from "./ConvergenceDetailChart";

export interface AlgorithmSeriesConvergenceTableProps {
    experiment: import("../model/types").Experiment | null;
    className?: string;
    maxSeries?: number;
}

// убираем отдельный props-файл, чтобы не плодить мусор
// (оставлено определение выше)

export const AlgorithmSeriesConvergenceTable: React.FC<AlgorithmSeriesConvergenceTableProps> = ({
    experiment,
    className,
    maxSeries,
}) => {
    const { matrix, progress } = useConvergenceMatrix(experiment);
    const [page, setPage] = useState(0);
    const [selectedCell, setSelectedCell] = useState<SelectedCell | null>(null);
    const chartRef = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        setPage(0);
        setSelectedCell(null);
    }, [experiment]);

    const seriesList = matrix?.seriesList ?? [];
    const algoList = matrix?.algoList ?? [];
    const cells = matrix?.cells ?? {};

    const pageSize = maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;
    const totalPages = Math.max(1, Math.ceil((seriesList.length || 1) / pageSize));

    useEffect(() => {
        setPage((prev) => {
            if (prev >= totalPages) return totalPages - 1;
            if (prev < 0) return 0;
            return prev;
        });
    }, [totalPages]);

    useEffect(() => {
        if (selectedCell && chartRef.current) {
            chartRef.current.scrollIntoView({ behavior: "smooth", block: "start" });
        }
    }, [selectedCell]);

    const selectedDetail: SelectedDetail | null = useMemo(() => {
        if (!selectedCell || !experiment || !matrix) return null;

        const { seriesId, accelId } = selectedCell;

        const series = (experiment.seriesList ?? []).find((s) => s.id === seriesId) ?? null;
        const accel = (experiment.accelList ?? []).find((a) => a.id === accelId) ?? null;
        const sa =
            (experiment.seriesAccelList ?? []).find(
                (x) => x.series_id === seriesId && x.accel_id === accelId
            ) ?? null;

        const seriesInfo = matrix.seriesList.find((s) => s.key === seriesId) ?? null;
        const algoInfo = matrix.algoList.find((a) => a.key === accelId) ?? null;

        const analysis = matrix.cells[`${accelId}::${seriesId}`] ?? null;
        const limit = series?.limit ?? null;

        const points: DetailPoint[] = sa
            ? (() => {
                  const sorted = getPointsSortedByN(sa);
                  const pts: DetailPoint[] = [];

                  let prevVal: { re: number | null; im: number | null } | null = null;

                  for (const p of sorted) {
                      const valueRe = p.value?.re ?? null;
                      const valueImRaw = p.value?.im ?? null;
                      const valueIm = valueImRaw ?? 0;
                      const err = errorNorm(p.value, limit);
                      const sign = realDiffSign(p.value, limit);

                      let diffRe: number | null = null;
                      let diffIm: number | null = null;
                      let diffNorm: number | null = null;

                      if (
                          prevVal &&
                          valueRe != null &&
                          Number.isFinite(valueRe) &&
                          prevVal.re != null &&
                          Number.isFinite(prevVal.re)
                      ) {
                          const prevIm = prevVal.im ?? 0;
                          const dRe = valueRe - prevVal.re;
                          const dIm = valueIm - prevIm;
                          const dn = Math.hypot(dRe, dIm);
                          diffRe = dRe;
                          diffIm = dIm;
                          diffNorm = Number.isFinite(dn) ? dn : null;
                      }

                      prevVal = { re: valueRe, im: valueImRaw };

                      pts.push({
                          n: p.n,
                          valueRe,
                          valueIm: valueImRaw,
                          err,
                          sign,
                          diffRe,
                          diffIm,
                          diffNorm,
                      });
                  }

                  return pts;
              })()
            : [];

        return {
            seriesInfo,
            algoInfo,
            series,
            accel,
            analysis,
            limit,
            points,
        };
    }, [selectedCell, experiment, matrix]);

    if (!experiment) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет данных для анализа (experiment = null).
                </div>
            </div>
        );
    }

    if (!matrix || progress.running) {
        const { current, total } = progress;
        const pct = total > 0 ? Math.max(0, Math.min(100, Math.round((current / total) * 100))) : 0;

        return (
            <div className={className}>
                <div className="rounded-xl border border-border bg-panel p-4 shadow-panel">
                    <div className="mb-2 flex items-center justify-between text-sm text-textDim">
                        <span>Подсчёт монотонности и направления...</span>
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

    if (seriesList.length === 0 || algoList.length === 0) {
        return (
            <div className={className}>
                <div className="text-textDim text-sm">
                    Нет пар ряд × алгоритм для анализа (seriesList или accelList пусты).
                </div>
            </div>
        );
    }

    const startIndex = page * pageSize;
    const endIndex = Math.min(startIndex + pageSize, seriesList.length);
    const seriesSlice = seriesList.slice(startIndex, endIndex);

    return (
        <div className={className}>
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

                                    const colorClass = getCellColorClassSafe(
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
                                                setSelectedCell({
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

            <div ref={chartRef}>
                {selectedDetail && selectedDetail.analysis && (
                    <ConvergenceDetailChart detail={selectedDetail} />
                )}
            </div>
        </div>
    );
};

function getCellColorClassSafe(
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
