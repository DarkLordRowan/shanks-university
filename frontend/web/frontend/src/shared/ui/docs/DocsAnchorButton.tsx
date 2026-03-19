import React, { useCallback } from "react";
import { useLocation, useNavigate } from "react-router-dom";
import { buildDocsHref } from "@/shared/lib/docs/tableDocs";

type DocsAnchorButtonProps = {
    anchorId: string;
    label: string;
    className?: string;
};

function scrollToAnchor(anchorId: string) {
    const element = document.getElementById(anchorId);
    if (!element) return;
    element.scrollIntoView({ behavior: "smooth", block: "start" });
}

export const DocsAnchorButton: React.FC<DocsAnchorButtonProps> = ({
    anchorId,
    label,
    className,
}) => {
    const navigate = useNavigate();
    const location = useLocation();

    const stopPropagation = useCallback((event: React.SyntheticEvent) => {
        event.stopPropagation();
    }, []);

    const handleClick = useCallback(
        (event: React.MouseEvent<HTMLButtonElement>) => {
            event.preventDefault();
            event.stopPropagation();

            const href = buildDocsHref(anchorId);
            const sameDocsPage = location.pathname === "/docs";
            const sameHash = location.hash === `#${anchorId}`;

            if (sameDocsPage && sameHash) {
                scrollToAnchor(anchorId);
                return;
            }

            navigate(href);
        },
        [anchorId, location.hash, location.pathname, navigate]
    );

    return (
        <button
            type="button"
            aria-label={`Открыть документацию: ${label}`}
            title={`Документация: ${label}`}
            onPointerDown={stopPropagation}
            onMouseDown={stopPropagation}
            onClick={handleClick}
            className={[
                "inline-flex h-5 w-5 items-center justify-center rounded-full border border-border/80 bg-surface/95 text-[10px] font-semibold text-textDim shadow-panel transition",
                "opacity-100 sm:opacity-0 sm:group-hover:opacity-100 sm:group-focus-within:opacity-100 focus-visible:opacity-100",
                "hover:border-primary/60 hover:text-white",
                className ?? "",
            ].join(" ")}
        >
            ?
        </button>
    );
};
