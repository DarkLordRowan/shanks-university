import React, { useMemo, useState } from "react";
import { TREE } from "../data/algorithms";
import type { AlgNode } from "../types/algorithms.ts";
import { type Author, resolveAuthors } from "../types/author.ts";


/* ---------- helpers ---------- */
function flattenIds(nodes: AlgNode[]): string[] {
    const acc: string[] = [];
    const walk = (arr: AlgNode[]) => {
        for (const n of arr) {
            if (n.id) acc.push(n.id);
            if (n.children?.length) walk(n.children);
        }
    };
    walk(nodes);
    return acc;
}

/* ---------- Tree node ---------- */
type NodeProps = {
    node: AlgNode;
    level?: number;
    expanded: Set<string>;
    toggle: (id: string) => void;
    filter: string;
};

const TreeNode: React.FC<NodeProps> = ({node, level = 0, expanded, toggle, filter}) => {
    const hasChildren = !!node.children?.length;
    const authorList: Author[] = resolveAuthors(node.authorIds);

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
    }, [filter, node, authorList]);

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
            <div
                className="flex items-center gap-2 py-1"
                style={{paddingLeft: `${level * 14}px`}}
            >
                {hasChildren ? (
                    <button
                        onClick={() => toggle(node.id)}
                        className="inline-flex h-6 w-6 items-center justify-center rounded-md border border-border/60 bg-panel/70 text-textDim hover:text-white"
                        aria-label={isOpen ? "Collapse" : "Expand"}
                        title={isOpen ? "Свернуть" : "Развернуть"}
                    >
                        {isOpen ? "−" : "+"}
                    </button>
                ) : (
                    <span className="inline-block h-6 w-6"/>
                )}

                <div className="rounded-lg px-2 py-1">
                    <div className="font-medium text-white">{node.title}</div>
                    {node.subtitle && <div className="text-xs text-textDim">{node.subtitle}</div>}
                    {!!authorList.length && (
                        <div className="mt-1 flex flex-wrap gap-x-2 gap-y-1 text-xs text-textDim">
                            {authorList.map((a) =>
                                a.url ? (
                                    <a key={a.id} href={a.url} target="_blank" rel="noreferrer"
                                       className="underline hover:text-white">
                                        {a.name}
                                    </a>
                                ) : (
                                    <span key={a.id}>{a.name}</span>
                                )
                            )}
                        </div>
                    )}
                    {node.url && <u className="text-xs text-textDim">{node.url}</u>}
                </div>
            </div>

            {hasChildren && isOpen && (
                <div className="border-l border-border/40">
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

/* ---------- Page ---------- */
const Algorithms: React.FC = () => {
    const [query, setQuery] = useState("");
    const [expanded, setExpanded] = useState<Set<string>>(new Set());

    const allIds = useMemo(() => flattenIds(TREE), []);

    const expandAll = () => setExpanded(new Set(allIds));
    const collapseAll = () => setExpanded(new Set());
    const toggle = (id: string) =>
        setExpanded((prev) => {
            const next = new Set(prev);
            next.has(id) ? next.delete(id) : next.add(id);
            return next;
        });

    React.useEffect(() => {
        if (query.trim()) setExpanded(new Set(allIds));
    }, [query, allIds]);

    return (
        <div className="card">
            <div className="mb-4 flex flex-col gap-3 md:flex-row md:items-center md:justify-between">
                <h1 className="text-2xl font-bold text-primary">Алгоритмы ускорения сходимости</h1>

                <div className="flex gap-2">
                    <input
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                        placeholder="Поиск: epsilon, levin, wynn, …"
                        className="w-64 rounded-xl2 border border-border/60 bg-panel/70 px-3 py-2 text-sm outline-none focus:ring-2 focus:ring-primary/40"
                    />
                    <button onClick={expandAll} className="btn">Развернуть всё</button>
                    <button onClick={collapseAll} className="btn">Свернуть всё</button>
                </div>
            </div>

            <div className="rounded-xl2 border border-border/60 bg-panel/70 p-2">
                {TREE.map((root) => (
                    <TreeNode
                        key={root.id}
                        node={root}
                        expanded={expanded}
                        toggle={toggle}
                        filter={query}
                    />
                ))}
            </div>
        </div>
    );
};

export default Algorithms;
