import React, { memo, useMemo } from "react";
import katex from "katex";
import "katex/dist/katex.min.css";

type FormulaProps = {
    latex: string;
    inline?: boolean;
    className?: string;
};

export const Formula: React.FC<FormulaProps> = memo(({latex, inline = true, className}) => {
    const html = useMemo(() => {
        return katex.renderToString(latex, {
            displayMode: !inline,
            throwOnError: false,
            strict: "warn",
            trust: false,
            output: "html",
        });
    }, [latex, inline]);

    const Tag = inline ? "span" : "div";
    return <Tag className={className} dangerouslySetInnerHTML={{__html: html}}/>;
});
