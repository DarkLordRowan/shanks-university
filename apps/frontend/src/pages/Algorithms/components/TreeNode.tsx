import React, { useMemo } from "react";
import type { AlgNode } from "../../../types/algorithms";
import { resolveAuthors } from "../../../data/authors";
import { Link } from "react-router-dom";
import { depthAccentClass } from "../utils/ui";

export type NodeProps = {
    node: AlgNode;
    level?: number;
    expanded: Set<string>;
    toggle: (id: string) => void;
    filter: string;
};

export const TreeNode: React.FC<NodeProps> = (
    {
        node,
        level = 0,
        expanded,
        toggle,
        filter,
    }) => {
    const hasChildren = !!node.children?.length;

    const authorList = useMemo(() => resolveAuthors(node.authorIds), [node.authorIds]);

    const matches = useMemo(() => {
        if (!filter) return true;
        const q = filter.toLowerCase();
        const authors = authorList.map((a) => a.name.toLowerCase()).join(" ");
        return (
            node.id.toLowerCase().includes(q) ||
            node.title.toLowerCase().includes(q) ||
            (node.subtitle?.toLowerCase().includes(q) ?? false) ||
            authors.includes(q)
        );
    }, [filter, node.id, node.title, node.subtitle, authorList]);

    const hasMatchInChildren = useMemo(() => {
        if (!node.children?.length || !filter) return false;
        const q = filter.toLowerCase();
        const check = (n: AlgNode): boolean => {
            const a = resolveAuthors(n.authorIds).map((x) => x.name.toLowerCase()).join(" ");
            const self =
                n.id.toLowerCase().includes(q) ||
                n.title.toLowerCase().includes(q) ||
                (n.subtitle?.toLowerCase().includes(q) ?? false) ||
                a.includes(q);
            return self || (n.children?.some(check) ?? false);
        };
        return node.children.some(check);
    }, [node.children, filter]);

    if (filter && !matches && !hasMatchInChildren) return null;

    const isOpen = expanded.has(node.id);

    return (
        <div className="select-none">
            {/* строка узла */}
            <div
                className={`group relative my-1 flex items-start gap-2 rounded-xl2 border border-border/60 bg-panel/70 px-3 py-2 shadow-panel hover:bg-panel/80`}
                style={{marginLeft: `${level * 14}px`}}
            >
                {/* левая цветная полоса по уровню */}
                <div
                    className={`absolute left-0 top-0 h-full w-[3px] rounded-l-xl2 ${depthAccentClass(level)}`}
                />

                {/* кнопка раскрытия / отступ */}
                {hasChildren ? (
                    <button
                        onClick={() => toggle(node.id)}
                        className="mt-1 inline-flex h-6 w-6 shrink-0 items-center justify-center rounded-md border border-border/60 bg-surface/40 text-textDim transition-colors hover:text-white"
                        aria-label={isOpen ? "Collapse" : "Expand"}
                        title={isOpen ? "Свернуть" : "Развернуть"}
                    >
                        {isOpen ? "−" : "+"}
                    </button>
                ) : (
                    <span className="mt-1 inline-block h-6 w-6 shrink-0"/>
                )}

                {/* контент узла */}
                <div className="min-w-0">
                    <div className="flex items-center gap-2">
                        <Link
                            to={`/algorithms/${node.id}`}
                            className="font-semibold tracking-wide text-white hover:underline"
                        >
                            {node.title}
                        </Link>
                        <code className="rounded-md bg-surface/40 px-2 py-[2px] text-[11px] text-textDim">
                            {node.id}
                        </code>
                    </div>

                    {node.subtitle && (
                        <div className="mt-0.5 text-sm text-textDim">{node.subtitle}</div>
                    )}

                    {!!authorList.length && (
                        <div className="mt-2 flex flex-wrap items-center gap-2">
                            <span className="text-xs text-textDim">Авторы:</span>
                            {authorList.map((a) =>
                                    a.url ? (
                                        <a
                                            key={a.id}
                                            href={a.url}
                                            target="_blank"
                                            rel="noreferrer"
                                            className="rounded-md border border-border/60 bg-surface/40 px-2 py-0.5 text-xs underline hover:text-white"
                                        >
                                            {a.name}
                                        </a>
                                    ) : (
                                        <span
                                            key={a.id}
                                            className="rounded-md border border-border/60 bg-surface/40 px-2 py-0.5 text-xs text-textDim"
                                        >
                    {a.name}
                  </span>
                                    )
                            )}
                        </div>
                    )}

                    {node.url && (
                        <div className="mt-1 text-xs text-textDim underline">{node.url}</div>
                    )}
                </div>
            </div>

            {/* дети + вертикальная направляющая */}
            {hasChildren && isOpen && (
                <div className="ml-[14px] border-l border-dashed border-border/40">
                    {node.children!.map((ch) => (
                        <TreeNode
                            key={ch.id}
                            node={ch}
                            level={level + 1}
                            expanded={expanded}
                            toggle={toggle}
                            filter={filter}
                        />
                    ))}
                </div>
            )}
        </div>
    );
};
