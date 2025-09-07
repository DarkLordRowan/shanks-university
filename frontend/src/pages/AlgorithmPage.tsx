import React from "react";
import { useParams, Link } from "react-router-dom";
import { TREE } from "../data/algorithms";
import { resolveAuthors } from "../data/authors";
import type { AlgNode } from "../types/algorithms";

/* рекурсивный поиск по дереву */
function findNodeById(nodes: AlgNode[], id: string): AlgNode | undefined {
    for (const n of nodes) {
        if (n.id === id) return n;
        if (n.children) {
            const child = findNodeById(n.children, id);
            if (child) return child;
        }
    }
    return undefined;
}

const AlgorithmPage: React.FC = () => {
    const { id } = useParams<{ id: string }>();
    const node = id ? findNodeById(TREE, id) : undefined;

    if (!node) {
        return (
            <div className="card">
                <h1 className="mb-2 text-xl font-bold text-red-400">
                    Алгоритм не найден
                </h1>
                <Link to="/algorithms" className="link">
                    ← Назад к списку алгоритмов
                </Link>
            </div>
        );
    }

    const authors = resolveAuthors(node.authorIds);

    return (
        <div className="card space-y-3">
            <h1 className="text-2xl font-bold text-primary">{node.title}</h1>
            {node.subtitle && (
                <p className="text-textDim text-sm">{node.subtitle}</p>
            )}

            {!!authors.length && (
                <div>
                    <h2 className="font-semibold">Авторы:</h2>
                    <div className="flex flex-wrap gap-2 mt-1">
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

            {node.document && (
                <div>
                    <h2 className="font-semibold">Документ:</h2>
                    <p className="text-sm text-textDim">{node.document}</p>
                </div>
            )}

            {node.url && (
                <div>
                    <h2 className="font-semibold">Ссылка:</h2>
                    <a
                        href={node.url}
                        target="_blank"
                        rel="noreferrer"
                        className="link"
                    >
                        {node.url}
                    </a>
                </div>
            )}

            <Link to="/algorithms" className="btn mt-4">
                ← Назад к списку
            </Link>
        </div>
    );
};

export default AlgorithmPage;
