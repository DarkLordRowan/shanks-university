type Scalar = number | string | boolean | null;

export function nonNullEntries<T extends Record<string, unknown>>(obj: T | null | undefined) {
    if (!obj) return [] as [string, unknown][];
    return Object.entries(obj).filter(([, v]) => v !== null && v !== undefined);
}

function toSortableNumber(v: Scalar): number | null {
    if (typeof v === "number") return Number.isFinite(v) ? v : null;
    if (typeof v === "string") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    return null;
}

export function parseSeriesXArg(args: { x?: unknown } | null | undefined): { xLabel: string; xSort: number | null } {
    const raw = args?.x as Scalar | undefined;
    if (raw == null) return { xLabel: "\u2205", xSort: null };
    return {
        xLabel: String(raw),
        xSort: toSortableNumber(raw),
    };
}

export function buildMatrixArgsSummary(args: Record<string, unknown> | null | undefined): string {
    const entries = nonNullEntries(args);
    if (entries.length === 0) return "";
    entries.sort(([a], [b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${String(v)}`).join(", ");
}
