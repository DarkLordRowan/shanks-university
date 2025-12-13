// src/shared/ui/Matrix/Matrix.tsx

import React, { useRef } from "react";
import { useWheelScrollCapture } from "@/shared/lib/dom/useWheelScrollCapture";

export interface MatrixAxisItem<TMeta = unknown> {
    id: string;
    meta?: TMeta;
}

export interface MatrixProps<TRowMeta = unknown, TColMeta = unknown> {
    rows: MatrixAxisItem<TRowMeta>[];
    cols: MatrixAxisItem<TColMeta>[];

    /** Левый верхний угол */
    renderCorner?: () => React.ReactNode;

    /** Заголовок строки (левый столбец) */
    renderRowHeader: (row: MatrixAxisItem<TRowMeta>, rowIndex: number) => React.ReactNode;

    /** Заголовок колонки (верхняя строка) */
    renderColHeader: (col: MatrixAxisItem<TColMeta>, colIndex: number) => React.ReactNode;

    /** Ячейка значения */
    renderCell: (
        row: MatrixAxisItem<TRowMeta>,
        col: MatrixAxisItem<TColMeta>,
        rowIndex: number,
        colIndex: number
    ) => React.ReactNode;

    /** Ширина левого столбца (ось rows) */
    rowWidth?: number | string;

    /** Ширина каждой колонки (ось cols) */
    colWidth?: number | string;

    className?: string;
    tableClassName?: string;
    thClassName?: string;
    tdClassName?: string;

    emptyFallback?: React.ReactNode;

    /** Включить внутренний скролл таблицы */
    enableInnerScroll?: boolean;

    /** Максимальная высота области скролла */
    maxBodyHeight?: string;

    /** Закрепить верхнюю строку и левый столбец */
    stickyHeaders?: boolean;
}

export function Matrix<TRowMeta = unknown, TColMeta = unknown>(
    props: MatrixProps<TRowMeta, TColMeta>
) {
    const {
        rows,
        cols,
        renderCorner,
        renderRowHeader,
        renderColHeader,
        renderCell,
        rowWidth,
        colWidth,
        className,
        tableClassName,
        thClassName,
        tdClassName,
        emptyFallback = null,
        enableInnerScroll = true,
        maxBodyHeight = "70vh",
        stickyHeaders = true,
    } = props;

    const scrollRef = useRef<HTMLDivElement | null>(null);
    useWheelScrollCapture(scrollRef, enableInnerScroll);

    if (rows.length === 0 || cols.length === 0) {
        return <>{emptyFallback}</>;
    }

    const thBase = "px-2 py-1 align-middle";
    const tdBase = "px-2 py-1 align-middle";

    const stickyTop = stickyHeaders ? "sticky top-0 z-20 bg-surface" : "";
    const stickyLeft = stickyHeaders ? "sticky left-0 z-10 bg-surface" : "";
    const stickyCorner = stickyHeaders ? "sticky top-0 left-0 z-30 bg-surface" : "";

    const rowStyle =
        rowWidth != null ? { width: rowWidth, minWidth: rowWidth, maxWidth: rowWidth } : undefined;

    const colStyle =
        colWidth != null ? { width: colWidth, minWidth: colWidth, maxWidth: colWidth } : undefined;

    return (
        <div className={className ?? ""}>
            <div
                ref={scrollRef}
                className={enableInnerScroll ? "overflow-auto" : undefined}
                style={enableInnerScroll ? { maxHeight: maxBodyHeight } : undefined}
            >
                <table
                    className={`border-separate border-spacing-0 w-full ${tableClassName ?? ""}`}
                >
                    <thead>
                        <tr>
                            <th
                                className={`${thBase} text-left ${stickyCorner} ${thClassName ?? ""}`}
                                style={rowStyle}
                            >
                                {renderCorner ? renderCorner() : null}
                            </th>

                            {cols.map((col, j) => (
                                <th
                                    key={col.id}
                                    className={`${thBase} text-center ${stickyTop} ${thClassName ?? ""}`}
                                    style={colStyle}
                                >
                                    {renderColHeader(col, j)}
                                </th>
                            ))}
                        </tr>
                    </thead>

                    <tbody>
                        {rows.map((row, i) => (
                            <tr key={row.id}>
                                <th
                                    className={`${thBase} text-left ${stickyLeft} ${thClassName ?? ""}`}
                                    style={rowStyle}
                                >
                                    {renderRowHeader(row, i)}
                                </th>

                                {cols.map((col, j) => (
                                    <td
                                        key={col.id}
                                        className={`${tdBase} text-center ${tdClassName ?? ""}`}
                                        style={colStyle}
                                    >
                                        {renderCell(row, col, i, j)}
                                    </td>
                                ))}
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
        </div>
    );
}
