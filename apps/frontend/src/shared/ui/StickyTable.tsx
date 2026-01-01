// src/shared/ui/StickyTable.tsx
import React, { useLayoutEffect, useRef } from "react";

interface StickyTableProps {
    header: React.ReactNode;
    body: React.ReactNode;
    stickyOffset?: number;
    className?: string;

    /** ref на внешний контейнер, чтобы родитель мог делать snapshot */
    wrapperRef?: React.Ref<HTMLDivElement>;
    /** если true, убираем overflow-x-auto, показываем всю таблицу целиком */
    noScroll?: boolean;
}

export const StickyTable: React.FC<StickyTableProps> = ({
    header,
    body,
    stickyOffset = 0,
    className = "",
    wrapperRef,
    noScroll = false,
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

        if (!noScroll) {
            if (source === "header") {
                if (bodyEl.scrollLeft !== headerEl.scrollLeft) {
                    bodyEl.scrollLeft = headerEl.scrollLeft;
                }
            } else {
                if (headerEl.scrollLeft !== bodyEl.scrollLeft) {
                    headerEl.scrollLeft = bodyEl.scrollLeft;
                }
            }
        }

        requestAnimationFrame(() => {
            syncingScrollRef.current = false;
        });
    };

    const onHeaderScroll: React.UIEventHandler<HTMLDivElement> = () => {
        if (noScroll) return;
        syncScroll("header");
    };

    const onBodyScroll: React.UIEventHandler<HTMLDivElement> = () => {
        if (noScroll) return;
        syncScroll("body");
    };

    /* ---------- синхронизация ширины колонок ---------- */

    const syncColumnWidths = () => {
        const headerTable = headerTableRef.current;
        const bodyTable = bodyTableRef.current;
        if (!headerTable || !bodyTable) return;

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

        const widths: number[] = [];
        for (let i = 0; i < len; i++) {
            const w = bodyCells[i].getBoundingClientRect().width;
            widths.push(w);
        }

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
    }, [header, body]);

    const headerWrapperClass = noScroll
        ? "" // без sticky, без z-index, без подложки
        : "sticky z-20 bg-surface/95 backdrop-blur-sm";

    const headerWrapperStyle = noScroll ? undefined : { top: stickyOffset };

    return (
        <div ref={wrapperRef} className={`relative ${className}`}>
            {/* шапка секции (липкая только когда noScroll = false) */}
            <div className={headerWrapperClass} style={headerWrapperStyle}>
                <div
                    ref={headerScrollRef}
                    className={
                        (noScroll ? "overflow-visible " : "overflow-x-auto ") +
                        "rounded-xl2 border border-border bg-panel"
                    }
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
                className={
                    (noScroll ? "overflow-visible " : "overflow-x-auto ") +
                    "rounded-xl2 border border-border bg-panel shadow-panel"
                }
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
