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
    const syncingRef = useRef(false);

    const headerTableRef = useRef<HTMLTableElement | null>(null);
    const bodyTableRef = useRef<HTMLTableElement | null>(null);

    const syncScroll = (source: "header" | "body") => {
        if (syncingRef.current) return;
        syncingRef.current = true;

        const headerEl = headerScrollRef.current;
        const bodyEl = bodyScrollRef.current;
        if (!headerEl || !bodyEl) {
            syncingRef.current = false;
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
            syncingRef.current = false;
        });
    };

    const onHeaderScroll: React.UIEventHandler<HTMLDivElement> = () => {
        syncScroll("header");
    };

    const onBodyScroll: React.UIEventHandler<HTMLDivElement> = () => {
        syncScroll("body");
    };

    const syncColumnWidths = () => {
        const headerTable = headerTableRef.current;
        const bodyTable = bodyTableRef.current;
        if (!headerTable || !bodyTable) return;

        const thead = headerTable.tHead;
        const tbody = bodyTable.tBodies[0];
        if (!thead || !tbody) return;

        const headerRow = thead.rows.length > 0 ? thead.rows[thead.rows.length - 1] : null;
        const bodyRow = tbody.rows[0];

        if (!headerRow || !bodyRow) return;

        const headerCells = Array.from(headerRow.children) as HTMLTableCellElement[];
        const bodyCells = Array.from(bodyRow.children) as HTMLTableCellElement[];

        const len = Math.min(headerCells.length, bodyCells.length);

        for (let i = 0; i < len; i++) {
            const bodyCell = bodyCells[i];
            const headerCell = headerCells[i];

            const width = bodyCell.getBoundingClientRect().width;

            headerCell.style.width = `${width}px`;
            headerCell.style.minWidth = `${width}px`;

            bodyCell.style.width = `${width}px`;
            bodyCell.style.minWidth = `${width}px`;
        }
    };

    useLayoutEffect(() => {
        syncColumnWidths();

        const handleResize = () => {
            syncColumnWidths();
        };

        window.addEventListener("resize", handleResize);
        return () => {
            window.removeEventListener("resize", handleResize);
        };
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
