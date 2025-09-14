import React from "react";
import { useParams, Link } from "react-router-dom";
import { TREE } from "../../data/algorithms";
import { resolveAuthors } from "../../data/authors";
import type { AlgNode } from "../../types/algorithms";

import { findNodeById } from "./utils/tree";
import { buildSrcLinks } from "./utils/github";
import { CodeViewer } from "./components/CodeViewer";
import { MarkdownDoc } from "./components/MarkdownDoc";

const Algorithm: React.FC = () => {
    const { id } = useParams<{ id: string }>();
    const node = id ? findNodeById(TREE as AlgNode[], id) : undefined;

    const [layout, setLayout] = React.useState<"stack" | "split">(
        (localStorage.getItem("alg-layout") as "stack" | "split") || "stack"
    );
    React.useEffect(() => {
        localStorage.setItem("alg-layout", layout);
    }, [layout]);

    if (!node) {
        return (
            <div className="card">
                <h1 className="mb-2 text-xl font-bold text-red-400">Алгоритм не найден</h1>
                <Link to="/algorithms" className="link">
                    ← Назад к списку алгоритмов
                </Link>
            </div>
        );
    }

    const authors = resolveAuthors(node.authorIds);
    const hasChildren = !!node.children?.length;
    const implementations = (node.children ?? []).slice().sort((a, b) => {
        const aBase = /базов|(^|_)one(_|$)/i.test((a.subtitle ?? "") + " " + a.id + " " + a.title);
        const bBase = /базов|(^|_)one(_|$)/i.test((b.subtitle ?? "") + " " + b.id + " " + b.title);
        return Number(bBase) - Number(aBase);
    });

    const { raw, web, path } = buildSrcLinks(node.id);

    return (
        <div className="space-y-4">
            {/* Шапка */}
            <div className="card space-y-3">
                <div className="flex flex-wrap items-center justify-between gap-2">
                    <h1 className="text-2xl font-bold text-primary">{node.title}</h1>

                    {/* Переключатель раскладки — только для листьев */}
                    {!hasChildren && (
                        <div className="flex items-center gap-2">
                            <span className="text-sm text-textDim">Вид:</span>
                            <div className="flex overflow-hidden rounded-xl2 border border-border/60">
                                <button
                                    onClick={() => setLayout("stack")}
                                    className={`px-3 py-1 text-sm ${
                                        layout === "stack" ? "bg-accent/90 text-black" : "bg-panel/70 text-textDim hover:text-white"
                                    }`}
                                >
                                    В столбик
                                </button>
                                <button
                                    onClick={() => setLayout("split")}
                                    className={`px-3 py-1 text-sm ${
                                        layout === "split" ? "bg-accent/90 text-black" : "bg-panel/70 text-textDim hover:text-white"
                                    }`}
                                >
                                    Код слева / Теория справа
                                </button>
                            </div>
                        </div>
                    )}
                </div>

                {node.subtitle && <p className="text-sm text-textDim">{node.subtitle}</p>}

                {!!authors.length && (
                    <div>
                        <h2 className="font-semibold">Авторы:</h2>
                        <div className="mt-1 flex flex-wrap gap-2">
                            {authors.map((a) =>
                                    a.url ? (
                                        <a
                                            key={a.id}
                                            href={a.url}
                                            target="_blank"
                                            rel="noreferrer"
                                            className="rounded-md border border-border/60 bg-surface/40 px-2 py-0.5 text-sm underline hover:text-white"
                                        >
                                            {a.name}
                                        </a>
                                    ) : (
                                        <span
                                            key={a.id}
                                            className="rounded-md border border-border/60 bg-surface/40 px-2 py-0.5 text-sm text-textDim"
                                        >
                    {a.name}
                  </span>
                                    )
                            )}
                        </div>
                    </div>
                )}

                {hasChildren ? (
                    <>
                        <div className="rounded-xl2 border border-border/60 bg-panel/70 p-3">
                            <div className="mb-2 font-semibold">Реализации:</div>
                            <ul className="space-y-1">
                                {implementations.map((child) => (
                                    <li key={child.id} className="flex items-center gap-2">
                                        <span className="inline-block h-2 w-2 rounded-full bg-accent/90" />
                                        <Link to={`/algorithms/${child.id}`} className="link">
                                            {child.title}
                                        </Link>
                                        {child.subtitle && <span className="text-xs text-textDim">— {child.subtitle}</span>}
                                    </li>
                                ))}
                            </ul>
                            <p className="mt-3 text-xs text-textDim">
                                Этот алгоритм имеет несколько вариантов реализации. Откройте соответствующую страницу, чтобы посмотреть исходный код.
                            </p>
                        </div>
                        <Link to="/algorithms" className="btn">
                            ← Назад к списку
                        </Link>
                    </>
                ) : (
                    <>
                        <div className="flex flex-wrap items-center gap-3 text-sm">
                            <span className="text-textDim">Файл:</span>
                            <code className="rounded bg-surface/40 px-2 py-[2px]">{path}</code>
                            <a href={web} target="_blank" rel="noreferrer" className="link">
                                Открыть на GitHub
                            </a>
                        </div>
                        {node.document && <div className="text-sm text-textDim">Документ: {node.document}</div>}
                        <Link to="/algorithms" className="btn mt-2">
                            ← Назад к списку
                        </Link>
                    </>
                )}
            </div>

            {/* Контент */}
            {hasChildren ? (
                <MarkdownDoc id={node.id} />
            ) : layout === "split" ? (
                <div className="grid gap-4 md:grid-cols-2">
                    {/* В исходнике исторически показывали .tpp в названии — оставим как есть для UX */}
                    <CodeViewer url={raw} filename={`${node.id}.tpp`} />
                    <MarkdownDoc id={node.id} />
                </div>
            ) : (
                <>
                    <MarkdownDoc id={node.id} />
                    <CodeViewer url={raw} filename={`${node.id}.tpp`} />
                </>
            )}
        </div>
    );
};

export default Algorithm;
