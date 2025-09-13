import React from "react";
import { useParams, Link } from "react-router-dom";
import { TREE } from "../data/algorithms";
import { resolveAuthors } from "../data/authors";
import type { AlgNode } from "../types/algorithms";

import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import rehypeRaw from "rehype-raw";
import rehypeHighlight from "rehype-highlight";
import rehypeSlug from "rehype-slug";
import rehypeAutolinkHeadings from "rehype-autolink-headings";

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

/* ===== GitHub-пути ===== */
const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "Fixes",
    srcFolder: "shanks_transformation/methods/algorithm", // .tpp
    docsFolder: "theory/algorithm", // .md
};

function buildSrcLinks(algId: string) {
    const path = `${GH.srcFolder}/${algId}.hpp`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}

function buildDocLinks(algId: string) {
    const path = `${GH.docsFolder}/${algId}.tex`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}

/* ===== CodeViewer (подсветка C++ + ровная нумерация строк) =====
   Фикс: подсвечиваем код через hljs.highlight(...) => HTML, затем
   делим по \n и показываем каждую строку отдельным <div>. Так
   line-height и шрифты синхронизированы с левой «линейкой». */
const CodeViewer: React.FC<{ url: string; filename: string }> = ({ url, filename }) => {
    const [code, setCode] = React.useState("");
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);
    const [hlLines, setHlLines] = React.useState<string[]>([]);

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
        return () => { alive = false; };
    }, [url]);

    React.useEffect(() => {
        if (!code) return;
        let cancelled = false;
        (async () => {
            const hljs = (await import("highlight.js/lib/core")).default;
            const { default: cpp } = await import("highlight.js/lib/languages/cpp");
            hljs.registerLanguage("cpp", cpp);
            const html = hljs.highlight(code, { language: "cpp", ignoreIllegals: true }).value;
            if (!cancelled) setHlLines(html.split("\n"));
        })();
        return () => { cancelled = true; };
    }, [code]);

    const copy = async () => {
        try { await navigator.clipboard.writeText(code); } catch { /* empty */ }
    };

    const total = hlLines.length || code.split("\n").length;

    return (
        <div className="card">
            <div className="mb-2 flex flex-wrap items-center justify-between gap-2">
                <div className="text-sm text-textDim">{filename}</div>
                <div className="flex gap-2">
                    <button onClick={copy} className="btn">Copy</button>
                </div>
            </div>

            {loading && <div className="text-textDim text-sm">Загрузка исходника…</div>}
            {error && <div className="text-red-400 text-sm">Не удалось загрузить файл: {error}</div>}

            {!loading && !error && (
                <div className="overflow-auto rounded-xl2 border border-border/60 bg-surface/40">
                    <div className="grid grid-cols-[auto,1fr]">
                        <ol
                            className="m-0 select-none border-r border-border/60 bg-surface/30 px-3 py-4 text-right text-sm font-mono leading-[1.5]"
                            style={{ minWidth: 0 }}
                        >
                            {Array.from({ length: total }).map((_, i) => (
                                <li key={i} className="tabular-nums">{i + 1}</li>
                            ))}
                        </ol>

                        {/* код: одна строка = один блок, чтобы line-height совпадал с <ol> */}
                        <div className="m-0 overflow-auto p-4 text-sm font-mono leading-[1.5] whitespace-pre">
                            {hlLines.map((line, i) => (
                                <div
                                    key={i}
                                    className="min-h-[1em]"
                                    dangerouslySetInnerHTML={{ __html: line.length ? line : " " }}
                                />
                            ))}
                        </div>
                    </div>
                </div>
            )}
        </div>
    );
};

/* ===== MarkdownDoc (теория) + TOC ===== */
type TocItem = { id: string; text: string; level: number };

// фильтр «служебных» заголовков
const isMetaHeading = (txt: string) =>
    /^(id|title|subtitle|authors)\s*:/i.test(txt.trim());

