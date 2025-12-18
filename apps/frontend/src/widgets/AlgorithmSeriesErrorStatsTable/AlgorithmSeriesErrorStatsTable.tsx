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
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";
import { computeErrorStats, type ErrorStats } from "./model/errorStats";
import { ErrorStatsCell, type HeatClass } from "./ui/ErrorStatsCell";

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
        <MatrixPaged<Accel, Series>
            resetKey={experiment.id}
            rows={rows}
            cols={cols}
            maxColsPerPage={maxSeries}
            maxBodyHeight="80vh"
            rowWidth={220}
            colWidth={90}
            minCellHeightPx={64}
            thClassName="px-0 py-0"
            tdClassName="px-0 py-0"
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
                buildWorkbook,
            }}
            renderCorner={() => (
                <div className="px-1 py-1 text-left text-[10px] text-textDim">Алгоритм \ Ряд</div>
            )}
            renderRowHeader={(row) => {
                const a = row.meta as any;
                if (!a) return null;

                const titleLines: string[] = [];
                titleLines.push(`Алгоритм: ${a.name ?? row.id}`);
                titleLines.push(`id: ${row.id}`);
                if (a?.m != null) titleLines.push(`m: ${String(a.m)}`);

                const args = a?.algorithmArgs ?? a?.args;
                if (args && typeof args === "object") {
                    titleLines.push("args:");
                    for (const k of Object.keys(args).sort()) {
                        const v = args[k];
                        if (v == null) continue;
                        titleLines.push(`  ${k}: ${String(v)}`);
                    }
                }
                if (a?.argsSummary) titleLines.push(`summary: ${String(a.argsSummary)}`);

                return (
                    <div
                        className="px-1 py-[2px] text-left align-top"
                        title={titleLines.join("\n")}
                    >
                        <div className="leading-tight">
                            <div className="max-w-[200px] whitespace-normal break-words text-[10px] text-textDim">
                                {a.name ?? row.id}
                            </div>

                            <div className="text-[9px] text-textDim/70 whitespace-nowrap">
                                {a?.m != null ? `m=${String(a.m)}` : "m=∅"}
                            </div>

                            <div className="text-[9px] text-textDim/60 break-all">{row.id}</div>

                            {a?.argsSummary && (
                                <div className="mt-[1px] max-w-[200px] whitespace-normal break-words text-[8px] text-textDim/60">
                                    {String(a.argsSummary)}
                                </div>
                            )}
                        </div>
                    </div>
                );
            }}
            renderColHeader={(col) => {
                const s = col.meta as any;
                if (!s) return null;

                const name = col.meta?.name ?? col.id;
                const x = s?.xLabel ?? s?.x ?? "∅";
                const prec = s?.precision ?? "∅";

                return (
                    <div
                        className="flex flex-col items-center justify-end gap-1 px-1 py-1"
                        title={`${name}\n x = ${String(x)}\n prec = ${String(prec)}\n id = ${col.id}`}
                    >
                        <span
                            className="text-[9px] leading-tight text-center whitespace-nowrap"
                            style={{
                                writingMode: "vertical-rl",
                                textOrientation: "mixed",
                                transform: "rotate(180deg)",
                            }}
                        >
                            {name}
                        </span>

                        <span className="text-[8px] leading-tight text-textDim/70 whitespace-nowrap">
                            x={String(x)}
                        </span>

                        <span className="text-[8px] leading-tight text-textDim/60 whitespace-nowrap">
                            {String(prec)}
                        </span>
                    </div>
                );
            }}
            renderCell={(row, col) => {
                const stats = statsIndex[col.id]?.[row.id] ?? null;
                const active = selected?.accelId === row.id && selected?.seriesId === col.id;
                const heatClass = classifyByMax(stats, globalMax);

                const a = row.meta as any;
                const s = col.meta as any;

                const algoName = a?.name ?? row.id;
                const seriesName = s?.name ?? col.id;
                const x = s?.xLabel ?? s?.x ?? "∅";
                const prec = s?.precision ?? "∅";

                const titleLines: string[] = [];
                titleLines.push(`Ряд: ${seriesName} (x=${String(x)}, prec=${String(prec)})`);
                titleLines.push(`Алгоритм: ${algoName}${a?.m != null ? `, m=${String(a.m)}` : ""}`);

                const args = a?.algorithmArgs ?? a?.args;
                if (args && typeof args === "object") {
                    const keys = Object.keys(args)
                        .filter((k) => args[k] != null)
                        .sort();
                    if (keys.length > 0) {
                        titleLines.push("args:");
                        for (const k of keys) titleLines.push(`  ${k}: ${String(args[k])}`);
                    }
                }
                if (a?.argsSummary) titleLines.push(`summary: ${String(a.argsSummary)}`);

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
