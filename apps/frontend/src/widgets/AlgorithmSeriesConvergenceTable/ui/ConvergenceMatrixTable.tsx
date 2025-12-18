import React, { useCallback, useEffect, useMemo, useState } from "react";
import * as XLSX from "xlsx-js-style";
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
import type { MatrixAxisItem } from "@/shared/ui/Matrix/Matrix";
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";

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

type ColorClass = "green" | "blue" | "yellow" | "red" | "neutral";

function isMonotone(mon: MonotonicityType): boolean {
    return (
        mon === "strict_decreasing_error" ||
        mon === "non_increasing_error" ||
        mon === "constant_error"
    );
}

function describeClass(side: SideType, mon: MonotonicityType): string {
    const mono = isMonotone(mon);

    if (side === "one_sided" && mono) return "односторонний и монотонный";
    if (side === "one_sided" && !mono) return "односторонний и немонотонный";
    if (side === "two_sided" && mono) return "двусторонний и монотонный";
    if (side === "two_sided" && !mono) return "двусторонний и немонотонный";

    if (side === "no_limit") return "предел не просматривается";
    if (mon === "not_enough_data") return "недостаточно данных по ошибке";

    return "тип не определён";
}

function formatIntervals(ns: number[], maxRanges = 5): string {
    if (!ns.length) return "—";

    const sorted = Array.from(new Set(ns)).sort((a, b) => a - b);

    const ranges: Array<{ start: number; end: number }> = [];
    let start = sorted[0];
    let prev = sorted[0];

    for (let i = 1; i < sorted.length; i++) {
        const x = sorted[i];
        if (x === prev + 1) {
            prev = x;
            continue;
        }
        ranges.push({ start, end: prev });
        start = x;
        prev = x;
    }
    ranges.push({ start, end: prev });

    const parts = ranges
        .slice(0, maxRanges)
        .map((r) => (r.start === r.end ? `${r.start}` : `${r.start}–${r.end}`));

    if (ranges.length > maxRanges) {
        parts.push("…");
    }

    return parts.join(", ");
}

function getCellColorClass(side: SideType, mon: MonotonicityType, selected: boolean): string {
    const sel = selected ? " ring-2 ring-accent ring-offset-1 ring-offset-surface" : "";

    if (side === "no_limit" || mon === "not_enough_data" || mon === "no_limit") {
        return "border-border/60 text-textDim/70 bg-surface/30 hover:bg-surface/40" + sel;
    }

    const mono = isMonotone(mon);

    if (side === "one_sided" && mono) {
        return "border-border text-textDim bg-emerald-500/25 hover:bg-emerald-500/35" + sel;
    }
    if (side === "one_sided" && !mono) {
        return "border-border text-textDim bg-sky-500/25 hover:bg-sky-500/35" + sel;
    }

    if (side === "two_sided" && mono) {
        return "border-border text-textDim bg-amber-300/35 hover:bg-amber-300/45" + sel;
    }
    if (side === "two_sided" && !mono) {
        return "border-border text-textDim bg-red-500/30 hover:bg-red-500/40" + sel;
    }

    return "border-border text-textDim bg-surface/40 hover:bg-surface/50" + sel;
}

function classifyColor(side: SideType, mon: MonotonicityType): ColorClass {
    if (side === "no_limit" || mon === "not_enough_data" || mon === "no_limit") {
        return "neutral";
    }

    const mono = isMonotone(mon);

    if (side === "one_sided" && mono) return "green";
    if (side === "one_sided" && !mono) return "blue";
    if (side === "two_sided" && mono) return "yellow";
    if (side === "two_sided" && !mono) return "red";

    return "neutral";
}

