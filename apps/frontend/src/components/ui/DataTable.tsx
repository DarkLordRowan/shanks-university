import React from "react";
import { Pager } from "./Pager";

function classNames(...s: (string | false | null | undefined)[]) {
    return s.filter(Boolean).join(" ");
}

export const DataTable: React.FC<{
    headers: string[];
    rows: (string | number)[][];
    searchable?: boolean;
    compact?: boolean;
}> = ({ headers, rows, searchable = true, compact = false }) => {
    const [q, setQ] = React.useState("");
    const [page, setPage] = React.useState(1);
    const [pageSize, setPageSize] = React.useState(50);

    const filtered = React.useMemo(() => {
        if (!q) return rows;
        const needle = q.toLowerCase();
        return rows.filter((r) => r.some((v) => String(v).toLowerCase().includes(needle)));
    }, [q, rows]);

    const start = (page - 1) * pageSize;
    const slice = filtered.slice(start, start + pageSize);

    React.useEffect(() => {
        setPage(1);
    }, [q, pageSize]);

    return (
        <div className="flex flex-col gap-3">
            <div className="flex items-center justify-between gap-3">
                {searchable && (
                    <input
                        className="w-64 rounded-md border border-border bg-surface px-3 py-1 text-sm"
                        placeholder="Поиск…"
                        value={q}
                        onChange={(e) => setQ(e.target.value)}
                    />
                )}
                <Pager page={page} setPage={setPage} total={filtered.length} pageSize={pageSize} setPageSize={setPageSize} />
            </div>

            <div className="overflow-auto rounded-xl border border-border/60">
                <table className={classNames("min-w-full table-fixed", compact ? "text-xs" : "text-sm")}>
                    <thead className="sticky top-0 bg-panel/80 backdrop-blur">
                    <tr>
                        {headers.map((h, i) => (
                            <th key={i} className="truncate border-b border-border/60 px-3 py-2 text-left font-semibold">
                                {h}
                            </th>
                        ))}
                    </tr>
                    </thead>
                    <tbody>
                    {slice.map((r, i) => (
                        <tr key={i} className="even:bg-panel/40">
                            {r.map((v, j) => (
                                <td key={j} className="truncate border-t border-border/30 px-3 py-2 align-top">
                                    <span title={String(v)}>{String(v)}</span>
                                </td>
                            ))}
                        </tr>
                    ))}
                    {!slice.length && (
                        <tr>
                            <td className="px-3 py-6 text-center text-textDim" colSpan={headers.length}>
                                Ничего не найдено
                            </td>
                        </tr>
                    )}
                    </tbody>
                </table>
            </div>
        </div>
    );
};
