import type { FilterMode, Group } from "./MatrixAxisFilter";
import type { ArgClause, ArgsOp } from "./MatrixAccelsFilter";

export function normalize(s: string): string {
    return (s ?? "").trim().toLowerCase();
}

export function formatArgs(args: Record<string, unknown> | null | undefined): string {
    if (!args) return "";
    const entries = Object.entries(args).filter(([, v]) => v !== null && v !== undefined);
    if (entries.length === 0) return "";
    return entries.map(([k, v]) => `${k}=${typeof v === "string" ? v : String(v)}`).join(", ");
}

export function groupByKey<T>(
    items: T[],
    getKey: (x: T) => string,
    getTitle: (x: T) => string
): Group<T>[] {
    const map = new Map<string, { title: string; items: T[] }>();
    for (const it of items) {
        const key = getKey(it);
        const title = getTitle(it);
        const cur = map.get(key);
        if (!cur) map.set(key, { title, items: [it] });
        else cur.items.push(it);
    }
    return Array.from(map.entries())
        .map(([key, v]) => ({ key, title: v.title, items: v.items }))
        .sort((a, b) => a.title.localeCompare(b.title));
}

export function applyGroupFilter<T>(
    groups: Group<T>[],
    selectedGroupKeys: Set<string>,
    mode: FilterMode
): T[] {
    if (mode === "whitelist") {
        return groups.filter((g) => selectedGroupKeys.has(g.key)).flatMap((g) => g.items);
    }
    return groups.filter((g) => !selectedGroupKeys.has(g.key)).flatMap((g) => g.items);
}

export function parseNullableNumber(v: string): number | null {
    const t = v.trim();
    if (!t) return null;
    const n = Number(t);
    return Number.isFinite(n) ? n : null;
}

export function applyAccelMFilter<T extends { m: number | null | undefined }>(
    list: T[],
    mMin: number | null,
    mMax: number | null
): T[] {
    if (mMin == null && mMax == null) return list;
    return list.filter((a) => {
        const m = a.m;
        if (m == null) return false;
        if (mMin != null && m < mMin) return false;
        if (mMax != null && m > mMax) return false;
        return true;
    });
}

export function applySeriesPrecisionFilter<T extends { precision?: string | null }>(
    list: T[],
    selectedPrecisions: Set<string>,
    mode: FilterMode
): T[] {
    if (selectedPrecisions.size === 0) return mode === "blacklist" ? list : [];
    if (mode === "whitelist") {
        return list.filter((s) => selectedPrecisions.has(s.precision ?? ""));
    }
    return list.filter((s) => !selectedPrecisions.has(s.precision ?? ""));
}

type ScalarQuery =
    | { kind: "num"; value: number }
    | { kind: "bool"; value: boolean }
    | { kind: "null" }
    | { kind: "str" };

function parseScalarQuery(v: string): ScalarQuery {
    const t = v.trim();
    if (!t) return { kind: "str" };

    const tl = t.toLowerCase();
    if (tl === "true") return { kind: "bool", value: true };
    if (tl === "false") return { kind: "bool", value: false };
    if (tl === "null") return { kind: "null" };

    const n = Number(t);
    if (Number.isFinite(n) && /^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$/.test(t)) {
        return { kind: "num", value: n };
    }

    return { kind: "str" };
}

function valueMatches(argVal: unknown, vqRaw: string): boolean {
    const q = parseScalarQuery(vqRaw);

    if (q.kind === "num") {
        if (typeof argVal === "number") return argVal === q.value;
        if (typeof argVal === "string") return normalize(argVal) === normalize(vqRaw);
        return false;
    }

    if (q.kind === "bool") {
        if (typeof argVal === "boolean") return argVal === q.value;
        if (typeof argVal === "string") return normalize(argVal) === (q.value ? "true" : "false");
        return false;
    }

    if (q.kind === "null") {
        return argVal === null || normalize(String(argVal)) === "null";
    }

    return normalize(String(argVal)).includes(normalize(vqRaw));
}

function clauseMatchesArgs(args: Record<string, unknown> | null, clause: ArgClause): boolean {
    const kq = normalize(clause.key);
    const vq = normalize(clause.value);

    if (!kq && !vq) return true;
    if (!args) return false;

    if (!kq && vq) {
        for (const [, v] of Object.entries(args)) {
            if (v == null) continue;
            if (valueMatches(v, clause.value)) return true;
        }
        return false;
    }

    if (kq) {
        const keys = Object.keys(args);
        const matchedKeys = keys.filter((kk) => normalize(kk).includes(kq));
        if (matchedKeys.length === 0) return false;

        if (!vq) return true;

        for (const kk of matchedKeys) {
            const v = args[kk];
            if (v == null) continue;
            if (valueMatches(v, clause.value)) return true;
        }
        return false;
    }

    return true;
}

export function applyArgsClauses<T extends { args: Record<string, unknown> | null }>(
    list: T[],
    op: ArgsOp,
    clauses: ArgClause[]
): T[] {
    const effective = clauses.filter((c) => normalize(c.key) !== "" || normalize(c.value) !== "");
    if (effective.length === 0) return list;

    return list.filter((x) => {
        const matches = effective.map((c) => clauseMatchesArgs(x.args, c));
        return op === "and" ? matches.every(Boolean) : matches.some(Boolean);
    });
}

export function areSetsEqual<T>(a: Set<T>, b: Set<T>): boolean {
    if (a.size !== b.size) return false;
    for (const v of a) {
        if (!b.has(v)) return false;
    }
    return true;
}

export function accelSearchText(x: {
    name: string;
    m: number | null;
    args: Record<string, unknown> | null;
}): string {
    return normalize([x.name, x.m != null ? String(x.m) : "", formatArgs(x.args)].filter(Boolean).join(" "));
}

export function seriesSearchText(x: {
    name: string;
    precision?: string | null;
    args: Record<string, unknown> | null;
}): string {
    return normalize([x.name, x.precision ?? "", formatArgs(x.args)].filter(Boolean).join(" "));
}