export const ConvergenceMatrixTable: React.FC<ConvergenceMatrixTableProps> = ({
    matrix,
    maxSeries,
    selectedCell,
    onCellSelect,
}) => {
    const rawSeriesList = matrix.seriesList ?? [];
    const algoList = matrix.algoList ?? [];
    const cells = matrix.cells ?? {};

    const thresholds = useMemo(() => {
        let maxSignChanges = 0;
        let maxViolations = 0;

        for (const key of Object.keys(cells)) {
            const analysis = cells[key];
            if (!analysis) continue;

            const sc =
                typeof analysis.signChangesCount === "number" ? analysis.signChangesCount : 0;
            if (sc > maxSignChanges) maxSignChanges = sc;

            const violationsRaw =
                typeof analysis.growthViolationsCount === "number"
                    ? analysis.growthViolationsCount
                    : 0;
            if (violationsRaw > maxViolations) maxViolations = violationsRaw;
        }

        return { maxSignChanges, maxViolations };
    }, [cells]);

    const [maxSignChangesForOneSided, setMaxSignChangesForOneSided] = useState<number>(0);
    const [maxViolationsForMonotone, setMaxViolationsForMonotone] = useState<number>(0);

    useEffect(() => {
        setMaxSignChangesForOneSided(0);
        setMaxViolationsForMonotone(0);
    }, [matrix]);

    const allPrecisions = useMemo(() => {
        const set = new Set<string>();
        for (const s of rawSeriesList) {
            if (s.precision != null) set.add(String(s.precision));
        }
        return Array.from(set).sort();
    }, [rawSeriesList]);

    const [precisionFilter, setPrecisionFilter] = useState<"ALL" | string>("ALL");

    const seriesList = useMemo(() => {
        if (precisionFilter === "ALL") return rawSeriesList;
        return rawSeriesList.filter((s) => String(s.precision) === precisionFilter);
    }, [rawSeriesList, precisionFilter]);

    const signChangesSliderMax = thresholds.maxSignChanges > 0 ? thresholds.maxSignChanges : 5;
    const violationsSliderMax = thresholds.maxViolations > 0 ? thresholds.maxViolations : 5;

    const rows: MatrixAxisItem<{ algoIndex: number }>[] = useMemo(
        () => algoList.map((a, i) => ({ id: a.key, meta: { algoIndex: i } })),
        [algoList]
    );

    const allCols: MatrixAxisItem<{ seriesIndex: number }>[] = useMemo(
        () => seriesList.map((s, i) => ({ id: s.key, meta: { seriesIndex: i } })),
        [seriesList]
    );

    const buildWorkbook = useCallback((): XLSX.WorkBook => {
        const allSeries = seriesList;
        const allAlgos = algoList;

        const wb = XLSX.utils.book_new();

        if (!allSeries.length || !allAlgos.length) {
            const wsEmpty = XLSX.utils.aoa_to_sheet([["Нет данных для экспорта"]]);
            XLSX.utils.book_append_sheet(wb, wsEmpty, "convergence");
            return wb;
        }

        // ---------- заголовок основной матрицы ----------
        const headerRow: (string | number)[] = ["Алгоритм \\ Ряд"];
        for (const s of allSeries) {
            headerRow.push(`${s.seriesName} (x=${s.xLabel}, prec=${s.precision ?? "∅"})`);
        }

        const wsData: (string | number | null)[][] = [];
        wsData.push(headerRow);

        // сетка цветов для последующего проставления стилей
        const colorGrid: ColorClass[][] = [];
        colorGrid.push(new Array(headerRow.length).fill("neutral"));

        // статистика по строкам и столбцам
        const rowStats: Array<{ green: number; blue: number; yellow: number; red: number }> =
            allAlgos.map(() => ({ green: 0, blue: 0, yellow: 0, red: 0 }));

        const colStats: Array<{ green: number; blue: number; yellow: number; red: number }> =
            allSeries.map(() => ({ green: 0, blue: 0, yellow: 0, red: 0 }));

        // ---------- строки по алгоритмам ----------
        allAlgos.forEach((algo, algoIdx) => {
            const row: (string | number | null)[] = [];

            const algoParts: string[] = [];
            algoParts.push(algo.algorithmName);
            if (algo.m != null) algoParts.push(`m=${algo.m}`);

            const algoArgsEntries = nonNullEntries(algo.algorithmArgs);
            if (algoArgsEntries.length > 0) {
                const argsStr = algoArgsEntries
                    .sort(([a], [b]) => a.localeCompare(b))
                    .map(([k, v]) => `${k}=${v}`)
                    .join(", ");
                algoParts.push(`{${argsStr}}`);
            }

            row.push(algoParts.join(" "));

            const colorRow: ColorClass[] = [];
            colorRow.push("neutral"); // первая колонка

            allSeries.forEach((s, seriesIdx) => {
                const key = `${algo.key}::${s.key}`;
                const analysis = cells[key];

                if (!analysis) {
                    row.push(null);
                    colorRow.push("neutral");
                    return;
                }

                const rawSide = analysis.side;
                const rawMon = analysis.monotonicity;

                const signChanges =
                    typeof analysis.signChangesCount === "number" ? analysis.signChangesCount : 0;

                const violationsCount =
                    typeof analysis.growthViolationsCount === "number"
                        ? analysis.growthViolationsCount
                        : 0;

                let effectiveSide: SideType = rawSide;
                if (rawSide !== "no_limit") {
                    effectiveSide =
                        signChanges <= maxSignChangesForOneSided ? "one_sided" : "two_sided";
                }

                let effectiveMon: MonotonicityType = rawMon;
                if (rawMon !== "no_limit" && rawMon !== "not_enough_data") {
                    if (violationsCount <= maxViolationsForMonotone) {
                        if (rawMon === "has_growth") effectiveMon = "non_increasing_error";
                    }
                }

                const sideShort = formatSideShort(effectiveSide);
                const monShort = formatMonotonicityShort(effectiveMon);

                const cellText = [
                    `${sideShort} | ${monShort}`,
                    `k=${analysis.stepsAnalyzed}`,
                    `sign_changes=${analysis.signChangesCount ?? "∅"}`,
                    `growth_violations=${analysis.growthViolationsCount ?? "∅"}`,
                ].join(" / ");

                row.push(cellText);

                const colorClass = classifyColor(effectiveSide, effectiveMon);
                colorRow.push(colorClass);

                if (colorClass !== "neutral") {
                    rowStats[algoIdx][colorClass]++;
                    colStats[seriesIdx][colorClass]++;
                }
            });

            wsData.push(row);
            colorGrid.push(colorRow);
        });

        // ---------- основная таблица ----------
        const ws = XLSX.utils.aoa_to_sheet(wsData);

        const colorStyles: Record<ColorClass, any> = {
            neutral: {},
            green: { fill: { patternType: "solid", fgColor: { rgb: "C6EFCE" } } },
            blue: { fill: { patternType: "solid", fgColor: { rgb: "C6D9F1" } } },
            yellow: { fill: { patternType: "solid", fgColor: { rgb: "FFF2CC" } } },
            red: { fill: { patternType: "solid", fgColor: { rgb: "F8CBAD" } } },
        };

        for (let r = 1; r < colorGrid.length; r++) {
            const rowColors = colorGrid[r];
            for (let c = 1; c < rowColors.length; c++) {
                const colorClass = rowColors[c];
                if (colorClass === "neutral") continue;

                const cellAddress = XLSX.utils.encode_cell({ r, c });
                const cell = ws[cellAddress];
                if (!cell) continue;

                cell.s = {
                    ...(cell.s || {}),
                    ...colorStyles[colorClass],
                };
            }
        }

        // ---------- лист статистики по строкам ----------
        const rowStatsData: (string | number)[][] = [];
        rowStatsData.push([
            "Алгоритм",
            "green (одностор.+монотонн.)",
            "blue (одностор.+немонотонн.)",
            "yellow (двустор.+монотонн.)",
            "red (двустор.+немонотонн.)",
            "total",
        ]);

        allAlgos.forEach((algo, i) => {
            const st = rowStats[i];
            const total = st.green + st.blue + st.yellow + st.red;

            const algoParts: string[] = [];
            algoParts.push(algo.algorithmName);
            if (algo.m != null) algoParts.push(`m=${algo.m}`);

            rowStatsData.push([algoParts.join(" "), st.green, st.blue, st.yellow, st.red, total]);
        });

        const wsRowStats = XLSX.utils.aoa_to_sheet(rowStatsData);

        // ---------- лист статистики по столбцам ----------
        const colStatsData: (string | number)[][] = [];
        colStatsData.push([
            "Ряд",
            "green (одностор.+монотонн.)",
            "blue (одностор.+немонотонн.)",
            "yellow (двустор.+монотонн.)",
            "red (двустор.+немонотонн.)",
            "total",
        ]);

        allSeries.forEach((s, j) => {
            const st = colStats[j];
            const total = st.green + st.blue + st.yellow + st.red;

            const seriesLabel = `${s.seriesName} (x=${s.xLabel}, prec=${s.precision ?? "∅"})`;
            colStatsData.push([seriesLabel, st.green, st.blue, st.yellow, st.red, total]);
        });

        const wsColStats = XLSX.utils.aoa_to_sheet(colStatsData);

        XLSX.utils.book_append_sheet(wb, ws, "convergence");
        XLSX.utils.book_append_sheet(wb, wsRowStats, "row_stats");
        XLSX.utils.book_append_sheet(wb, wsColStats, "col_stats");

        return wb;
    }, [algoList, cells, maxSignChangesForOneSided, maxViolationsForMonotone, seriesList]);

    if (rawSeriesList.length === 0 || algoList.length === 0) {
        return (
            <div className="text-textDim text-sm">
                Нет пар ряд × алгоритм для анализа (seriesList или accelList пусты).
            </div>
        );
    }

    return (
        <MatrixPaged<{ algoIndex: number }, { seriesIndex: number }>
            resetKey={`""::${precisionFilter}`}
            rows={rows}
            cols={allCols}
            maxColsPerPage={maxSeries && maxSeries > 0 ? maxSeries : 0}
            rowWidth={160}
            colWidth={50}
            thClassName="px-0 py-0"
            tdClassName="px-0 py-0"
            maxBodyHeight="70vh"
            emptyFallback={<div className="text-textDim text-sm">Нет данных для отображения.</div>}
            export={{
                fileBaseName: "convergence-matrix",
                enablePng: true,
                enableXlsx: true,
                buildWorkbook: () => buildWorkbook(),
            }}
            renderTitle={() => "Монотонность и направление: алгоритмы × ряды"}
            renderSubtitle={() => (
                <>
                    Алгоритмы: {algoList.length} · Ряды: {seriesList.length}
                    {precisionFilter !== "ALL"
                        ? ` (из ${rawSeriesList.length}, precision=${precisionFilter})`
                        : ""}
                </>
            )}
            renderHeaderRight={() => (
                <div className="flex items-center gap-3">
                    <div className="flex items-center gap-1 text-[10px]">
                        <span>precision:</span>
                        <select
                            className="rounded border border-border bg-surface px-2 py-[2px]"
                            value={precisionFilter}
                            onChange={(e) =>
                                setPrecisionFilter(
                                    e.target.value === "ALL" ? "ALL" : e.target.value
                                )
                            }
                        >
                            <option value="ALL">Все</option>
                            {allPrecisions.map((p) => (
                                <option key={p} value={p}>
                                    {p}
                                </option>
                            ))}
                        </select>
                    </div>

                    <div className="flex flex-col gap-[2px] text-[10px]">
                        <div className="flex items-center gap-1">
                            <span
                                className="whitespace-nowrap"
                                title="Если число смен знака ≤ X, пара считается односторонней"
                            >
                                max sign changes:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={signChangesSliderMax}
                                value={maxSignChangesForOneSided}
                                onChange={(e) =>
                                    setMaxSignChangesForOneSided(Number(e.target.value))
                                }
                                className="h-[4px] w-28 cursor-pointer"
                            />
                            <span className="w-6 text-right tabular-nums">
                                {maxSignChangesForOneSided}
                            </span>
                        </div>

                        <div className="flex items-center gap-1">
                            <span
                                className="whitespace-nowrap"
                                title="Если число расхождений ≤ Y, ошибка считается монотонной"
                            >
                                max deviations:
                            </span>
                            <input
                                type="range"
                                min={0}
                                max={violationsSliderMax}
                                value={maxViolationsForMonotone}
                                onChange={(e) =>
                                    setMaxViolationsForMonotone(Number(e.target.value))
                                }
                                className="h-[4px] w-28 cursor-pointer"
                            />
                            <span className="w-6 text-right tabular-nums">
                                {maxViolationsForMonotone}
                            </span>
                        </div>
                    </div>
                </div>
            )}
            renderCorner={() => (
                <div className="px-1 py-1 text-left text-[10px] text-textDim">Алгоритм \ Ряд</div>
            )}
            renderColHeader={(col) => {
                const idx = col.meta?.seriesIndex ?? 0;
                const s = seriesList[idx];
                if (!s) return null;

                return (
                    <div
                        className="flex flex-col items-center justify-end gap-1 px-1 py-1"
                        title={`${s.seriesName}\n x = ${s.xLabel}\n prec = ${s.precision}`}
                    >
                        <span
                            className="text-[9px] leading-tight text-center whitespace-nowrap"
                            style={{
                                writingMode: "vertical-rl",
                                textOrientation: "mixed",
                                transform: "rotate(180deg)",
                            }}
                        >
                            {s.seriesName}
                        </span>

                        <span className="text-[8px] leading-tight text-textDim/70 whitespace-nowrap">
                            x={s.xLabel}
                        </span>

                        <span className="text-[8px] leading-tight text-textDim/60 whitespace-nowrap">
                            {s.precision}
                        </span>
                    </div>
                );
            }}
            renderRowHeader={(row) => {
                const algo = algoList.find((a) => a.key === row.id);
                if (!algo) return null;

                return (
                    <div
                        className="px-1 py-[2px] text-left align-top"
                        title={(() => {
                            const lines: string[] = [];
                            lines.push(`Алгоритм: ${algo.algorithmName}`);
                            lines.push(`m = ${algo.m != null ? String(algo.m) : "∅"}`);
                            const entries = nonNullEntries(algo.algorithmArgs);
                            if (entries.length > 0) {
                                lines.push("Аргументы:");
                                for (const [k, v] of entries.sort(([a, b]) => a.localeCompare(b))) {
                                    lines.push(`  ${k}: ${v}`);
                                }
                            }
                            return lines.join("\n");
                        })()}
                    >
                        <div className="leading-tight">
                            <span className="block max-w-[150px] whitespace-normal break-words text-[10px] text-textDim">
                                {algo.algorithmName}
                            </span>

                            <span className="block text-[9px] text-textDim/70 whitespace-nowrap">
                                {algo.m != null ? `m=${String(algo.m)}` : "m=∅"}
                            </span>

                            {algo.argsSummary && (
                                <div className="mt-[1px] max-w-[150px] whitespace-normal break-words text-[8px] text-textDim/60">
                                    {algo.argsSummary}
                                </div>
                            )}
                        </div>
                    </div>
                );
            }}
            renderCell={(row, col, _i, _j) => {
                const algo = algoList.find((a) => a.key === row.id);
                const s = seriesList[col.meta?.seriesIndex ?? 0];
                if (!algo || !s) return null;

                const key = `${algo.key}::${s.key}`;
                const analysis = cells[key];

                if (!analysis) {
                    return (
                        <div className="w-full h-full min-h-[32px] flex items-center justify-center text-[10px] text-textDim/50">
                            —
                        </div>
                    );
                }

                const rawSide = analysis.side;
                const rawMon = analysis.monotonicity;

                const signChanges =
                    typeof analysis.signChangesCount === "number" ? analysis.signChangesCount : 0;

                const violationsCount =
                    typeof analysis.growthViolationsCount === "number"
                        ? analysis.growthViolationsCount
                        : 0;

                let effectiveSide: SideType = rawSide;
                if (rawSide !== "no_limit") {
                    effectiveSide =
                        signChanges <= maxSignChangesForOneSided ? "one_sided" : "two_sided";
                }

                let effectiveMon: MonotonicityType = rawMon;
                if (rawMon !== "no_limit" && rawMon !== "not_enough_data") {
                    if (violationsCount <= maxViolationsForMonotone) {
                        if (rawMon === "has_growth") effectiveMon = "non_increasing_error";
                    }
                }

                const sideShort = formatSideShort(effectiveSide);
                const monShort = formatMonotonicityShort(effectiveMon);

                const titleLines: string[] = [];
                titleLines.push(`Ряд: ${s.seriesName} (x=${s.xLabel}, prec=${s.precision})`);
                titleLines.push(
                    `Алгоритм: ${algo.algorithmName}` + (algo.m != null ? `, m=${algo.m}` : "")
                );
                titleLines.push("Аргументы алгоритма:");

                const algoEntries = nonNullEntries(algo.algorithmArgs);
                if (algoEntries.length > 0) {
                    for (const [k, v] of algoEntries.sort(([a, b]) => a.localeCompare(b))) {
                        titleLines.push(`  ${k} = ${String(v)}`);
                    }
                }
                if (algo.argsSummary) titleLines.push(`  (${algo.argsSummary})`);
                if (algoEntries.length > 0 || algo.argsSummary) titleLines.push("");

                titleLines.push(`Класс: ${describeClass(effectiveSide, effectiveMon)}`);

                const signNsText =
                    analysis.signChangeNs && analysis.signChangeNs.length > 0
                        ? formatIntervals(analysis.signChangeNs)
                        : analysis.firstSignChangeN != null
                          ? String(analysis.firstSignChangeN)
                          : "—";

                const growthNsText =
                    analysis.growthNs && analysis.growthNs.length > 0
                        ? formatIntervals(analysis.growthNs)
                        : analysis.firstGrowthN != null
                          ? String(analysis.firstGrowthN)
                          : "—";

                titleLines.push(
                    `Число смен знака: ${analysis.signChangesCount}, ns: ${signNsText}`
                );
                titleLines.push(
                    `Число роста |Aₙ−lim|: ${analysis.growthViolationsCount}, ns: ${growthNsText}`
                );
                titleLines.push(`Пар (n−1,n) в анализе: ${analysis.stepsAnalyzed}`);
                titleLines.push("");
                titleLines.push("Клик — детальный график.");

                const isSelected =
                    selectedCell?.seriesId === s.key && selectedCell?.accelId === algo.key;

                const colorClass = getCellColorClass(effectiveSide, effectiveMon, isSelected);
                const domId = getConvergenceCellDomId(algo.key, s.key);

                return (
                    <div
                        id={domId}
                        title={titleLines.join("\n")}
                        className={
                            "w-full h-full min-h-[32px] cursor-pointer border border-transparent " +
                            colorClass
                        }
                        onClick={() => onCellSelect({ seriesId: s.key, accelId: algo.key })}
                    >
                        <div className="flex select-none flex-col items-center justify-center gap-[1px] leading-tight py-[2px]">
                            <span className="font-mono text-[10px]">
                                {sideShort} | {monShort}
                            </span>
                            <span className="text-[9px] text-textDim/80">
                                k: {analysis.stepsAnalyzed}
                            </span>
                        </div>
                    </div>
                );
            }}
        />
    );
};
