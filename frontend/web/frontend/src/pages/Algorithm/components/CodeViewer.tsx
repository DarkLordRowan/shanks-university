import React from "react";

export const CodeViewer: React.FC<{ url: string; filename: string }> = ({ url, filename }) => {
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
        return () => {
            alive = false;
        };
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
        return () => {
            cancelled = true;
        };
    }, [code]);

    const copy = async () => {
        try {
            await navigator.clipboard.writeText(code);
        } catch {
            /* noop */
        }
    };

    const total = hlLines.length || code.split("\n").length;

    return (
        <div className="card">
            <div className="mb-2 flex flex-wrap items-center justify-between gap-2">
                <div className="text-sm text-textDim">{filename}</div>
                <div className="flex gap-2">
                    <button onClick={copy} className="btn">
                        Copy
                    </button>
                </div>
            </div>

            {loading && <div className="text-sm text-textDim">Загрузка исходника…</div>}
            {error && <div className="text-sm text-red-400">Не удалось загрузить файл: {error}</div>}

            {!loading && !error && (
                <div className="overflow-auto rounded-xl2 border border-border/60 bg-surface/40">
                    <div className="grid grid-cols-[auto,1fr]">
                        <ol
                            className="m-0 select-none border-r border-border/60 bg-surface/30 px-3 py-4 text-right text-sm font-mono leading-[1.5]"
                            style={{ minWidth: 0 }}
                        >
                            {Array.from({ length: total }).map((_, i) => (
                                <li key={i} className="tabular-nums">
                                    {i + 1}
                                </li>
                            ))}
                        </ol>

                        {/* Каждая строка кода — отдельный div, чтобы совпал line-height */}
                        <div className="m-0 overflow-auto whitespace-pre p-4 text-sm font-mono leading-[1.5]">
                            {hlLines.map((line, i) => (
                                <div key={i} className="min-h-[1em]" dangerouslySetInnerHTML={{ __html: line.length ? line : " " }} />
                            ))}
                        </div>
                    </div>
                </div>
            )}
        </div>
    );
};
