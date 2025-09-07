import React from "react";
import { useParams, Link } from "react-router-dom";
import { TREE } from "../data/algorithms";
import { resolveAuthors } from "../data/authors";
import type { AlgNode } from "../types/algorithms";

// Markdown
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import rehypeRaw from "rehype-raw";
import rehypeHighlight from "rehype-highlight";

/* ===== поиск в дереве ===== */
function findNodeById(nodes: AlgNode[], id: string): AlgNode | undefined {
    for (const n of nodes) {
        if (n.id === id) return n;
        if (n.children?.length) {
            const hit = findNodeById(n.children, id);
            if (hit) return hit;
        }
    }
    return undefined;
}

/* ===== GitHub-константы ===== */
const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "feature/structure",
    srcFolder: "shanks_transformation/include/series_acceleration/algorithm",
    docsFolder: "docs/algorithm",
};

function buildSrcLinks(algId: string) {
    const path = `${GH.srcFolder}/${algId}.tpp`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}

function buildDocLinks(algId: string) {
    const path = `${GH.docsFolder}/${algId}.md`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}

/* ===== CodeViewer с подсветкой (C++) ===== */
const CodeViewer: React.FC<{ url: string; filename: string }> = ({ url, filename }) => {
    const [code, setCode] = React.useState("");
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);
    const codeRef = React.useRef<HTMLElement | null>(null);

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

    React.useEffect(() => {
        if (!code || !codeRef.current) return;
        let cancelled = false;
        (async () => {
            const hljs = (await import("highlight.js/lib/core")).default;
            const { default: cpp } = await import("highlight.js/lib/languages/cpp");
            if (cancelled) return;
            hljs.registerLanguage("cpp", cpp);
            hljs.highlightElement(codeRef.current!);
        })();
        return () => {
            cancelled = true;
        };
    }, [code]);

    const copy = async () => {
        try {
            await navigator.clipboard.writeText(code);
        } catch {/* ignore */}
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
            {error && <div className="text-red-400 text-sm">Не удалось загрузить файл: {error}</div>}
            {!loading && !error && (
                <pre className="overflow-auto rounded-xl2 border border-border/60 bg-surface/40 p-4 text-sm leading-relaxed">
          <code ref={codeRef} className="language-cpp">{code}</code>
        </pre>
            )}
        </div>
    );
};

/* ===== MarkdownDoc (теория) ===== */
const MarkdownDoc: React.FC<{ id: string }> = ({ id }) => {
    const { raw, web, path } = React.useMemo(() => buildDocLinks(id), [id]);
    const [md, setMd] = React.useState<string>("");
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);

    React.useEffect(() => {
        let alive = true;
        setLoading(true);
        setError(null);
        fetch(raw, { cache: "no-store" })
            .then(async (r) => {
                if (!r.ok) throw new Error(`${r.status} ${r.statusText}`);
                const txt = await r.text();
                if (alive) setMd(txt);
            })
            .catch((e) => alive && setError(e.message))
            .finally(() => alive && setLoading(false));
        return () => {
            alive = false;
        };
    }, [raw]);

    return (
        <div className="card">
            <div className="mb-3 flex items-center justify-between">
                <div className="text-sm text-textDim">Теория • <code className="px-1">{path}</code></div>
                <a href={web} target="_blank" rel="noreferrer" className="link">Открыть на GitHub</a>
            </div>

            {loading && <div className="text-textDim text-sm">Загрузка теории…</div>}
            {error && (
                <div className="text-textDim text-sm">
                    Теоретический документ не найден или недоступен: {error}
                </div>
            )}
            {!loading && !error && (
                <article className="prose prose-invert max-w-none prose-pre:bg-surface/40 prose-code:text-white">
                    <ReactMarkdown
                        remarkPlugins={[remarkGfm]}
                        rehypePlugins={[rehypeRaw, rehypeHighlight]}
                    >
                        {md}
                    </ReactMarkdown>
                </article>
            )}
        </div>
    );
};

/* ===== страница алгоритма ===== */
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
    const hasChildren = !!node.children?.length;

    const implementations = (node.children ?? []).slice().sort((a, b) => {
        const aBase = /базов|(^|_)one(_|$)/i.test((a.subtitle ?? "") + " " + a.id + " " + a.title);
        const bBase = /базов|(^|_)one(_|$)/i.test((b.subtitle ?? "") + " " + b.id + " " + b.title);
        return Number(bBase) - Number(aBase);
    });

    const { raw, web, path } = buildSrcLinks(node.id);

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
                                        {child.subtitle && (
                                            <span className="text-xs text-textDim">— {child.subtitle}</span>
                                        )}
                                    </li>
                                ))}
                            </ul>
                            <p className="mt-3 text-xs text-textDim">
                                Этот алгоритм имеет несколько вариантов реализации. Откройте соответствующую страницу, чтобы посмотреть исходный код.
                            </p>
                        </div>

                        <Link to="/algorithms" className="btn">← Назад к списку</Link>
                    </>
                ) : (
                    <>
                        <div className="flex flex-wrap items-center gap-3 text-sm">
                            <span className="text-textDim">Файл:</span>
                            <code className="rounded bg-surface/40 px-2 py-[2px]">{path}</code>
                            <a href={web} target="_blank" rel="noreferrer" className="link">Открыть на GitHub</a>
                        </div>
                        {node.document && (
                            <div className="text-sm text-textDim">Документ: {node.document}</div>
                        )}
                        <Link to="/algorithms" className="btn mt-2">← Назад к списку</Link>
                    </>
                )}
            </div>

            {/* Теория — для всех (родителей и листьев). Если файла нет, будет сообщение. */}
            <MarkdownDoc id={node.id} />

            {/* Исходник — только для листьев */}
            {!hasChildren && <CodeViewer url={raw} filename={`${node.id}.tpp`} />}
        </div>
    );
};

export default AlgorithmPage;
