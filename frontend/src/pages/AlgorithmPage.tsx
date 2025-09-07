import React from "react";
import { useParams, Link } from "react-router-dom";
import { TREE } from "../data/algorithms";
import { resolveAuthors } from "../data/authors";
import type { AlgNode } from "../types/algorithms";

/** Найти узел по id */
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

/** База репозитория и пути (подправь при необходимости) */
const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "feature/structure",
    // относительный путь внутри репо до папки с .tpp:
    folder: "shanks_transformation/include/series_acceleration/algorithm",
};

/** Построить ссылки на raw и обычную страницу GitHub */
function buildLinks(algId: string) {
    const path = `${GH.folder}/${algId}.tpp`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}

/** Просмотрщик исходника */
const CodeViewer: React.FC<{ url: string; filename: string }> = ({ url, filename }) => {
    const [code, setCode] = React.useState<string>("");
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);

    React.useEffect(() => {
        let alive = true;
        setLoading(true);
        setError(null);
        fetch(url, { cache: "no-store" })
            .then(async (r) => {
                if (!r.ok) throw new Error(`${r.status} ${r.statusText}`);
                const txt = await r.text();
                if (alive) setCode(txt);
            })
            .catch((e) => alive && setError(e.message))
            .finally(() => alive && setLoading(false));
        return () => {
            alive = false;
        };
    }, [url]);

    const copy = async () => {
        try {
            await navigator.clipboard.writeText(code);
        } catch {
            /* ignore */
        }
    };

    return (
        <div className="card">
            <div className="mb-2 flex items-center justify-between">
                <div className="text-sm text-textDim">{filename}</div>
                <div className="flex gap-2">
                    <button onClick={copy} className="btn">Copy</button>
                </div>
            </div>

            {loading && <div className="text-textDim text-sm">Загрузка исходника…</div>}
            {error && (
                <div className="text-red-400 text-sm">
                    Не удалось загрузить файл: {error}
                </div>
            )}
            {!loading && !error && (
                <pre className="overflow-auto rounded-xl2 border border-border/60 bg-surface/40 p-4 text-sm leading-relaxed">
          <code>{code}</code>
        </pre>
            )}
        </div>
    );
};

const AlgorithmPage: React.FC = () => {
    const { id } = useParams<{ id: string }>();
    const node = id ? findNodeById(TREE, id) : undefined;

    if (!node) {
        return (
            <div className="card">
                <h1 className="mb-2 text-xl font-bold text-red-400">Алгоритм не найден</h1>
                <Link to="/algorithms" className="link">← Назад к списку алгоритмов</Link>
            </div>
        );
    }

    const authors = resolveAuthors(node.authorIds);
    const { raw, web, path } = buildLinks(node.id);

    return (
        <div className="space-y-4">
            <div className="card space-y-3">
                <h1 className="text-2xl font-bold text-primary">{node.title}</h1>
                {node.subtitle && <p className="text-textDim text-sm">{node.subtitle}</p>}

                {!!authors.length && (
                    <div>
                        <h2 className="font-semibold">Авторы:</h2>
                        <div className="mt-1 flex flex-wrap gap-2">
                            {authors.map((a) =>
                                    a.url ? (
                                        <a key={a.id} href={a.url} target="_blank" rel="noreferrer"
                                           className="rounded-md border border-border/60 bg-surface/40 px-2 py-0.5 text-sm underline hover:text-white">
                                            {a.name}
                                        </a>
                                    ) : (
                                        <span key={a.id}
                                              className="rounded-md border border-border/60 bg-surface/40 px-2 py-0.5 text-sm text-textDim">
                    {a.name}
                  </span>
                                    )
                            )}
                        </div>
                    </div>
                )}

                <div className="flex flex-wrap items-center gap-3 text-sm">
                    <span className="text-textDim">Файл:</span>
                    <code className="rounded bg-surface/40 px-2 py-[2px]">{path}</code>
                    <a href={web} target="_blank" rel="noreferrer" className="link">Открыть на GitHub</a>
                </div>

                {node.document && (
                    <div className="text-sm text-textDim">Документ: {node.document}</div>
                )}

                <Link to="/algorithms" className="btn mt-2">← Назад к списку</Link>
            </div>

            {/* viewer кода из raw.githubusercontent.com */}
            <CodeViewer url={raw} filename={`${node.id}.tpp`} />
        </div>
    );
};

export default AlgorithmPage;
