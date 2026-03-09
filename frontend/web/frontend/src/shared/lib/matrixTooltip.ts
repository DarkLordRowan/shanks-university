import { nonNullEntries } from "@/shared/lib/matrixArgs";

export function appendAlgorithmArgsTooltipLines(
    titleLines: string[],
    algorithmArgs: Record<string, unknown> | null | undefined,
    argsSummary: string | null | undefined
): boolean {
    const entries = nonNullEntries(algorithmArgs);
    entries.sort(([a], [b]) => a.localeCompare(b));

    for (const [k, v] of entries) {
        titleLines.push(`  ${k} = ${String(v)}`);
    }

    if (argsSummary) {
        titleLines.push(`  (${argsSummary})`);
    }

    return entries.length > 0 || Boolean(argsSummary);
}
