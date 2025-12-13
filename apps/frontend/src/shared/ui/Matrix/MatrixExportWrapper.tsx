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
            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(25);

            const table = node.querySelector("table") as HTMLTableElement | null;

            const prevNodeOverflow = node.style.overflow;
            const prevNodeWidth = node.style.width;

            const prevTableWidth = table?.style.width ?? "";
            const prevTableMinWidth = table?.style.minWidth ?? "";

            node.style.overflow = "visible";

            if (table) {
                const fullW = Math.ceil(table.scrollWidth) + 2;
                table.style.width = `${fullW}px`;
                table.style.minWidth = `${fullW}px`;
            } else {
                const fullW = Math.ceil(node.scrollWidth) + 2;
                node.style.width = `${fullW}px`;
            }

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(50);

            const targetEl: HTMLElement = (table ?? node) as HTMLElement;

            const targetWidth = Math.ceil(targetEl.scrollWidth) + 2;
            const targetHeight = Math.ceil(targetEl.scrollHeight) + 2;

            const dataUrl = await htmlToImage.toPng(node, {
                cacheBust: true,
                pixelRatio: window.devicePixelRatio || 2,

                width: targetWidth,
                height: targetHeight,

                style: {
                    overflow: "visible",
                    width: `${targetWidth}px`,
                    height: `${targetHeight}px`,
                },
            });

            setProgress(90);

            const a = document.createElement("a");
            a.href = dataUrl;
            a.download = `${fileBaseName}.png`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);

            setProgress(100);

            node.style.overflow = prevNodeOverflow;
            node.style.width = prevNodeWidth;
            if (table) {
                table.style.width = prevTableWidth;
                table.style.minWidth = prevTableMinWidth;
            }
        } catch (e) {
            console.error("PNG export failed", e);
            setProgress(null);
        } finally {
            setTimeout(() => {
                setExporting(false);
                setProgress(null);
                setNoInnerScroll(false);
                setNoSticky(false);
            }, 250);
        }
    }, [exporting, fileBaseName]);

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
