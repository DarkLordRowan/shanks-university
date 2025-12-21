// src/shared/ui/Matrix/MatrixExportWrapper.tsx

import React, { useCallback, useRef, useState } from "react";
import * as htmlToImage from "html-to-image";
import * as XLSX from "xlsx-js-style";

export interface MatrixExportWrapperProps {
    fileBaseName: string;
    buildWorkbook?: () => XLSX.WorkBook;
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
    enablePng = false,
    enableXlsx = false,
    children,
}) => {
    const captureRef = useRef<HTMLDivElement | null>(null);

    const [exporting, setExporting] = useState(false);
    const [progress, setProgress] = useState<number | null>(null);
    const [noInnerScroll, setNoInnerScroll] = useState(false);
    const [noSticky, setNoSticky] = useState(false);

    const exportJpeg = useCallback(async () => {
        if (exporting) return;
        const node = captureRef.current;
        if (!node) return;

        setExporting(true);
        setProgress(5);

        const EXTRA_PX = 12;
        const EXPORT_BG = "#0b1220";
        const JPEG_QUALITY = 0.95;

        const EXPORT_CLASS = "matrix-exporting";
        const ensureExportStyle = () => {
            const id = "matrix-export-style";
            if (document.getElementById(id)) return;
            const style = document.createElement("style");
            style.id = id;
            style.textContent = `
                body.${EXPORT_CLASS} * {
                    transition: none !important;
                    animation: none !important;
                    caret-color: transparent !important;
                }
            `;
            document.head.appendChild(style);
        };

        try {
            ensureExportStyle();

            setNoInnerScroll(true);
            setNoSticky(true);

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            await new Promise<void>((r) => requestAnimationFrame(() => r()));

            const fontsAny = (document as any).fonts;
            if (fontsAny?.ready) await fontsAny.ready;

            document.body.classList.add(EXPORT_CLASS);

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(25);

            const table = node.querySelector("table") as HTMLTableElement | null;
            const targetEl = (table ?? node) as HTMLElement;

            // размеры по контенту (не по viewport)
            const targetWidth = Math.ceil(targetEl.scrollWidth) + EXTRA_PX;
            const targetHeight = Math.ceil(targetEl.scrollHeight) + EXTRA_PX;

            const clone = node.cloneNode(true) as HTMLDivElement;

            clone.style.overflow = "visible";
            clone.style.width = `${targetWidth}px`;
            clone.style.height = `${targetHeight}px`;
            clone.style.boxSizing = "border-box";
            clone.style.paddingBottom = `${EXTRA_PX}px`;
            clone.style.transform = "none";
            (clone.style as any).zoom = "1";

            const cloneTable = clone.querySelector("table") as HTMLTableElement | null;
            if (cloneTable) {
                cloneTable.style.width = `${targetWidth}px`;
                cloneTable.style.minWidth = `${targetWidth}px`;
                (cloneTable.style as any).tableLayout = "fixed";
            }

            const stage = document.createElement("div");
            stage.style.position = "fixed";
            stage.style.left = "0";
            stage.style.top = "0";
            stage.style.width = `${targetWidth}px`;
            stage.style.height = `${targetHeight}px`;
            stage.style.opacity = "0";
            stage.style.pointerEvents = "none";
            stage.style.overflow = "visible";
            stage.style.zIndex = "-1";

            stage.appendChild(clone);
            document.body.appendChild(stage);

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(50);

            const canvas = await htmlToImage.toCanvas(clone, {
                cacheBust: true,
                pixelRatio: 2,
                width: targetWidth,
                height: targetHeight,
                style: {
                    width: `${targetWidth}px`,
                    height: `${targetHeight}px`,
                    overflow: "visible",
                    backgroundColor: "transparent",
                    transform: "none",
                },
            });

            const ctx = canvas.getContext("2d");
            if (!ctx) throw new Error("Canvas 2D context not available");

            ctx.globalCompositeOperation = "destination-over";
            ctx.fillStyle = EXPORT_BG;
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            setProgress(80);

            const dataUrl = canvas.toDataURL("image/jpeg", JPEG_QUALITY);

            document.body.removeChild(stage);
            document.body.classList.remove(EXPORT_CLASS);

            setProgress(90);

            const a = document.createElement("a");
            a.href = dataUrl;
            a.download = `${fileBaseName}.jpg`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);

            setProgress(100);
        } catch (e) {
            console.error("JPEG export failed", e);
            setProgress(null);
        } finally {
            document.body.classList.remove("matrix-exporting");
            setTimeout(() => {
                setExporting(false);
                setProgress(null);
                setNoInnerScroll(false);
                setNoSticky(false);
            }, 250);
        }
    }, [exporting, fileBaseName]);

    const exportXlsx = useCallback(() => {
        if (!enableXlsx || buildWorkbook == undefined) {
            return;
        }
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
    }, [buildWorkbook, enableXlsx, fileBaseName]);

    return (
        <div className="relative">
            {children({
                exporting,
                noInnerScroll,
                noSticky,
                captureRef,
            })}
            <div className="mb-2 flex items-center gap-2 text-[10px] text-textDim">
                Export:
                {enablePng && (
                    <button
                        type="button"
                        className="rounded border border-border bg-surface px-2 py-[3px] hover:bg-panel disabled:opacity-50"
                        onClick={exportJpeg}
                        disabled={exporting}
                        title="Экспорт без прозрачности (JPEG)"
                    >
                        {exporting ? "IMG…" : "IMG"}
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
        </div>
    );
};
