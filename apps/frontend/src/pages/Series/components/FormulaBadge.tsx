import React from "react";
import ReactMarkdown from "react-markdown";
import remarkMath from "remark-math";
import rehypeKatex from "rehype-katex";
import "katex/dist/katex.min.css";

export const FormulaBadge: React.FC<{ latex: string }> = ({ latex }) => (
    <div className="rounded-2xl border border-border/50 bg-surface/30 px-4 py-6 text-center shadow-panel">
        <div className="prose prose-invert max-w-none text-lg md:text-xl leading-tight">
            <ReactMarkdown remarkPlugins={[remarkMath]} rehypePlugins={[rehypeKatex]}>
                {latex.includes("$$") || latex.includes("$") ? latex : `$$${latex}$$`}
            </ReactMarkdown>
        </div>
    </div>
);
