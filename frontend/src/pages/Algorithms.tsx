import React, { useMemo, useState } from "react";

/** Узел дерева алгоритмов */
type AlgNode = {
    id: string;
    title: string;       // краткое имя (EN)
    subtitle?: string;   // подпись/перевод (RU)
    url?: string;
    document?: string;
    author?: Author[];
    children?: AlgNode[];
};

type Author = {
    id: string;
    name: string;
    url: string;
}

/** Данные: сгруппировал в семейства, но ты можешь свободно перестраивать дерево */
const TREE: AlgNode[] = [
    {
        id: "weniger_algorithm",
        title: "weniger_algorithm",
        subtitle: "Алгоритм Венигера (Преобразование Венигера)",
        document: "Weniger.pdf -> weniger_algorithm.pdf",
        children: []
    },
    {
        id: "drummond_d_algorithm",
        title: "drummond_d_algorithm",
        subtitle: "Алгоритм Драммонда",
        children: []
    },
    {
        id: "",
        title: "Алгоритм Левина (Levin’s Transformation)",
        subtitle: "",
        children: []
    },
    {
        id: "",
        title: "Алгоритм Лубкина (W-алгоритм Лубкина)",
        subtitle: "",
        children: []
    },
    {
        id: "",
        title: "Алгоритм Ричардсона (Richardson Extrapolation)",
        subtitle: "",
        children: []
    },
    {
        id: "ford_sidi_algorithm",
        title: "ford_sidi_algorithm",
        subtitle: "Алгоритм Форд–Сиди (Ford–Sidi Algorithm, W_k)",
        children: [
            { id: "ford_sidi_one_algorithm", title: "ford_sidi_one_algorithm", subtitle: "Алгоритм Форд–Сиди (первая версия))" },
            { id: "ford_sidi_two_algorithm", title: "ford_sidi_two_algorithm", subtitle: "Алгоритм Форд–Сиди (вторая версия)" },
            { id: "ford_sidi_three_algorithm", title: "ford_sidi_three_algorithm", subtitle: "Алгоритм Форд–Сиди (третья версия)" },
        ]
    },
    {
        id: "chang_wynn_algorithm",
        title: "chang_wynn_algorithm",
        subtitle: "Алгоритм Чанг–Винна (Chang–Wynn Algorithm)",
        children: []
    },
    {
        id: "",
        title: "",
        subtitle: "Алгоритм Шенкса (Shanks’ Algorithm)",
        children: []
    },
    {
        id: "wynn_epsilon_algorithm",
        title: "wynn_epsilon_algorithm",
        subtitle: "Эпсилон-алгоритм Винна (Wynn’s ε-algorithm)",
        document: "",
        author: [
            {
                id: "Peter Wynn",
                name: "Peter Wynn",
                url: "https://en.wikipedia.org/wiki/Peter_Wynn_%28mathematician%29",
            }
        ],
        children: [
            { id: "wynn_epsilon_one_algorithm", title: "wynn_epsilon_one_algorithm", subtitle: "Эпсилон-алгоритм Винна (базовый)" },
            { id: "wynn_epsilon_two_algorithm", title: "wynn_epsilon_two_algorithm", subtitle: "Эпсилон-алгоритм Винна (вторая версия)" },
            { id: "wynn_epsilon_three_algorithm", title: "wynn_epsilon_three_algorithm", subtitle: "Эпсилон-алгоритм Винна (третья версия)" },
        ]
    },
    {
        id: "wynn_rho_algorithm",
        title: "wynn_rho_algorithm",
        subtitle: "Ро-алгоритм Винна (Wynn’s rho-algorithm)",
        author: [
            {
                id: "Peter Wynn",
                name: "Peter Wynn",
                url: "https://en.wikipedia.org/wiki/Peter_Wynn_%28mathematician%29",
            }
        ],
        children: []
    },
    {
        id: "",
        title: "Тета-алгоритм Брезински (Brezinski’s θ-algorithm)",
        subtitle: "",
        children: []
    },
];

/* ----------------------------- UI компоненты ----------------------------- */

function flattenIds(nodes: AlgNode[]): string[] {
    const acc: string[] = [];
    const walk = (arr: AlgNode[]) => {
        for (const n of arr) {
            acc.push(n.id);
            if (n.children?.length) walk(n.children);
        }
    };
    walk(nodes);
    return acc;
}

type NodeProps = {
    node: AlgNode;
    level?: number;
    expanded: Set<string>;
    toggle: (id: string) => void;
    filter: string;
};

const TreeNode: React.FC<NodeProps> = ({ node, level = 0, expanded, toggle, filter }) => {
    const hasChildren = !!node.children?.length;

    // match by id/title/subtitle
    const matches = useMemo(() => {
        if (!filter) return true;
        const q = filter.toLowerCase();
        return (
            node.id.toLowerCase().includes(q) ||
            node.title.toLowerCase().includes(q) ||
            (node.subtitle?.toLowerCase().includes(q) ?? false)
        );
    }, [filter, node]);

    // если не совпадает узел и нет совпадений в детях — скрываем
    const hasMatchInChildren = useMemo(() => {
        if (!node.children) return false;
        const check = (n: AlgNode): boolean =>
            (n.id + n.title + (n.subtitle ?? "")).toLowerCase().includes(filter.toLowerCase()) ||
            (!!n.children && n.children.some(check));
        return node.children.some(check);
    }, [node.children, filter]);

    if (filter && !matches && !hasMatchInChildren) return null;

    const isOpen = expanded.has(node.id);

    return (
        <div className="select-none">
            <div
                className="flex items-center gap-2 py-1"
                style={{ paddingLeft: `${level * 14}px` }}
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
                    <span className="inline-block h-6 w-6" />
                )}

                <div className="rounded-lg px-2 py-1">
                    <div className="font-medium text-white">{node.title}</div>
                    {node.subtitle && (
                        <div className="text-xs text-textDim">{node.subtitle}</div>
                    )}
                    {node.url && (
                        <u className="text-xs text-textDim">{node.url}</u>
                    )}
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

    // При поиске автоматически раскрываем все для удобства просмотра результатов
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
                        placeholder="Поиск: epsilon, levin, rho, …"
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
