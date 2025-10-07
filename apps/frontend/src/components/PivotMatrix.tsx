import React from "react";
import type { Item } from "../types/item.ts";
import type { PivotByX } from "../types/pivotByX.ts";

type Props = { pivot: PivotByX };

function pickItem(pivot: PivotByX, seriesName: string, algorithmName: string): Item | undefined {
    let best: Item | undefined;
    for (const it of pivot.objects) {
        if (it.seriesName === seriesName && it.algorithmName === algorithmName) {
            if (!best) best = it;
            else {
                const a = it.m ?? Number.POSITIVE_INFINITY;
                const b = best.m ?? Number.POSITIVE_INFINITY;
                if (a < b) best = it;
            }
        }
    }
    return best;
}

function fmtLim(v: number | null): string {
    if (v === null || !Number.isFinite(v)) return "—";
    return Number(v).toPrecision(4);
}

export const PivotMatrix: React.FC<Props> = ({pivot}) => {
    const cols = pivot.allAlgorithms;
    const rows = pivot.allSeries;

    return (
        <div className="overflow-auto rounded-xl2 border border-border shadow-panel bg-panel">
            <table className="w-full border-collapse">
                <thead className="bg-surface/60 backdrop-blur">
                <tr className="text-textDim text-sm">
                    <th className="sticky left-0 z-10 px-3 py-2 text-left bg-surface/60">
                        x = {Number.isNaN(pivot.x) ? "NaN" : String(pivot.x)}
                    </th>
                    {cols.map((alg) => (
                        <th key={alg} className="px-3 py-2 text-center border-l border-border">
                            {alg}
                        </th>
                    ))}
                </tr>
                </thead>
                <tbody className="text-sm">
                {rows.map((series) => (
                    <tr key={series} className="odd:bg-surface/30">
                        <th className="sticky left-0 z-10 bg-panel px-3 py-2 text-left font-medium text-textDim border-t border-border">
                            {series}
                        </th>
                        {cols.map((alg) => {
                            const it = pickItem(pivot, series, alg);
                            if (!it) {
                                return (
                                    <td
                                        key={alg}
                                        className="px-3 py-2 text-center border-t border-l border-border text-textDim/60"
                                    >
                                        —
                                    </td>
                                );
                            }
                            const hasError = it.error != null;
                            const nStep =
                                hasError && typeof it.error?.data?.n === "number"
                                    ? it.error!.data!.n
                                    : null;

                            return (
                                <td
                                    key={alg}
                                    title={hasError ? it.error!.description : ""}
                                    className={[
                                        "px-3 py-2 text-center border-t border-l border-border font-semibold tabular-nums",
                                        hasError
                                            ? "bg-red-900/30 text-red-300"
                                            : "bg-emerald-900/20 text-emerald-300",
                                    ].join(" ")}
                                >
                                    {hasError ? (nStep ?? "error") : fmtLim(it.seriesLim)}
                                </td>
                            );
                        })}
                    </tr>
                ))}
                </tbody>
            </table>
        </div>
    );
};
