// widgets/ErrorMatrixTable.tsx

import React, { useEffect, useMemo, useState } from "react";
import * as XLSX from "xlsx-js-style";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { buildErrorMatrixFromExperiment } from "@/shared/lib/error-matrix/buildErrorMatrix";
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";
import type { MatrixAxisItem, MatrixProps } from "@/shared/ui/Matrix/Matrix";
import { ExperimentMatrixFilterScope } from "@/shared/ui/Matrix/filters/ExperimentMatrixFilterScope";
import {
    appendSheet,
    buildKeyValueSheet,
    buildSheetFromAoa,
    createWorkbook,
} from "@/shared/lib/xlsxExport";

/** Оформление ячейки по количеству ошибок */
function getCellClasses(count: number): string {
    if (count <= 0) return "bg-surface/40 text-textDim/40 border-border/60";
    if (count === 1) return "bg-primary/20 border-primary/60 text-textDim";
    if (count <= 4) return "bg-primary/40 border-primary/80 text-textDim";
    return "bg-secondary/50 border-secondary text-textDim";
}

export interface ErrorMatrixTableProps {
    experiment: Experiment | null;
    /** размер страницы по шагам n (колонкам); если не задан, показываем все */
    maxSteps?: number;
    className?: string;
}

type SortKey = "total" | "ok" | "err" | "okPct" | "errPct";
type SortDir = "asc" | "desc";

interface SortState {
    key: SortKey;
    dir: SortDir;
}

type RowMeta = {
    key: string;
    algorithmName: string;
    m: unknown; // number | bigint | string | null
    argsSummary: string;
    algorithmArgs: Record<string, unknown> | null;
};

type ColMeta = {
    n: number;
};

