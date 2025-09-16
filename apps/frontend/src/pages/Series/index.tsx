import React from "react";
import { Link } from "react-router-dom";
import { SERIES } from "../../data/series";

const SeriesList: React.FC = () => {
    const [q, setQ] = React.useState("");

    const filtered = React.useMemo(() => {
        if (!q.trim()) return SERIES;
        const s = q.toLowerCase();
        return SERIES.filter((row) => {
            return (
                row.id.toLowerCase().includes(s) ||
                row.title.toLowerCase().includes(s) ||
                (row.subtitle?.toLowerCase().includes(s) ?? false)
            );
        });
    }, [q]);

    return (
        <div className="space-y-4">
            {/* header */}
            <div className="mb-2 flex flex-col gap-3 md:flex-row md:items-center md:justify-between">
                <h1 className="text-2xl font-bold text-primary">Ряды</h1>
                <div className="flex gap-2">
                    <input
                        value={q}
                        onChange={(e) => setQ(e.target.value)}
                        placeholder="Поиск: geom, zeta, altern…, …"
                        className="w-72 rounded-xl2 border border-border/60 bg-panel/70 px-3 py-2 text-sm outline-none focus:ring-2 focus:ring-primary/40"
                    />
                    <button onClick={() => setQ("")} className="btn">Сброс</button>
                </div>
            </div>

            {/* list */}
            <div className="grid gap-3 md:grid-cols-2 xl:grid-cols-3">
                {filtered.map((row) => {
                    return (
                        <div key={row.id} className="rounded-xl2 border border-border/60 bg-panel/60 p-3">
                            <div className="mb-1 flex items-center justify-between gap-2">
                                <Link to={`/series/${row.id}`} className="font-semibold text-white hover:underline">
                                    {row.title}
                                </Link>
                                <code className="rounded bg-surface/40 px-2 py-[2px] text-[11px] text-textDim">{row.id}</code>
                            </div>
                            {row.subtitle && <div className="text-sm text-textDim">{row.subtitle}</div>}
                            <div className="mt-3">
                                <Link to={`/series/${row.id}`} className="btn">Открыть</Link>
                            </div>
                        </div>
                    );
                })}
            </div>

            {filtered.length === 0 && (
                <div className="card text-sm text-textDim">Ничего не найдено по запросу «{q}»</div>
            )}
        </div>
    );
};

export default SeriesList;
