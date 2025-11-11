
import React, { useEffect, useMemo, useState } from "react";
import { Pager } from "./Pager";

function classNames(...s: (string | false | null | undefined)[]) {
  return s.filter(Boolean).join(" ");
}

type ColumnType = "text" | "number";

type NumericFilter = { min?: number | ""; max?: number | "" };
type TextFilter = string;

export const DataTable: React.FC<{
  headers: string[];
  rows: (string | number)[][];
  searchable?: boolean;
  compact?: boolean;
  /** enable click-to-sort on headers */
  enableSorting?: boolean;
  /** show a filter row under the headers */
  enableColumnFilters?: boolean;
  /** persist UI state (filters, sort, page, pageSize, search) to localStorage */
  storageKey?: string;
  /** optionally specify column types; if omitted, types will be inferred */
  columnTypes?: ColumnType[];
}> = ({
  headers,
  rows,
  searchable = true,
  compact = false,
  enableSorting = true,
  enableColumnFilters = true,
  storageKey,
  columnTypes,
}) => {
  // ---------- Types ----------
  const inferredTypes: ColumnType[] = useMemo(() => {
    if (columnTypes && columnTypes.length === headers.length) return columnTypes;
    // infer: if 90%+ of non-empty cells are numeric -> "number"
    return headers.map((_, colIdx) => {
      let total = 0;
      let numeric = 0;
      for (let r = 0; r < rows.length; r++) {
        const v = rows[r]?.[colIdx];
        if (v === null || v === undefined || v === "") continue;
        total++;
        const n = typeof v === "number" ? v : Number(String(v).replace(",", "."));
        if (Number.isFinite(n)) numeric++;
      }
      if (total === 0) return "text";
      return numeric / total >= 0.9 ? "number" : "text";
    });
  }, [headers, rows, columnTypes]);

  // ---------- State ----------
  const [q, setQ] = useState<string>("");
  const [page, setPage] = useState<number>(1);
  const [pageSize, setPageSize] = useState<number>(50);
  const [sortIndex, setSortIndex] = useState<number | null>(null);
  const [sortDir, setSortDir] = useState<"asc" | "desc">("asc");
  const [filters, setFilters] = useState<(NumericFilter | TextFilter)[]>(
    headers.map((_h, i) => (inferredTypes[i] === "number" ? { min: "", max: "" } : ""))
  );

  // ---------- Persistence ----------
  useEffect(() => {
    if (!storageKey) return;
    try {
      const raw = localStorage.getItem(storageKey);
      if (!raw) return;
      const parsed = JSON.parse(raw);
      if (parsed) {
        setQ(parsed.q ?? "");
        setPage(parsed.page ?? 1);
        setPageSize(parsed.pageSize ?? 50);
        setSortIndex(parsed.sortIndex ?? null);
        setSortDir(parsed.sortDir ?? "asc");
        if (Array.isArray(parsed.filters) && parsed.filters.length === headers.length) {
          setFilters(parsed.filters);
        }
      }
    } catch {}
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [storageKey]);

  useEffect(() => {
    if (!storageKey) return;
    const state = JSON.stringify({ q, page, pageSize, sortIndex, sortDir, filters });
    try {
      localStorage.setItem(storageKey, state);
    } catch {}
  }, [q, page, pageSize, sortIndex, sortDir, filters, storageKey]);

  // Reset to first page when data or filters change
  useEffect(() => {
    setPage(1);
  }, [q, rows, filters, sortIndex, sortDir]);

  // ---------- Filtering ----------
  const filtered = useMemo(() => {
    const qLower = q.trim().toLowerCase();
    return rows.filter((row) => {
      // global search
      if (qLower) {
        const joined = row.map((c) => String(c)).join(" ").toLowerCase();
        if (!joined.includes(qLower)) return false;
      }
      // column filters
      if (!enableColumnFilters) return true;
      for (let i = 0; i < headers.length; i++) {
        const v = row[i];
        const t = inferredTypes[i];
        const f = filters[i];
        if (t === "number") {
          const { min, max } = (f as NumericFilter) ?? {};
          const n = typeof v === "number" ? v : Number(String(v).replace(",", "."));
          if (min !== "" && Number.isFinite(Number(min)) && !(n >= Number(min))) return false;
          if (max !== "" && Number.isFinite(Number(max)) && !(n <= Number(max))) return false;
        } else {
          const s = (f as TextFilter).trim().toLowerCase();
          if (s && !String(v ?? "").toLowerCase().includes(s)) return false;
        }
      }
      return true;
    });
  }, [rows, q, filters, inferredTypes, headers.length, enableColumnFilters]);

  // ---------- Sorting ----------
  const sorted = useMemo(() => {
    if (sortIndex === null) return filtered;
    const idx = sortIndex;
    const dirFactor = sortDir === "asc" ? 1 : -1;
    const isNum = inferredTypes[idx] === "number";
    const clone = [...filtered];
    clone.sort((a, b) => {
      const va = a[idx];
      const vb = b[idx];
      if (isNum) {
        const na = typeof va === "number" ? va : Number(String(va).replace(",", "."));
        const nb = typeof vb === "number" ? vb : Number(String(vb).replace(",", "."));
        const sa = Number.isFinite(na) ? na : Number.NEGATIVE_INFINITY;
        const sb = Number.isFinite(nb) ? nb : Number.NEGATIVE_INFINITY;
        return sa === sb ? 0 : sa > sb ? dirFactor : -dirFactor;
      }
      const sa = String(va ?? "");
      const sb = String(vb ?? "");
      return sa.localeCompare(sb) * dirFactor;
    });
    return clone;
  }, [filtered, sortIndex, sortDir, inferredTypes]);

  // ---------- Pagination ----------
  const start = (page - 1) * pageSize;
  const end = start + pageSize;
  const slice = sorted.slice(start, end);

  // ---------- UI helpers ----------
  const toggleSort = (i: number) => {
    if (!enableSorting) return;
    if (sortIndex === i) {
      setSortDir(sortDir === "asc" ? "desc" : "asc");
    } else {
      setSortIndex(i);
      setSortDir("asc");
    }
  };

  const resetFilters = () => {
    setFilters(headers.map((_h, i) => (inferredTypes[i] === "number" ? { min: "", max: "" } : "")));
    setQ("");
    setSortIndex(null);
    setSortDir("asc");
  };

  return (
    <div className="space-y-3">
      <div className="flex flex-wrap items-center justify-between gap-3">
        <div className="flex items-center gap-2">
          {searchable && (
            <input
              className="w-64 rounded-md border border-border bg-surface px-3 py-1 text-sm"
              placeholder="Поиск…"
              value={q}
              onChange={(e) => setQ(e.target.value)}
            />
          )}
          {enableColumnFilters && (
            <button
              onClick={resetFilters}
              className="rounded-md border border-border bg-panel px-3 py-1 text-xs text-textDim hover:bg-panel/80"
            >
              Сбросить фильтры
            </button>
          )}
        </div>
        <Pager page={page} setPage={setPage} total={filtered.length} pageSize={pageSize} setPageSize={setPageSize} />
      </div>

      <div className="overflow-auto rounded-xl border border-border/60">
        <table className={classNames("min-w-full table-fixed", compact ? "text-xs" : "text-sm")}>
          <thead className="sticky top-0 bg-panel/80 backdrop-blur">
            <tr>
              {headers.map((h, i) => (
                <th
                  key={i}
                  className={classNames(
                    "truncate border-b border-border/60 px-3 py-2 text-left font-medium",
                    enableSorting && "cursor-pointer select-none hover:bg-panel/60"
                  )}
                  onClick={() => toggleSort(i)}
                  title={enableSorting ? "Сортировать" : undefined}
                >
                  <div className="flex items-center gap-1">
                    <span className="truncate">{h}</span>
                    {enableSorting && sortIndex === i && (
                      <span className="text-textDim">{sortDir === "asc" ? "▲" : "▼"}</span>
                    )}
                  </div>
                </th>
              ))}
            </tr>
            {enableColumnFilters && (
              <tr className="bg-panel/60">
                {headers.map((_, i) => {
                  const t = inferredTypes[i];
                  return (
                    <th key={i} className="border-b border-border/60 px-3 py-1 text-left font-normal">
                      {t === "number" ? (
                        <div className="flex items-center gap-2">
                          <input
                            className="w-24 rounded-md border border-border bg-background/60 px-2 py-1 text-xs"
                            placeholder="мин"
                            value={(filters[i] as NumericFilter).min ?? ""}
                            onClick={(e) => e.stopPropagation()}
                            onChange={(e) => {
                              const v = e.target.value;
                              setFilters((prev) => {
                                const copy = [...prev];
                                copy[i] = { ...(copy[i] as NumericFilter), min: v === "" ? "" : Number(v) };
                                return copy;
                              });
                            }}
                          />
                          <input
                            className="w-24 rounded-md border border-border bg-background/60 px-2 py-1 text-xs"
                            placeholder="макс"
                            value={(filters[i] as NumericFilter).max ?? ""}
                            onClick={(e) => e.stopPropagation()}
                            onChange={(e) => {
                              const v = e.target.value;
                              setFilters((prev) => {
                                const copy = [...prev];
                                copy[i] = { ...(copy[i] as NumericFilter), max: v === "" ? "" : Number(v) };
                                return copy;
                              });
                            }}
                          />
                        </div>
                      ) : (
                        <input
                          className="w-full rounded-md border border-border bg-background/60 px-2 py-1 text-xs"
                          placeholder="фильтр…"
                          value={(filters[i] as TextFilter) as string}
                          onClick={(e) => e.stopPropagation()}
                          onChange={(e) => {
                            const v = e.target.value;
                            setFilters((prev) => {
                              const copy = [...prev];
                              copy[i] = v;
                              return copy;
                            });
                          }}
                        />
                      )}
                    </th>
                  );
                })}
              </tr>
            )}
          </thead>
          <tbody>
            {slice.map((row, rIdx) => (
              <tr key={rIdx} className={rIdx % 2 ? "bg-panel/20" : ""}>
                {row.map((c, cIdx) => (
                  <td key={cIdx} className="truncate border-b border-border/60 px-3 py-2 align-top">
                    {String(c)}
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
