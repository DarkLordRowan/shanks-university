// src/shared/ui/StickyTable.tsx
import React, { useRef, useLayoutEffect } from "react";

interface StickyTableProps {
    header: React.ReactNode;
    body: React.ReactNode;
    stickyOffset?: number;
    className?: string;
}

export const StickyTable: React.FC<StickyTableProps> = ({
    header,
    body,
    stickyOffset = 0,
    className = "",
}) => {
    const headerScrollRef = useRef<HTMLDivElement | null>(null);
    const bodyScrollRef = useRef<HTMLDivElement | null>(null);
    const syncingScrollRef = useRef(false);

    const headerTableRef = useRef<HTMLTableElement | null>(null);
    const bodyTableRef = useRef<HTMLTableElement | null>(null);

    /* ---------- синхронизация горизонтального скролла ---------- */

    const syncScroll = (source: "header" | "body") => {
        if (syncingScrollRef.current) return;
        syncingScrollRef.current = true;

        const headerEl = headerScrollRef.current;
        const bodyEl = bodyScrollRef.current;
        if (!headerEl || !bodyEl) {
            syncingScrollRef.current = false;
            return;
        }

        if (source === "header") {
            if (bodyEl.scrollLeft !== headerEl.scrollLeft) {
                bodyEl.scrollLeft = headerEl.scrollLeft;
            }
        } else {
            if (headerEl.scrollLeft !== bodyEl.scrollLeft) {
                headerEl.scrollLeft = bodyEl.scrollLeft;
            }
        }

        requestAnimationFrame(() => {
            syncingScrollRef.current = false;
        });
    };

    const onHeaderScroll: React.UIEventHandler<HTMLDivElement> = () => {
        syncScroll("header");
    };

    const onBodyScroll: React.UIEventHandler<HTMLDivElement> = () => {
        syncScroll("body");
    };

    /* ---------- синхронизация ширины колонок ---------- */

    const syncColumnWidths = () => {
        const headerTable = headerTableRef.current;
        const bodyTable = bodyTableRef.current;
        if (!headerTable || !bodyTable) return;

        // если таблица скрыта (display: none), смысла мерить нет
        if (!bodyTable.offsetParent) return;

        const thead = headerTable.tHead;
        const tbody = bodyTable.tBodies[0];
        if (!thead || !tbody || tbody.rows.length === 0) return;

        const headerRow = thead.rows.length > 0 ? thead.rows[thead.rows.length - 1] : null;
        const bodyRow = tbody.rows[0];
        if (!headerRow || !bodyRow) return;

        const headerCells = Array.from(headerRow.children) as HTMLTableCellElement[];
        const bodyCells = Array.from(bodyRow.children) as HTMLTableCellElement[];

        const len = Math.min(headerCells.length, bodyCells.length);
        if (len === 0) return;

        // 1) сбрасываем предыдущие размеры во всех ячейках
        for (const cell of headerCells) {
            cell.style.width = "";
            cell.style.minWidth = "";
        }
        for (const cell of bodyCells) {
            cell.style.width = "";
            cell.style.minWidth = "";
        }
        headerTable.style.tableLayout = "auto";
        bodyTable.style.tableLayout = "auto";

        // 2) пересчитываем ширины заново по первой строке тела
        const widths: number[] = [];
        for (let i = 0; i < len; i++) {
            const w = bodyCells[i].getBoundingClientRect().width;
            widths.push(w);
        }

        // 3) фиксируем одинаковую ширину
        headerTable.style.tableLayout = "fixed";
        bodyTable.style.tableLayout = "fixed";

        for (let i = 0; i < len; i++) {
            const w = widths[i];
            if (!w || !Number.isFinite(w)) continue;

            const px = `${w}px`;

            const bodyCell = bodyCells[i];
            const headerCell = headerCells[i];

            headerCell.style.width = px;
            headerCell.style.minWidth = px;

            bodyCell.style.width = px;
            bodyCell.style.minWidth = px;
        }
    };

    useLayoutEffect(() => {
        let frameId: number | null = null;

        const runSync = () => {
            frameId = requestAnimationFrame(() => {
                syncColumnWidths();
            });
        };

        runSync();

        const handleResize = () => {
            runSync();
        };

        window.addEventListener("resize", handleResize);

        return () => {
            window.removeEventListener("resize", handleResize);
            if (frameId !== null) {
                cancelAnimationFrame(frameId);
            }
        };
        // header/body как зависимости: при их изменении делаем новый sync
    }, [header, body]);

    return (
        <div className={`relative ${className}`}>
            {/* sticky-шапка секции */}
            <div
                className="
                    sticky z-20
                    bg-surface/95 backdrop-blur-sm
                "
                style={{ top: stickyOffset }}
            >
                <div
                    ref={headerScrollRef}
                    className="overflow-x-auto rounded-xl2 border border-border bg-panel"
                    onScroll={onHeaderScroll}
                >
                    <table
                        ref={headerTableRef}
                        className="min-w-full border-collapse text-[10px] leading-tight text-textDim"
                    >
                        {header}
                    </table>
                </div>
            </div>

            <div
                ref={bodyScrollRef}
                className="overflow-x-auto rounded-xl2 border border-border bg-panel shadow-panel"
                onScroll={onBodyScroll}
            >
                <table
                    ref={bodyTableRef}
                    className="min-w-full border-collapse text-[10px] leading-tight text-textDim"
                >
                    {body}
                </table>
            </div>
        </div>
    );
};
