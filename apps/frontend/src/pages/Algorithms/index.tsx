import React, { useMemo, useState } from "react";
import { TREE } from "../../data/algorithms";
import type { AlgNode } from "../../types/algorithms";
import { flattenIds } from "./utils/tree";
import { TreeNode } from "./components/TreeNode";

const Algorithms: React.FC = () => {
    const [query, setQuery] = useState("");
    const [expanded, setExpanded] = useState<Set<string>>(new Set());

    const allIds = useMemo(() => flattenIds(TREE as AlgNode[]), []);

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

            <div className="space-y-2 rounded-xl2 border border-border/60 bg-panel/50 p-2">
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
