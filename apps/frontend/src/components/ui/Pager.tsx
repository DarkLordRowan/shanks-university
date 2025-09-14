import React from "react";

export const Pager: React.FC<{
    page: number;
    setPage: (n: number) => void;
    total: number;
    pageSize: number;
    setPageSize: (n: number) => void;
}> = ({page, setPage, total, pageSize, setPageSize}) => {
    const maxPage = Math.max(1, Math.ceil(total / pageSize));
    const prev = () => setPage(Math.max(1, page - 1));
    const next = () => setPage(Math.min(maxPage, page + 1));

    return (
        <div className="flex flex-wrap items-center gap-3 text-xs">
      <span className="text-textDim">
        Строк: <b>{total}</b>
      </span>
            <div className="flex items-center gap-2">
                <button className="rounded-md border border-border px-2 py-1 disabled:opacity-50" disabled={page === 1}
                        onClick={() => setPage(1)}>
                    «
                </button>
                <button className="rounded-md border border-border px-2 py-1 disabled:opacity-50" disabled={page === 1}
                        onClick={prev}>
                    ‹
                </button>
                <span>
          Стр. <b>{page}</b> / {maxPage}
        </span>
                <button className="rounded-md border border-border px-2 py-1 disabled:opacity-50"
                        disabled={page === maxPage} onClick={next}>
                    ›
                </button>
                <button className="rounded-md border border-border px-2 py-1 disabled:opacity-50"
                        disabled={page === maxPage} onClick={() => setPage(maxPage)}>
                    »
                </button>
            </div>
            <select
                className="rounded-md border border-border bg-background px-2 py-1"
                value={pageSize}
                onChange={(e) => setPageSize(Number(e.target.value))}
            >
                {[25, 50, 100, 250, 1000].map((n) => (
                    <option key={n} value={n}>
                        {n}/стр
                    </option>
                ))}
            </select>
        </div>
    );
};
