import React from "react";
import ReactMarkdown from "react-markdown";
import remarkMath from "remark-math";
import rehypeKatex from "rehype-katex";
import "katex/dist/katex.min.css";

export const Formula: React.FC<{ latex: string }> = ({ latex }) => (
    <ReactMarkdown
        remarkPlugins={[remarkMath]}
        rehypePlugins={[rehypeKatex]}
        components={{
            p: (props) => <span {...props} />,
        }}
    >
        {latex.includes("$$") || latex.includes("$") ? latex : `$${latex}$`}
    </ReactMarkdown>
);
