import React, { useRef } from "react";
import { useWheelScrollCapture } from "@/shared/lib/dom/useWheelScrollCapture";

export interface MatrixAxisItem<TMeta = unknown> {
    id: string;
    meta?: TMeta;
}

export interface MatrixProps<TRowMeta = unknown, TColMeta = unknown> {
    rows: MatrixAxisItem<TRowMeta>[];
    cols: MatrixAxisItem<TColMeta>[];

    renderCorner?: () => React.ReactNode;
    renderRowHeader: (row: MatrixAxisItem<TRowMeta>, rowIndex: number) => React.ReactNode;
    renderColHeader: (col: MatrixAxisItem<TColMeta>, colIndex: number) => React.ReactNode;
    renderCell: (
        row: MatrixAxisItem<TRowMeta>,
        col: MatrixAxisItem<TColMeta>,
        rowIndex: number,
        colIndex: number
    ) => React.ReactNode;

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

    // Перехватываем wheel только для вертикали (deltaY) и только если контейнер может скроллиться.
    useWheelScrollCapture(scrollRef, enableInnerScroll);

    if (rows.length === 0 || cols.length === 0) {
        return <>{emptyFallback}</>;
    }

    const thBase = "border border-border px-2 py-1 align-middle";
    const tdBase = "border border-border px-2 py-1 align-middle";

    const stickyTop = stickyHeaders ? "sticky top-0 z-20 bg-surface" : "";
    const stickyLeft = stickyHeaders ? "sticky left-0 z-10 bg-surface" : "";
    const stickyCorner = stickyHeaders ? "sticky top-0 left-0 z-30 bg-surface" : "";

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
                            >
                                {renderCorner ? renderCorner() : null}
                            </th>

                            {cols.map((col, j) => (
                                <th
                                    key={col.id}
                                    className={`${thBase} text-center ${stickyTop} ${thClassName ?? ""}`}
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
                                >
                                    {renderRowHeader(row, i)}
                                </th>

                                {cols.map((col, j) => (
                                    <td
                                        key={col.id}
                                        className={`${tdBase} text-center ${tdClassName ?? ""}`}
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
