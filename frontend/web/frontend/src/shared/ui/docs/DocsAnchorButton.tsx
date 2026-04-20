import React, { useCallback } from "react";
import { buildDocsHref } from "@/shared/lib/docs/tableDocs";

type DocsAnchorButtonProps = {
    anchorId: string;
    label: string;
    className?: string;
};

export const DocsAnchorButton: React.FC<DocsAnchorButtonProps> = ({
    anchorId,
    label,
    className,
}) => {
    const href = buildDocsHref(anchorId);

    const stopPropagation = useCallback((event: React.SyntheticEvent) => {
        event.stopPropagation();
    }, []);

    return (
        <a
            href={href}
            target="_blank"
            rel="noopener noreferrer"
            aria-label={`Открыть документацию: ${label}`}
            title={`Документация: ${label}`}
            onPointerDown={stopPropagation}
            onMouseDown={stopPropagation}
            onClick={stopPropagation}
            className={[
                "inline-flex h-5 w-5 items-center justify-center rounded-full border border-border/80 bg-surface/95 text-[10px] font-semibold text-textDim shadow-panel transition",
                "opacity-100 sm:opacity-0 sm:group-hover:opacity-100 sm:group-focus-within:opacity-100 focus-visible:opacity-100",
                "hover:border-primary/60 hover:text-white",
                className ?? "",
            ].join(" ")}
        >
            ?
        </a>
    );
};
