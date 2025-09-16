import React from "react";
import { useParams, Link } from "react-router-dom";
import { SERIES } from "../../data/series";
import { findSeriesById } from "./utils/find";
import { buildSeriesDocLinks, buildSeriesSrcLinks } from "./utils/github";
import { FormulaPreview } from "./components/FormulaPreview";
import { CodeViewer } from "../Algorithm/components/CodeViewer.tsx";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import rehypeRaw from "rehype-raw";
import rehypeHighlight from "rehype-highlight";
import rehypeSlug from "rehype-slug";
import rehypeAutolinkHeadings from "rehype-autolink-headings";

const DocFromGithub: React.FC<{ id: string }> = ({ id }) => {
    const { raw, web, path } = React.useMemo(() => buildSeriesDocLinks(id), [id]);
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
            <div className="mb-3 flex flex-wrap items-center justify-between gap-2">
                <div className="text-sm text-textDim">Теория • <code className="px-1">{path}</code></div>
                <a href={web} className="link" target="_blank" rel="noreferrer">Открыть на GitHub</a>
            </div>

            {loading && <div className="text-sm text-textDim">Загрузка теории…</div>}
            {error && <div className="text-sm text-textDim">Документ не найден или недоступен: {error}</div>}

            {!loading && !error && (
                <article className="prose prose-invert max-w-none prose-pre:bg-surface/40 prose-code:text-white prose-a:text-primary prose-a:no-underline hover:prose-a:underline prose-hr:border-border/60">
                    <ReactMarkdown
                        remarkPlugins={[remarkGfm]}
                        rehypePlugins={[rehypeRaw, rehypeSlug, [rehypeAutolinkHeadings, { behavior: "append", properties: { className: ["link"] } }], rehypeHighlight]}
                    >
                        {md}
                    </ReactMarkdown>
                </article>
            )}
        </div>
    );
};

const SeriesPage: React.FC = () => {
    const { id } = useParams<{ id: string }>();
    const node = id ? findSeriesById(SERIES, id) : undefined;

    const [layout, setLayout] = React.useState<"stack" | "split">(
        (localStorage.getItem("series-layout") as "stack" | "split") || "stack"
    );
    React.useEffect(() => {
        localStorage.setItem("series-layout", layout);
    }, [layout]);

    if (!node) {
        return (
            <div className="card">
                <h1 className="mb-2 text-xl font-bold text-red-400">Ряд не найден</h1>
                <Link to="/series" className="link">← Назад к списку рядов</Link>
            </div>
        );
    }

    const { raw, web, path } = buildSeriesSrcLinks(node.id);

    return (
        <div className="space-y-4">
            {/* Header */}
            <div className="card space-y-3">
                <div className="flex flex-wrap items-center justify-between gap-2">
                    <h1 className="text-2xl font-bold text-primary">{node.title}</h1>

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
                                Код слева / Формула справа
                            </button>
                        </div>
                    </div>
                </div>

                {node.subtitle && <p className="text-sm text-textDim">{node.subtitle}</p>}

                <div className="flex flex-wrap items-center gap-3 text-sm">
                    <span className="text-textDim">Исходник:</span>
                    <code className="rounded bg-surface/40 px-2 py-[2px]">{path}</code>
                    <a href={web} target="_blank" rel="noreferrer" className="link">Открыть на GitHub</a>
                </div>

                {node.document && <div className="text-sm text-textDim">Документ: {node.document}</div>}

                <Link to="/series" className="btn mt-2">← Назад к списку</Link>
            </div>

            {/* Content layouts */}
            {layout === "split" ? (
                <div className="grid gap-4 md:grid-cols-2">
                    <CodeViewer url={raw} filename={`${node.id}.hpp`} />
                    {node.formula
                        ? <FormulaPreview latex={node.formula} title="Формула ряда" />
                        : <DocFromGithub id={node.id} />}
                </div>
            ) : (
                <>
                    {node.formula
                        ? <FormulaPreview latex={node.formula} title="Формула ряда" />
                        : <DocFromGithub id={node.id} />}
                    <CodeViewer url={raw} filename={`${node.id}.hpp`} />
                </>
            )}
        </div>
    );
};

export default SeriesPage;
