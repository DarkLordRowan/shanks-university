// src/widgets/AlgorithmSeriesErrorStatsTable/AlgorithmSeriesErrorStatsTable.tsx

import React, { useCallback, useMemo, useState } from "react";
import * as XLSX from "xlsx-js-style";
import type {
    Accel,
    Experiment,
    Series,
    SeriesAccel,
} from "@/entities/experiment/model/experiment";
import type { MatrixAxisItem } from "@/shared/ui/Matrix/Matrix";
import { computeErrorStats, type ErrorStats } from "./model/errorStats";
import { ErrorStatsCell, type HeatClass } from "./ui/ErrorStatsCell";
import { MatrixAlgorithmSeries } from "@/shared/ui/Matrix/MatrixAlgorithmSeries";

interface AlgorithmSeriesErrorStatsTableProps {
    experiment: Experiment | null;
    maxSeries?: number;
}

type Row = MatrixAxisItem<Accel>;
type Col = MatrixAxisItem<Series>;
type StatsIndex = Record<string, Record<string, ErrorStats | null>>;

function buildStatsIndex(seriesAccelList: SeriesAccel[] | undefined): StatsIndex {
    const index: StatsIndex = {};
    if (!seriesAccelList) return index;

    for (const sa of seriesAccelList) {
        const seriesId = sa.series_id;
        const accelId = sa.accel_id;

        const deviations = (sa.computed ?? []).map((p) => p?.deviation);
        const stats = computeErrorStats(deviations);

        if (!index[seriesId]) index[seriesId] = {};
        index[seriesId][accelId] = stats;
    }

    return index;
}

function getGlobalMax(statsIndex: StatsIndex): number {
    let g = 0;
    for (const seriesId of Object.keys(statsIndex)) {
        for (const accelId of Object.keys(statsIndex[seriesId])) {
            const st = statsIndex[seriesId][accelId];
            if (st && Number.isFinite(st.max) && st.max > g) g = st.max;
        }
    }
    return g;
}

function formatArgs(args: Record<string, any> | null | undefined): string {
    if (!args) return "";
    const entries = Object.entries(args).filter(([, v]) => v !== null && v !== undefined);
    if (entries.length === 0) return "";
    return entries.map(([k, v]) => `${k}=${typeof v === "string" ? v : String(v)}`).join(", ");
}

function classifyByMax(st: ErrorStats | null, globalMax: number): HeatClass {
    if (!st || !Number.isFinite(st.max) || st.count <= 0) return "neutral";
    if (!(globalMax > 0)) return "neutral";

    const x = Math.log10(st.max);
    const g = Math.log10(globalMax);

    const t = Math.min(1, Math.max(0, (x - (g - 6)) / 6));

    // чуть “шире” распределение по классам (больше различий)
    if (t < 0.2) return "ok";
    if (t < 0.45) return "warn";
    if (t < 0.7) return "bad";
    return "fatal";
}

function fmtExp(x: number | null | undefined, digits = 2): string {
    if (x == null || !Number.isFinite(x)) return "∅";
    return x.toExponential(digits);
}

export function getErrorStatsCellDomId(accelId: string, seriesId: string): string {
    return `errstats-cell-${accelId}::${seriesId}`;
}

