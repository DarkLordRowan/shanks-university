import React from "react";
import ReactMarkdown from "react-markdown";
import remarkMath from "remark-math";
import rehypeKatex from "rehype-katex";
import "katex/dist/katex.min.css";

/**
 * Рендерит формулу. Можно передать:
 *  - raw LaTeX, например `\sum_{n=0}^{\infty} r^n = \frac{1}{1-r}`
 *  - или markdown c блоками $$ ... $$
 */
export const FormulaPreview: React.FC<{ latex: string; title?: string }> = ({ latex, title }) => {
    const content = latex.includes("$$") || latex.includes("$")
        ? latex
        : `$$\n${latex}\n$$`;

    return (
        <div className="rounded-2xl border border-border/60 bg-panel/70 p-4">
            {title && <div className="mb-2 text-sm text-textDim">{title}</div>}
            <div className="prose prose-invert max-w-none prose-p:my-0">
                <ReactMarkdown
                    remarkPlugins={[remarkMath]}
                    rehypePlugins={[rehypeKatex]}
                >
                    {content}
                </ReactMarkdown>
            </div>
        </div>
    );
};
