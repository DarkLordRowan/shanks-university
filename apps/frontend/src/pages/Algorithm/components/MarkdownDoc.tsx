import React from "react";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import rehypeRaw from "rehype-raw";
import rehypeHighlight from "rehype-highlight";
import rehypeSlug from "rehype-slug";
import rehypeAutolinkHeadings from "rehype-autolink-headings";
import { buildDocLinks } from "../utils/github";

type TocItem = { id: string; text: string; level: number };

const isMetaHeading = (txt: string) => /^(id|title|subtitle|authors)\s*:/i.test(txt.trim());

export const MarkdownDoc: React.FC<{ id: string }> = ({ id }) => {
    const { raw, web, path } = React.useMemo(() => buildDocLinks(id), [id]);
    const [md, setMd] = React.useState<string>("");
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);

    const articleRef = React.useRef<HTMLElement | null>(null);
    const [, setToc] = React.useState<TocItem[]>([]); // пока TOC не выводим

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

    // TOC из DOM + фильтр мета-заголовков (на будущее — если захотите отрисовать)
    React.useEffect(() => {
        if (!articleRef.current) return;
        const hEls = Array.from(articleRef.current.querySelectorAll("h1, h2, h3"));
        const items: TocItem[] = hEls
            .map((h) => ({
                id: (h as HTMLElement).id,
                text: h.textContent || "",
                level: Number(h.tagName.slice(1)),
            }))
            .filter((t) => !isMetaHeading(t.text));
        setToc(items);
    }, [md]);

    // скрываем мета-заголовки и параграфы с такой же меткой
    const renderHeading =
        (Tag: "h1" | "h2" | "h3", cls: string) =>
            ({ children, ...props }: any) => {
                const text = String(children).trim();
                if (isMetaHeading(text)) return null;
                return (
                    <Tag className={cls} {...props}>
                        {children}
                    </Tag>
                );
            };

    const renderParagraph = ({ children, ...props }: any) => {
        const text = String(children).trim();
        if (isMetaHeading(text)) return null;
        return <p {...props}>{children}</p>;
    };

    return (
        <div className="card">
            <div className="mb-3 flex flex-wrap items-center justify-between gap-2">
                <div className="text-sm text-textDim">
                    Теория • <code className="px-1">{path}</code>
                </div>
                <a href={web} target="_blank" rel="noreferrer" className="link">
                    Открыть на GitHub
                </a>
            </div>

            {loading && <div className="text-sm text-textDim">Загрузка теории…</div>}
            {error && <div className="text-sm text-textDim">Теоретический документ не найден или недоступен: {error}</div>}

            {!loading && !error && (
                <div className="md:grid md:grid-cols-4 md:gap-6">
                    {/* Markdown */}
                    <article
                        ref={articleRef as any}
                        className="prose prose-invert max-w-none prose-pre:bg-surface/40 prose-code:text-white prose-a:text-primary prose-a:no-underline hover:prose-a:underline prose-hr:border-border/60 md:col-span-3"
                    >
                        <ReactMarkdown
                            remarkPlugins={[remarkGfm]}
                            rehypePlugins={[rehypeRaw, rehypeSlug, [rehypeAutolinkHeadings, { behavior: "append", properties: { className: ["link"] } }], rehypeHighlight]}
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
