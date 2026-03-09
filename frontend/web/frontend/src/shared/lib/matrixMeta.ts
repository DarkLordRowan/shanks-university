import type { Accel, AccelArgs, Series, SeriesArgs } from "@/entities/experiment/model/experiment";

type ParseX = (args: SeriesArgs | null) => { xLabel: string; xSort: number | null };
type BuildArgsSummary = (args: AccelArgs | null) => string;

export interface MatrixSeriesInfo {
    key: string;
    seriesName: string;
    xLabel: string;
    xSort: number | null;
    precision: string;
}

export interface MatrixAlgoInfo {
    key: string;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: AccelArgs | null;
}

export function buildMatrixSeriesInfos(
    seriesListRaw: Series[],
    usedSeriesIds: Set<string>,
    parseX: ParseX
): MatrixSeriesInfo[] {
    return seriesListRaw
        .filter((s) => usedSeriesIds.has(s.id))
        .map((s) => {
            const { xLabel, xSort } = parseX(s.args ?? null);
            return {
                key: s.id,
                seriesName: s.name,
                xLabel,
                xSort,
                precision: s.precision,
            };
        })
        .sort((a, b) => {
            const byName = a.seriesName.localeCompare(b.seriesName);
            if (byName !== 0) return byName;

            const ax = a.xSort;
            const bx = b.xSort;
            if (ax != null && bx != null) return ax - bx;
            if (ax != null) return -1;
            if (bx != null) return 1;
            return a.xLabel.localeCompare(b.xLabel);
        });
}

export function buildMatrixAlgoInfos(
    accelListRaw: Accel[],
    usedAccelIds: Set<string>,
    buildArgsSummary: BuildArgsSummary
): MatrixAlgoInfo[] {
    return accelListRaw
        .filter((a) => usedAccelIds.has(a.id))
        .map((a) => {
            const args = a.args ?? null;
            return {
                key: a.id,
                algorithmName: a.name,
                m: a.m,
                argsSummary: buildArgsSummary(args),
                algorithmArgs: args,
            };
        })
        .sort((a, b) => {
            const byName = a.algorithmName.localeCompare(b.algorithmName);
            if (byName !== 0) return byName;
            return (a.m ?? 0) - (b.m ?? 0);
        });
}