const MarkdownDoc: React.FC<{ id: string }> = ({ id }) => {
    const { raw, web, path } = React.useMemo(() => buildDocLinks(id), [id]);
    const [md, setMd] = React.useState<string>("");
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);

    const articleRef = React.useRef<HTMLElement | null>(null);
    const [, setToc] = React.useState<TocItem[]>([]);

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
        return () => { alive = false; };
    }, [raw]);

    // TOC из DOM + фильтр мета-заголовков
    React.useEffect(() => {
        if (!articleRef.current) return;
        const hEls = Array.from(articleRef.current.querySelectorAll("h1, h2, h3"));
        const items: TocItem[] = hEls.map((h) => ({
            id: (h as HTMLElement).id,
            text: h.textContent || "",
            level: Number(h.tagName.slice(1)),
        })).filter(t => !isMetaHeading(t.text));
        setToc(items);
    }, [md]);

    // кастомные рендеры заголовков/параграфов: скрываем «мету» и приглушаем размеры
    const renderHeading =
        (Tag: "h1" | "h2" | "h3", cls: string) =>
            ({ children, ...props }: any) => {
                const text = String(children).trim();
                if (isMetaHeading(text)) return null;
                return <Tag className={cls} {...props}>{children}</Tag>;
            };

    const renderParagraph = ({ children, ...props }: any) => {
        const text = String(children).trim();
        if (isMetaHeading(text)) return null;
        return <p {...props}>{children}</p>;
    };

    return (
        <div className="card">
            <div className="mb-3 flex flex-wrap items-center justify-between gap-2">
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
                <div className="md:grid md:grid-cols-4 md:gap-6">
                    {/* Markdown */}
                    <article
                        ref={articleRef as any}
                        className="prose prose-invert max-w-none prose-pre:bg-surface/40 prose-code:text-white prose-a:text-primary prose-a:no-underline hover:prose-a:underline prose-hr:border-border/60 md:col-span-3"
                    >
                        <ReactMarkdown
                            remarkPlugins={[remarkGfm]}
                            rehypePlugins={[
                                rehypeRaw,
                                rehypeSlug,
                                [rehypeAutolinkHeadings, { behavior: "append", properties: { className: ["link"] } }],
                                rehypeHighlight,
                            ]}
                            components={{
                                h1: renderHeading("h1", "text-2xl"),
                                h2: renderHeading("h2", "text-xl"),
                                h3: renderHeading("h3", "text-lg"),
                                p: renderParagraph,
                            }}
                        >
                            {md}
                        </ReactMarkdown>
                    </article>
                </div>
            )}
        </div>
    );
};

/* ===== страница алгоритма ===== */
const AlgorithmPage: React.FC = () => {
    const { id } = useParams<{ id: string }>();
    const node = id ? findNodeById(TREE, id) : undefined;

    const [layout, setLayout] = React.useState<"stack" | "split">(
        (localStorage.getItem("alg-layout") as "stack" | "split") || "stack"
    );
    React.useEffect(() => { localStorage.setItem("alg-layout", layout); }, [layout]);

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
            {/* Шапка */}
            <div className="card space-y-3">
                <div className="flex flex-wrap items-center justify-between gap-2">
                    <h1 className="text-2xl font-bold text-primary">{node.title}</h1>

                    {/* Переключатель раскладки (активен только для листьев) */}
                    {!hasChildren && (
                        <div className="flex items-center gap-2">
                            <span className="text-sm text-textDim">Вид:</span>
                            <div className="flex overflow-hidden rounded-xl2 border border-border/60">
                                <button
                                    onClick={() => setLayout("stack")}
                                    className={`px-3 py-1 text-sm ${layout === "stack" ? "bg-accent/90 text-black" : "bg-panel/70 text-textDim hover:text-white"}`}
                                >
                                    В столбик
                                </button>
                                <button
                                    onClick={() => setLayout("split")}
                                    className={`px-3 py-1 text-sm ${layout === "split" ? "bg-accent/90 text-black" : "bg-panel/70 text-textDim hover:text-white"}`}
                                >
                                    Код слева / Теория справа
                                </button>
                            </div>
                        </div>
                    )}
                </div>

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
                                        <Link to={`/algorithms/${child.id}`} className="link">{child.title}</Link>
                                        {child.subtitle && <span className="text-xs text-textDim">— {child.subtitle}</span>}
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
                        {node.document && <div className="text-sm text-textDim">Документ: {node.document}</div>}
                        <Link to="/algorithms" className="btn mt-2">← Назад к списку</Link>
                    </>
                )}
            </div>

            {/* Раскладка: для листьев можно «в столбик» или «сплит» */}
            {hasChildren ? (
                <MarkdownDoc id={node.id} />
            ) : layout === "split" ? (
                <div className="grid gap-4 md:grid-cols-2">
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

export default AlgorithmPage;