export const AlgorithmSeriesErrorStatsTable: React.FC<AlgorithmSeriesErrorStatsTableProps> = ({
    experiment,
    maxSeries = 20,
}) => {
    const [selected, setSelected] = useState<{ accelId: string; seriesId: string } | null>(null);

    const rows: Row[] = useMemo(
        () => (experiment?.accelList ?? []).map((a) => ({ id: a.id, meta: a })),
        [experiment?.accelList]
    );

    const cols: Col[] = useMemo(
        () => (experiment?.seriesList ?? []).map((s) => ({ id: s.id, meta: s })),
        [experiment?.seriesList]
    );

    const statsIndex = useMemo(
        () => buildStatsIndex(experiment?.seriesAccelList),
        [experiment?.seriesAccelList]
    );

    const globalMax = useMemo(() => getGlobalMax(statsIndex), [statsIndex]);

    const buildWorkbook = useCallback(
        ({
            rows,
            cols,
        }: {
            rows: Row[];
            cols: Col[];
            pager: { startIndex: number; endIndex: number };
        }): XLSX.WorkBook => {
            const wb = XLSX.utils.book_new();

            const headerStyle: XLSX.CellStyle = {
                fill: { patternType: "solid", fgColor: { rgb: "0B1220" } },
                font: { color: { rgb: "E5E7EB" }, bold: true },
                alignment: { horizontal: "center", vertical: "center", wrapText: true },
                border: {
                    top: { style: "thin", color: { rgb: "374151" } },
                    bottom: { style: "thin", color: { rgb: "374151" } },
                    left: { style: "thin", color: { rgb: "374151" } },
                    right: { style: "thin", color: { rgb: "374151" } },
                },
            };

            const rowHeaderStyle: XLSX.CellStyle = {
                fill: { patternType: "solid", fgColor: { rgb: "0F172A" } },
                font: { color: { rgb: "E5E7EB" }, bold: true },
                alignment: { horizontal: "left", vertical: "top", wrapText: true },
                border: {
                    top: { style: "thin", color: { rgb: "374151" } },
                    bottom: { style: "thin", color: { rgb: "374151" } },
                    left: { style: "thin", color: { rgb: "374151" } },
                    right: { style: "thin", color: { rgb: "374151" } },
                },
            };

            const baseCell: XLSX.CellStyle = {
                alignment: { horizontal: "left", vertical: "top", wrapText: true },
                border: {
                    top: { style: "thin", color: { rgb: "374151" } },
                    bottom: { style: "thin", color: { rgb: "374151" } },
                    left: { style: "thin", color: { rgb: "374151" } },
                    right: { style: "thin", color: { rgb: "374151" } },
                },
            };

            const styleByHeat = (hc: HeatClass): XLSX.CellStyle => {
                // “сильнее” цвета, чтобы в Excel было заметнее
                const fg = (() => {
                    switch (hc) {
                        case "ok":
                            return "065F46"; // emerald-800-ish
                        case "warn":
                            return "4D7C0F"; // lime-700-ish
                        case "bad":
                            return "B45309"; // amber-700-ish
                        case "fatal":
                            return "991B1B"; // red-800-ish
                        case "neutral":
                        default:
                            return "111827"; // slate
                    }
                })();

                const fontColor = hc === "neutral" ? "9CA3AF" : "F9FAFB";

                return {
                    ...baseCell,
                    fill: { patternType: "solid", fgColor: { rgb: fg } },
                    font: { color: { rgb: fontColor }, bold: hc !== "neutral" },
                };
            };

            const aoa: (string | number | null)[][] = [];

            aoa.push([
                "Алгоритм \\ Ряд",
                ...cols.map((c) => {
                    const s = c.meta as any;
                    const name = c.meta?.name ?? c.id;
                    const x = s?.xLabel ?? s?.x ?? "∅";
                    const prec = s?.precision ?? "∅";
                    return `${name} (x=${String(x)}, prec=${String(prec)})`;
                }),
            ]);

            for (const r of rows) {
                const a = r.meta as any;
                const algoName = r.meta?.name ?? r.id;
                const m = a?.m != null ? `\nm=${String(a.m)}` : "";

                const line: (string | number | null)[] = [];
                line.push(`${algoName}${m}`);

                for (const c of cols) {
                    const st = statsIndex[c.id]?.[r.id] ?? null;
                    if (!st) {
                        line.push("—");
                        continue;
                    }
                    // порядок: max, mean, min
                    line.push(
                        `max=${fmtExp(st.max)}\nmean=${fmtExp(st.mean)}\nmin=${fmtExp(st.min)}\nn=${st.count}`
                    );
                }

                aoa.push(line);
            }

            const ws = XLSX.utils.aoa_to_sheet(aoa);

            ws["!cols"] = [{ wch: 34 }, ...cols.map(() => ({ wch: 30 }))];
            ws["!rows"] = [{ hpt: 30 }, ...rows.map(() => ({ hpt: 52 }))];

            const ref = ws["!ref"] || "A1:A1";
            const range = XLSX.utils.decode_range(ref);

            for (let C = range.s.c; C <= range.e.c; C++) {
                const addr = XLSX.utils.encode_cell({ r: 0, c: C });
                if (ws[addr]) ws[addr].s = headerStyle;
            }

            for (let R = 1; R <= range.e.r; R++) {
                const addr = XLSX.utils.encode_cell({ r: R, c: 0 });
                if (ws[addr]) ws[addr].s = rowHeaderStyle;
            }

            for (let R = 1; R <= range.e.r; R++) {
                for (let C = 1; C <= range.e.c; C++) {
                    const addr = XLSX.utils.encode_cell({ r: R, c: C });
                    const cell = ws[addr];
                    if (!cell) continue;

                    const rItem = rows[R - 1];
                    const cItem = cols[C - 1];
                    if (!rItem || !cItem) continue;

                    const st = statsIndex[cItem.id]?.[rItem.id] ?? null;
                    const hc = classifyByMax(st, globalMax);

                    cell.s = styleByHeat(hc);
                }
            }

            XLSX.utils.book_append_sheet(wb, ws, "error_stats");
            return wb;
        },
        [statsIndex, globalMax]
    );

    if (!experiment || rows.length === 0 || cols.length === 0) return null;

    return (
        <MatrixAlgorithmSeries
            accelList={experiment?.accelList ?? []}
            seriesList={experiment?.seriesList ?? []}
            maxColsPerPage={maxSeries}
            rowWidth={220}
            colWidth={90}
            minCellHeightPx={64}
            renderTitle={() => "Статистика ошибок: алгоритмы × ряды"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {rows.length} · Ряды: {cols.length} · global max:{" "}
                    <span className="font-mono tabular-nums">
                        {globalMax > 0 ? globalMax.toExponential(2) : "—"}
                    </span>
                </>
            )}
            export={{
                fileBaseName: "algorithm-series-error-stats",
                enablePng: true,
                enableXlsx: true,
                buildWorkbook: () => buildWorkbook(),
            }}
            renderCell={(row, col) => {
                const stats = statsIndex[col.id]?.[row.id] ?? null;
                const active = selected?.accelId === row.id && selected?.seriesId === col.id;
                const heatClass = classifyByMax(stats, globalMax);

                const a = row;
                const s = col;

                const algoName = a?.name ?? row.id;
                const argsA = formatArgs(a?.args) ?? [];
                const seriesName = s?.name ?? col.id;
                const argsS = formatArgs(s?.args) ?? "∅";
                const prec = s?.precision ?? "∅";

                const titleLines: string[] = [];
                titleLines.push(
                    `Ряд: ${seriesName} prec=${String(prec)}, \n args: ${String(argsS)}`
                );
                titleLines.push(
                    `Алгоритм: ${algoName}${a?.m != null ? `, m=${String(a.m)}` : ""} \n args: ${String(argsA)}`
                );

                titleLines.push("");

                if (!stats) {
                    titleLines.push("Нет данных по ошибке.");
                } else {
                    titleLines.push(`max=${stats.max}`);
                    titleLines.push(`mean=${stats.mean}`);
                    titleLines.push(`min=${stats.min}`);
                    titleLines.push(`n=${stats.count}`);
                }

                titleLines.push("");
                titleLines.push("Клик — выбрать ячейку.");

                const domId = getErrorStatsCellDomId(row.id, col.id);

                return (
                    <div id={domId} className="w-full h-full">
                        <ErrorStatsCell
                            stats={stats}
                            active={active}
                            heatClass={heatClass}
                            title={titleLines.join("\n")}
                            onClick={() => setSelected({ accelId: row.id, seriesId: col.id })}
                        />
                    </div>
                );
            }}
        />
    );
};
