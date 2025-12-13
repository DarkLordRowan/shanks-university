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

        const EXTRA_PX = 12;

        try {
            setNoInnerScroll(true);
            setNoSticky(true);

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(25);

            const table = node.querySelector("table") as HTMLTableElement | null;
            const targetEl = (table ?? node) as HTMLElement;
            const rect = targetEl.getBoundingClientRect();

            const targetWidth = Math.ceil(rect.width) + EXTRA_PX;
            const targetHeight = Math.ceil(rect.height) + EXTRA_PX;

            // 2) Клонируем и рендерим клон вне экрана
            const clone = node.cloneNode(true) as HTMLDivElement;

            // контейнер-стейдж, чтобы клон не влиял на страницу
            const stage = document.createElement("div");
            stage.style.position = "fixed";
            stage.style.left = "-100000px";
            stage.style.top = "0";
            stage.style.width = `${targetWidth}px`;
            stage.style.height = `${targetHeight}px`;
            stage.style.overflow = "visible";
            stage.style.pointerEvents = "none";
            stage.style.zIndex = "-1";

            // фиксируем размеры клона, но НЕ меняем display (никакого inline-block)
            clone.style.overflow = "visible";
            clone.style.width = `${targetWidth}px`;
            clone.style.height = `${targetHeight}px`;
            clone.style.boxSizing = "border-box";
            clone.style.paddingBottom = `${EXTRA_PX}px`;

            // если внутри есть table, лучше стабилизировать layout на клоне
            const cloneTable = clone.querySelector("table") as HTMLTableElement | null;
            if (cloneTable) {
                // важно: фиксируем ширину, но только на клоне
                cloneTable.style.width = `${targetWidth}px`;
                cloneTable.style.minWidth = `${targetWidth}px`;
                // чтобы не было переразметки колонок по содержимому
                (cloneTable.style as any).tableLayout = "fixed";
            }

            stage.appendChild(clone);
            document.body.appendChild(stage);

            await new Promise<void>((r) => requestAnimationFrame(() => r()));
            setProgress(50);

            const dataUrl = await htmlToImage.toPng(clone, {
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

            document.body.removeChild(stage);

            setProgress(90);

            const a = document.createElement("a");
            a.href = dataUrl;
            a.download = `${fileBaseName}.png`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);

            setProgress(100);
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