const ErrorMatrixTableView: React.FC<ErrorMatrixTableProps & { externalResetKey?: string }> = ({
    experiment,
    maxSteps,
    className,
    externalResetKey,
}) => {
    // варианты точности из series.precision
    const precisionOptions = useMemo(() => {
        const set = new Set<string>();
        for (const s of experiment?.seriesList ?? []) {
            if (s.precision) set.add(s.precision);
        }
        return Array.from(set).sort((a, b) => a.localeCompare(b));
    }, [experiment]);

    const [precision, setPrecision] = useState<string | null>(null);

    // сбрасываем precision, если такого значения больше нет
    useEffect(() => {
        if (precision && !precisionOptions.includes(precision)) setPrecision(null);
    }, [precision, precisionOptions]);

    const { nList, algoList, cellMap, cellMessagesMap, algoStats, totalErrorItems } = useMemo(
        () => buildErrorMatrixFromExperiment(experiment, precision),
        [experiment, precision]
    );

    const totalElements = useMemo(
        () => Object.values(algoStats).reduce((sum, st) => sum + st.total, 0),
        [algoStats]
    );

    const [sort, setSort] = useState<SortState>({ key: "err", dir: "desc" });

    const sortedAlgoList = useMemo(() => {
        return [...algoList].sort((a, b) => {
            const sa = algoStats[a.key] ?? { total: 0, success: 0, error: 0 };
            const sb = algoStats[b.key] ?? { total: 0, success: 0, error: 0 };

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

    // Matrix axes
    const rowsAxis: MatrixAxisItem<RowMeta>[] = useMemo(
        () =>
            sortedAlgoList.map((a) => ({
                id: a.key,
                meta: {
                    key: a.key,
                    algorithmName: a.algorithmName,
                    m: a.m,
                    argsSummary: a.argsSummary,
                    algorithmArgs: (a.algorithmArgs ?? null) as Record<string, unknown> | null,
                },
            })),
        [sortedAlgoList]
    );

    const colsAxis: MatrixAxisItem<ColMeta>[] = useMemo(
        () => nList.map((n) => ({ id: String(n), meta: { n } })),
        [nList]
    );

    // no data
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

    const toggleSort = (key: SortKey) => {
        setSort((s) =>
            s.key === key ? { key, dir: s.dir === "asc" ? "desc" : "asc" } : { key, dir: "desc" }
        );
    };

    const renderRowHeader: MatrixProps<RowMeta, ColMeta>["renderRowHeader"] = (row) => {
        const algo = row.meta!;
        const st = algoStats[algo.key] ?? { total: 0, success: 0, error: 0 };
        const total = st.total;
        const ok = st.success;
        const err = st.error;
        const okPct = total > 0 ? (ok / total) * 100 : null;
        const errPct = total > 0 ? (err / total) * 100 : null;

        const titleLines: string[] = [];
        titleLines.push(`Алгоритм: ${algo.algorithmName}`);
        titleLines.push(`m = ${algo.m != null ? String(algo.m) : "∅"}`);
        if (algo.algorithmArgs && Object.keys(algo.algorithmArgs).length > 0) {
            titleLines.push("Аргументы:");
            for (const [k, v] of Object.entries(algo.algorithmArgs).sort(([a], [b]) =>
                a.localeCompare(b)
            )) {
                titleLines.push(`  ${k}: ${String(v)}`);
            }
        }
        titleLines.push("");
        titleLines.push(`Всего: ${total}, OK: ${ok}, Err: ${err}`);
        if (okPct != null && errPct != null) {
            titleLines.push(`% OK ≈ ${okPct.toFixed(1)}%, % Err ≈ ${errPct.toFixed(1)}%`);
        }
        if (precision) titleLines.push(`Фильтр precision = ${precision}`);

        return (
            <div title={titleLines.join("\n")} className="leading-tight">
                <div className="max-w-[210px] truncate text-textDim">{algo.algorithmName}</div>
                <div className="text-[9px] text-textDim/70">
                    {algo.m != null ? `m=${String(algo.m)}` : "m=∅"}
                </div>
                {algo.argsSummary ? (
                    <div className="mt-[1px] max-w-[210px] truncate text-[8px] text-textDim/60">
                        {algo.argsSummary}
                    </div>
                ) : null}

                {/* агрегаты (вместо 5 sticky колонок) */}
                <div className="mt-1 grid grid-cols-5 gap-1 text-[9px] text-textDim/80">
                    <div
                        className="rounded border border-border bg-surface/40 px-1 py-[1px] text-center cursor-pointer select-none"
                        onClick={() => toggleSort("total")}
                        title="Сортировка: Всего"
                    >
                        {total}
                    </div>
                    <div
                        className="rounded border border-border bg-emerald-900/25 px-1 py-[1px] text-center text-emerald-200 cursor-pointer select-none"
                        onClick={() => toggleSort("ok")}
                        title="Сортировка: OK"
                    >
                        {ok}
                    </div>
                    <div
                        className="rounded border border-border bg-red-900/25 px-1 py-[1px] text-center text-red-200 cursor-pointer select-none"
                        onClick={() => toggleSort("err")}
                        title="Сортировка: Err"
                    >
                        {err}
                    </div>
                    <div
                        className="rounded border border-border bg-surface/40 px-1 py-[1px] text-center cursor-pointer select-none"
                        onClick={() => toggleSort("okPct")}
                        title="Сортировка: % OK"
                    >
                        {okPct != null ? `${okPct.toFixed(1)}%` : "—"}
                    </div>
                    <div
                        className="rounded border border-border bg-surface/40 px-1 py-[1px] text-center cursor-pointer select-none"
                        onClick={() => toggleSort("errPct")}
                        title="Сортировка: % Err"
                    >
                        {errPct != null ? `${errPct.toFixed(1)}%` : "—"}
                    </div>
                </div>
            </div>
        );
    };

    const renderColHeader: MatrixProps<RowMeta, ColMeta>["renderColHeader"] = (col) => {
        const n = col.meta!.n;
        return (
            <div className="w-[40px] text-center text-[10px]" title={`Шаг n = ${n}`}>
                n={n}
            </div>
        );
    };

    const renderCell: MatrixProps<RowMeta, ColMeta>["renderCell"] = (row, col) => {
        const algo = row.meta!;
        const n = col.meta!.n;
        const cellKey = `${algo.key}||${n}`;
        const count = cellMap.get(cellKey) ?? 0;
        const msgs = cellMessagesMap.get(cellKey) ?? [];

        const tooltipLines: string[] = [];
        tooltipLines.push(`Алгоритм: ${algo.algorithmName}`);
        tooltipLines.push(`шаг n = ${n}`);
        tooltipLines.push(count > 0 ? `количество ошибок: ${count}` : "ошибок нет");

        if (msgs.length > 0) {
            tooltipLines.push("");
            tooltipLines.push("Уникальные ошибки:");
            for (const msg of msgs) tooltipLines.push(`  • ${msg}`);
        }

        return (
            <div
                title={tooltipLines.join("\n")}
                className={`w-full h-full border px-[2px] py-[2px] text-center text-[10px] cursor-default ${getCellClasses(count)}`}
            >
                {count > 0 ? count : "—"}
            </div>
        );
    };

    const buildWorkbook = ({
        rows: _rows,
        cols: _cols,
    }: {
        rows: MatrixAxisItem<RowMeta>[];
        cols: MatrixAxisItem<ColMeta>[];
        pager: { startIndex: number; endIndex: number };
    }): XLSX.WorkBook => {
            const workbook = createWorkbook("Error matrix by steps", "Error matrix export");

            appendSheet(
                workbook,
                buildKeyValueSheet([
                    { key: "precision filter", value: precision ?? "all" },
                    { key: "algorithms", value: rowsAxis.length },
                    { key: "steps", value: colsAxis.length },
                    { key: "error items", value: totalErrorItems },
                    { key: "total elements", value: totalElements },
                    { key: "sort", value: `${sort.key} (${sort.dir})` },
                ]),
                "overview"
            );

            const header: (string | number | boolean | null)[] = [
                "algorithm",
                "total",
                "ok",
                "err",
                "%ok",
                "%err",
            ];
            for (const c of colsAxis) header.push(`n=${c.meta?.n}`);

            const data: (string | number | boolean | null)[][] = [header];

            for (const r of rowsAxis) {
                const algo = r.meta!;
                const st = algoStats[algo.key] ?? { total: 0, success: 0, error: 0 };
                const total = st.total;
                const ok = st.success;
                const err = st.error;
                const okPct = total > 0 ? (ok / total) * 100 : null;
                const errPct = total > 0 ? (err / total) * 100 : null;

                const line: (string | number | null)[] = [
                    algo.algorithmName,
                    total,
                    ok,
                    err,
                    okPct != null ? Number(okPct.toFixed(1)) : null,
                    errPct != null ? Number(errPct.toFixed(1)) : null,
                ];

                for (const c of colsAxis) {
                    const n = c.meta!.n;
                    const cellKey = `${algo.key}||${n}`;
                    const count = cellMap.get(cellKey) ?? 0;
                    line.push(count > 0 ? count : null);
                }

                data.push(line);
            }

            appendSheet(
                workbook,
                buildSheetFromAoa(data, {
                    cols: [
                        { wch: 28 },
                        { wch: 10 },
                        { wch: 10 },
                        { wch: 10 },
                        { wch: 10 },
                        { wch: 10 },
                        ...colsAxis.map(() => ({ wch: 8 })),
                    ],
                    headerRows: 1,
                    rowHeaderCols: 1,
                    decorateCell: ({ rowIndex, colIndex, cell }) => {
                        if (rowIndex === 0) return;
                        if ([1, 2, 3].includes(colIndex)) cell.z = "0";
                        if ([4, 5].includes(colIndex)) cell.z = "0.0";
                        if (colIndex >= 6) cell.z = "0";
                    },
                }),
                "error_matrix"
            );
            return workbook;
        };

    const totalAlgos = sortedAlgoList.length;
    const totalSteps = nList.length;

    return (
        <MatrixPaged<RowMeta, ColMeta>
            className={className}
            resetKey={`${experiment.id}::${precision ?? "ALL"}::${sort.key}:${sort.dir}::${externalResetKey ?? ""}`}
            rows={rowsAxis}
            cols={colsAxis}
            maxColsPerPage={maxSteps && maxSteps > 0 ? maxSteps : 0}
            // поведение таблицы
            enableInnerScroll
            maxBodyHeight="70vh"
            stickyHeaders
            rowWidth={260} // шире, потому что в row header теперь агрегаты
            colWidth={40}
            tableClassName="border-separate border-spacing-0"
            thClassName="bg-surface"
            tdClassName="p-0"
            // заголовки
            renderTitle={() => "Матрица ошибок: алгоритмы × шаги n"}
            renderSubtitle={() => (precision ? `precision: ${precision}` : "precision: все")}
            renderHeaderRight={({ page, totalPages, startIndex, endIndex }) => (
                <div className="flex items-center gap-3 text-[10px] text-textDim">
                    <div className="whitespace-nowrap">
                        Алгоритмы: {totalAlgos} · Шаги n:{" "}
                        {Math.min(endIndex, totalSteps) - startIndex} из {totalSteps} · Ошибочных
                        элементов: {totalErrorItems} · Всего элементов: {totalElements}
                    </div>

                    {precisionOptions.length > 0 ? (
                        <div className="flex items-center gap-1">
                            <span className="text-textDim/70">precision:</span>
                            <select
                                className="rounded border border-border bg-surface px-1 py-[1px]"
                                value={precision ?? ""}
                                onChange={(e) => setPrecision(e.target.value || null)}
                            >
                                <option value="">Все</option>
                                {precisionOptions.map((p) => (
                                    <option key={p} value={p}>
                                        {p}
                                    </option>
                                ))}
                            </select>
                        </div>
                    ) : null}

                    {totalPages > 1 ? (
                        <div className="text-textDim/60 whitespace-nowrap">
                            стр. {page + 1}/{totalPages} · шаги {startIndex + 1}–{endIndex}
                        </div>
                    ) : null}
                </div>
            )}
            // экспорт
            export={{
                fileBaseName: "error-matrix-steps",
                enablePng: true,
                enableXlsx: true,
                buildWorkbook,
            }}
            // рендер матрицы
            renderCorner={() => <span className="text-left">Алгоритм</span>}
            renderRowHeader={renderRowHeader}
            renderColHeader={renderColHeader}
            renderCell={renderCell}
            emptyFallback={
                <div className="rounded-xl2 border border-border bg-panel p-3 text-[11px] text-textDim/70">
                    Нет данных
                </div>
            }
        />
    );
};

export const ErrorMatrixTable: React.FC<ErrorMatrixTableProps> = ({
    experiment,
    maxSteps,
    className,
}) => {
    return (
        <ExperimentMatrixFilterScope experiment={experiment} resetKey={experiment?.id ?? "no-exp"}>
            {({ experimentFiltered, filterStateKey }) => (
                <ErrorMatrixTableView
                    experiment={experimentFiltered}
                    maxSteps={maxSteps}
                    className={className}
                    externalResetKey={filterStateKey}
                />
            )}
        </ExperimentMatrixFilterScope>
    );
};
