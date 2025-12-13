// src/shared/ui/Matrix/MatrixExportWrapper.tsx

import React, { useCallback, useRef, useState } from "react";
import * as htmlToImage from "html-to-image";
import * as XLSX from "xlsx-js-style";

export interface MatrixExportWrapperProps {
    fileBaseName: string;
    buildWorkbook: () => XLSX.WorkBook;
    enablePng?: boolean;
    enableXlsx?: boolean;
    children: (opts: {
        exporting: boolean;
        noInnerScroll: boolean;
        noSticky: boolean;
        captureRef: React.RefObject<HTMLDivElement | null>;
    }) => React.ReactNode;
}

export const MatrixExportWrapper: React.FC<MatrixExportWrapperProps> = ({
    fileBaseName,
    buildWorkbook,
    enablePng = true,
    enableXlsx = true,
    children,
}) => {
    const captureRef = useRef<HTMLDivElement | null>(null);

    const [exporting, setExporting] = useState(false);
    const [progress, setProgress] = useState<number | null>(null);
    const [noInnerScroll, setNoInnerScroll] = useState(false);
    const [noSticky, setNoSticky] = useState(false);

    /* ---------- PNG ---------- */

    const exportPng = useCallback(async () => {
        if (exporting) return;
        const node = captureRef.current;
        if (!node) return;

        setExporting(true);
        setProgress(5);

        try {
            setNoInnerScroll(true);
            setNoSticky(true);

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(25);

            const table = node.querySelector("table") as HTMLTableElement | null;

            const prevOverflow = node.style.overflow;
            const prevWidth = node.style.width;
            const prevTableWidth = table?.style.width ?? "";

            node.style.overflow = "visible";
            if (table) {
                table.style.width = `${table.scrollWidth}px`;
            } else {
                node.style.width = `${node.scrollWidth}px`;
            }

            setProgress(50);

            const dataUrl = await htmlToImage.toPng(node, {
                pixelRatio: window.devicePixelRatio || 2,
                cacheBust: true,
            });

            setProgress(90);

            const a = document.createElement("a");
            a.href = dataUrl;
            a.download = `${fileBaseName}.png`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);

            node.style.overflow = prevOverflow;
            node.style.width = prevWidth;
            if (table) table.style.width = prevTableWidth;

            setProgress(100);
        } finally {
            setTimeout(() => {
                setExporting(false);
                setProgress(null);
                setNoInnerScroll(false);
                setNoSticky(false);
            }, 250);
        }
    }, [exporting, fileBaseName]);

    /* ---------- XLSX ---------- */

    const exportXlsx = useCallback(() => {
        const wb = buildWorkbook();
        const out = XLSX.write(wb, { bookType: "xlsx", type: "array" });

        const blob = new Blob([out], {
            type: "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        });

        const url = URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = `${fileBaseName}.xlsx`;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
    }, [buildWorkbook, fileBaseName]);

    return (
        <div className="relative">
            <div className="mb-2 flex items-center gap-2 text-[10px] text-textDim">
                {enablePng && (
                    <button
                        type="button"
                        className="rounded border border-border bg-surface px-2 py-[3px] hover:bg-panel disabled:opacity-50"
                        onClick={exportPng}
                        disabled={exporting}
                    >
                        {exporting ? "PNG…" : "PNG"}
                    </button>
                )}

                {enableXlsx && (
                    <button
                        type="button"
                        className="rounded border border-border bg-surface px-2 py-[3px] hover:bg-panel"
                        onClick={exportXlsx}
                    >
                        XLSX
                    </button>
                )}

                {progress !== null && (
                    <div className="flex items-center gap-1 min-w-[80px]">
                        <span className="tabular-nums">{progress}%</span>
                        <div className="h-[4px] flex-1 rounded bg-border/40 overflow-hidden">
                            <div
                                className="h-full bg-accent/80 transition-all"
                                style={{ width: `${progress}%` }}
                            />
                        </div>
                    </div>
                )}
            </div>

            {children({
                exporting,
                noInnerScroll,
                noSticky,
                captureRef,
            })}
        </div>
    );
};
