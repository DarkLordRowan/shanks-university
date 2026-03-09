import type { Item } from "@/entities/item/model/item";

export type Row = {
    id: string;
    enabled: boolean;
    algo: string;
    m: number | null;
    x: number;
    seriesName: string;
    args: Record<string, unknown>;
};

export type SortKey = "algo" | "m" | "x" | "seriesName";
export type SortState = { key: SortKey; dir: 1 | -1 }[];

export function stableStringify(obj: unknown): string {
    if (obj == null || typeof obj !== "object") return JSON.stringify(obj);
    const entries = Object.entries(obj as Record<string, unknown>)
        .sort(([a], [b]) => a.localeCompare(b))
        .map(([k, v]) => [k, v] as const);
    return `{${entries.map(([k, v]) => `"${k}":${stableStringify(v)}`).join(",")}}`;
}

export function makeKey(it: Item): string {
    const algo = it.algorithm.algorithmName;
    const m = it.algorithm.m;
    const args = it.algorithm.algorithmArgs ?? {};
    return `${algo}|m=${m == null ? "∅" : m}|args=${stableStringify(args)}`;
}

export function buildRowsFromItems(items: Item[]): Row[] {
    const map = new Map<string, Row>();
    for (const it of items) {
        const id = makeKey(it);
        if (!map.has(id)) {
            map.set(id, {
                id,
                enabled: true,
                algo: it.algorithm.algorithmName,
                m: it.algorithm.m,
                x: it.series.x,
                seriesName: it.series.seriesName,
                args: it.algorithm.algorithmArgs ?? {},
            });
        }
    }
    return [...map.values()];
}

export function collectArgColumns(rows: Row[]): string[] {
    const s = new Set<string>();
    for (const r of rows) for (const k of Object.keys(r.args ?? {})) s.add(k);
    return [...s].sort((a, b) => a.localeCompare(b));
}

function getSortableValue(row: Row, key: SortKey): string | number | null {
    switch (key) {
        case "algo":
            return row.algo;
        case "m":
            return row.m;
        case "x":
            return row.x;
        case "seriesName":
            return row.seriesName;
    }
}

export function sortRows(rows: Row[], sort: SortState): Row[] {
    const r = [...rows];
    r.sort((a, b) => {
        for (const s of sort) {
            const ka = getSortableValue(a, s.key);
            const kb = getSortableValue(b, s.key);
            if (ka == null && kb == null) continue;
            if (ka == null) return 1;
            if (kb == null) return -1;
            if (ka < kb) return -1 * s.dir;
            if (ka > kb) return 1 * s.dir;
        }
        return 0;
    });
    return r;
}

export function formatArg(v: unknown): string {
    if (typeof v === "number") return Number.isFinite(v) ? String(v) : "∅";
    if (typeof v === "boolean") return v ? "true" : "false";
    if (v == null) return "∅";
    return String(v);
}
