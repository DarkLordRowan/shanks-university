import React, { useEffect, useMemo, useState } from "react";
import * as XLSX from "xlsx-js-style";
import { Matrix, type MatrixAxisItem, type MatrixProps } from "./Matrix";
import { MatrixExportWrapper } from "./MatrixExportWrapper";

export type PagerInfo = {
    page: number;
    totalPages: number;
    pageSize: number;
    startIndex: number;
    endIndex: number;
    totalCols: number;
};

function clamp(n: number, lo: number, hi: number) {
    return Math.max(lo, Math.min(hi, n));
}

export interface MatrixPagedProps<TRowMeta = unknown, TColMeta = unknown>
    extends Omit<
        MatrixProps<TRowMeta, TColMeta>,
        "cols" | "rows" | "renderRowHeader" | "renderColHeader" | "renderCell"
    > {
    rows: MatrixAxisItem<TRowMeta>[];
    cols: MatrixAxisItem<TColMeta>[];

    /** если <= 0, пагинация выключена */
    maxColsPerPage?: number;

    /** сброс страницы при смене ключа (например experiment.id) */
    resetKey?: string | number;

    /** верхняя панель слева */
    renderTitle?: () => React.ReactNode;

    /** верхняя панель под заголовком */
    renderSubtitle?: () => React.ReactNode;

    /** правая часть шапки (контролы, фильтры и т.п.) */
    renderHeaderRight?: (pager: PagerInfo) => React.ReactNode;

    /** рендеры матрицы */
    renderRowHeader: MatrixProps<TRowMeta, TColMeta>["renderRowHeader"];
    renderColHeader: MatrixProps<TRowMeta, TColMeta>["renderColHeader"];
    renderCell: MatrixProps<TRowMeta, TColMeta>["renderCell"];

    /** опциональный экспорт */
    export?: {
        fileBaseName: string;
        enablePng?: boolean;
        enableXlsx?: boolean;

        /**
         * Обычно экспорт ожидается "по видимым колонкам",
         * поэтому buildWorkbook получает текущий срез cols.
         * Если нужен экспорт "всего", игнорируйте colsSlice внутри buildWorkbook.
         */
        buildWorkbook: (args: {
            rows: MatrixAxisItem<TRowMeta>[];
            cols: MatrixAxisItem<TColMeta>[];
            pager: PagerInfo;
        }) => XLSX.WorkBook;
    };
}

function Pager({ info, onSetPage }: { info: PagerInfo; onSetPage: (p: number) => void }) {
    const { page, totalPages, startIndex, endIndex, totalCols } = info;
    if (totalPages <= 1) return null;

    return (
        <div className="flex items-center gap-1 text-[10px] text-textDim">
            <button
                type="button"
                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                onClick={() => onSetPage(0)}
                disabled={page === 0}
            >
                «
            </button>
            <button
                type="button"
                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                onClick={() => onSetPage(page - 1)}
                disabled={page === 0}
            >
                ‹
            </button>

            <span className="px-1">
                стр. {page + 1} / {totalPages}
            </span>

            <span className="text-textDim/60">
                колонки {startIndex + 1}–{endIndex} из {totalCols}
            </span>

            <button
                type="button"
                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                onClick={() => onSetPage(page + 1)}
                disabled={page >= totalPages - 1}
            >
                ›
            </button>
            <button
                type="button"
                className="rounded border border-border bg-surface px-1 py-[1px] hover:bg-panel disabled:opacity-40"
                onClick={() => onSetPage(totalPages - 1)}
                disabled={page >= totalPages - 1}
            >
                »
            </button>
        </div>
    );
}

export function MatrixPaged<TRowMeta = unknown, TColMeta = unknown>(
    props: MatrixPagedProps<TRowMeta, TColMeta>
) {
    const {
        rows,
        cols: allCols,
        maxColsPerPage = 0,
        resetKey,

        renderTitle,
        renderSubtitle,
        renderHeaderRight,

        renderRowHeader,
        renderColHeader,
        renderCell,

        export: exportCfg,

        ...matrixProps
    } = props;

    const [page, setPage] = useState(0);

    const pageSize = maxColsPerPage && maxColsPerPage > 0 ? maxColsPerPage : allCols.length || 1;

    const totalCols = allCols.length;
    const totalPages = Math.max(1, Math.ceil((totalCols || 1) / pageSize));

    useEffect(() => {
        setPage(0);
    }, [resetKey, maxColsPerPage]);

    useEffect(() => {
        setPage((p) => clamp(p, 0, totalPages - 1));
    }, [totalPages]);

    const startIndex = page * pageSize;
    const endIndex = Math.min(startIndex + pageSize, totalCols);

    const colsSlice = useMemo(() => {
        if (totalCols === 0) return [];
        if (pageSize >= totalCols) return allCols;
        return allCols.slice(startIndex, endIndex);
    }, [allCols, startIndex, endIndex, pageSize, totalCols]);

    const pagerInfo: PagerInfo = useMemo(
        () => ({
            page,
            totalPages,
            pageSize,
            startIndex,
            endIndex,
            totalCols,
        }),
        [page, totalPages, pageSize, startIndex, endIndex, totalCols]
    );

    const header =
        renderTitle || renderSubtitle || renderHeaderRight || totalPages > 1 ? (
            <div
                className="
                    relative mb-2
                    -mx-4 px-4 py-2
                    bg-surface/95 backdrop-blur-sm
                    flex items-center justify-between
                "
            >
                <div className="flex flex-col gap-1">
                    {renderTitle ? (
                        <span className="text-sm font-semibold text-textDim">{renderTitle()}</span>
                    ) : null}
                    {renderSubtitle ? (
                        <span className="text-[11px] text-textDim/80">{renderSubtitle()}</span>
                    ) : null}
                </div>

                <div className="flex items-center gap-3">
                    {renderHeaderRight ? renderHeaderRight(pagerInfo) : null}
                    <Pager
                        info={pagerInfo}
                        onSetPage={(p) => setPage(clamp(p, 0, totalPages - 1))}
                    />
                </div>
            </div>
        ) : null;

    if (!exportCfg) {
        return (
            <>
                {header}
                <Matrix<TRowMeta, TColMeta>
                    {...matrixProps}
                    rows={rows}
                    cols={colsSlice}
                    renderRowHeader={renderRowHeader}
                    renderColHeader={renderColHeader}
                    renderCell={renderCell}
                />
            </>
        );
    }

    return (
        <>
            {header}

            <MatrixExportWrapper
                fileBaseName={exportCfg.fileBaseName}
                enablePng={exportCfg.enablePng}
                enableXlsx={exportCfg.enableXlsx}
                buildWorkbook={() =>
                    exportCfg.buildWorkbook({
                        rows,
                        cols: colsSlice,
                        pager: pagerInfo,
                    })
                }
            >
                {({ noInnerScroll, noSticky, captureRef }) => (
                    <div ref={captureRef}>
                        <Matrix<TRowMeta, TColMeta>
                            {...matrixProps}
                            rows={rows}
                            cols={colsSlice}
                            enableInnerScroll={!noInnerScroll}
                            stickyHeaders={!noSticky}
                            renderRowHeader={renderRowHeader}
                            renderColHeader={renderColHeader}
                            renderCell={renderCell}
                        />
                    </div>
                )}
            </MatrixExportWrapper>
        </>
    );
}
