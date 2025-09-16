import React from "react";
import { Link } from "react-router-dom";
import { SERIES } from "../../data/series";
import { FormulaBadge } from "./components/FormulaBadge";

const SeriesList: React.FC = () => {
    const [query, setQuery] = React.useState("");
    const allIds = React.useMemo(() => SERIES.map(s => s.id), []);

    const filtered = React.useMemo(() => {
        const q = query.trim().toLowerCase();
        if (!q) return SERIES;

        const exact = SERIES.find(row => (row.id ?? "").toLowerCase() === q);
        if (exact) return [exact];

        return SERIES.filter((row) => {
            const hay =
                (row.id ?? "").toLowerCase() + " " +
                (row.title ?? "").toLowerCase() + " " +
                (row.subtitle ?? "").toLowerCase() + " " +
                (row.formula ?? "").toLowerCase();
            return hay.includes(q);
        });
    }, [query]);

    React.useEffect(() => {
        const q = query.trim().toLowerCase();
        if (!q) return;
        const exactId = allIds.find(id => id.toLowerCase() === q);
        if (exactId) {
            const el = document.getElementById(`series-card-${exactId}`);
            if (el) el.scrollIntoView({ behavior: "smooth", block: "center" });
        }
    }, [query, allIds]);

    return (
        <div className="space-y-4">
            {/* header */}
            <div className="mb-4 flex flex-col gap-3 md:flex-row md:items-center md:justify-between">
                <h1 className="text-2xl font-bold text-primary">Ряды</h1>

                <div className="flex gap-2">
                    <input
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                        placeholder="Поиск: geom, zeta, altern…, …"
                        className="w-72 rounded-xl2 border border-border/60 bg-panel/70 px-3 py-2 text-sm outline-none focus:ring-2 focus:ring-primary/40"
                    />
                    <button onClick={() => setQuery("")} className="btn">Сброс</button>
                </div>
            </div>

            {/* list */}
            <div className="grid gap-4 md:grid-cols-2 xl:grid-cols-3">
                {filtered.map((row) => {
                    const href = `/series/${row.id}`;
                    return (
                        <Link
                            key={row.id}
                            id={`series-card-${row.id}`}
                            to={href}
                            className="group block rounded-2xl border border-border/60 bg-panel/60 p-3 transition
                         hover:border-primary/50 hover:bg-panel/70 hover:shadow-[0_6px_24px_rgba(0,0,0,0.25)]"
                        >
                            <div className="mb-2 flex items-center justify-between">
                                <code className="rounded-md bg-surface/40 px-2 py-[2px] text-[11px] text-textDim">
                                    {row.id}
                                </code>
                            </div>

                            <div className="mb-3">
                                <FormulaBadge latex={row.title}/>
                            </div>

                            {row.subtitle && <div className="text-sm text-textDim">{row.subtitle}</div>}

                            <div className="mt-3">
                                <span className="inline-flex items-center gap-2 rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black transition group-hover:translate-x-[2px]">
                                    Открыть
                                </span>
                            </div>
                        </Link>
                    );
                })}
            </div>

            {filtered.length === 0 && (
                <div className="card text-sm text-textDim">
                    Ничего не найдено по запросу «{query}»
                </div>
            )}
        </div>
    );
};

export default SeriesList;
