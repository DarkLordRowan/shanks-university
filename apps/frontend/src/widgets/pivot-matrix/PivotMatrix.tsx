// PivotMatrix.tsx
// Компактная матрица с укороченными названиями рядов и подсказками (Tailwind).

import type { Item } from "@/entities/item/model/item";
import type { PivotByX } from "@/entities/pivot/model/pivotByX";
import React from "react";

type Props = { pivot: PivotByX; seriesTrim?: number };

/** Берём Item с минимальным m для (series, algorithm). */
function pickItem(p: PivotByX, s: string, a: string): Item | undefined {
    let best: Item | undefined;
    for (const it of p.objects) {
        if (it.series.seriesName === s && it.algorithm.algorithmName === a) {
            if (!best) best = it;
            else {
                const am = it.algorithm.m ?? Number.POSITIVE_INFINITY;
                const bm = best.algorithm.m ?? Number.POSITIVE_INFINITY;
                if (am < bm) best = it;
            }
        }
    }
    return best;
}

function fmtLim(v: number | null): string {
    if (v === null || !Number.isFinite(v)) return "—";
    return Number(v).toPrecision(4);
}

function shortAlg(name: string): string {
    return name.endsWith("Algorithm") ? name.slice(0, -9) : name;
}

export const PivotMatrix: React.FC<Props> = ({ pivot, seriesTrim = 100 }) => {
    const algs = pivot.allAlgorithms;
    const rows = pivot.allSeries;
    const { ref, cw } = useAutoColWidth(algs.length, {
        rowHdrW: 120,
        minW: 60,
        maxW: 120,
    });

    return (
        <div
            ref={ref}
            className="overflow-auto rounded-xl2 border border-border shadow-panel bg-panel text-[11px] leading-tight"
        >
            <table className="w-full border-collapse">
                <thead className="bg-surface/60 backdrop-blur text-textDim">
                    <tr>
                        <th className="sticky left-0 z-10 px-2 py-1 text-left bg-surface/60"></th>
                        {algs.map((alg) => (
                            <th
                                key={alg}
                                title={alg}
                                className="align-bottom border-l border-border"
                                style={{ width: cw, minWidth: cw, maxWidth: cw }}
                            >
                                <div
                                    className="mx-auto h-6 flex items-start justify-center px-1 text-center text-textDim"
                                    style={{
                                        textOrientation: "mixed",
                                        transform: "rotate(0deg)",
                                    }}
                                >
                                    <span className="truncate">{shortAlg(alg)}</span>
                                </div>
                            </th>
                        ))}
                    </tr>
                </thead>

                <tbody>
                    {rows.map((series) => {
                        const shortLabel =
                            series.length > seriesTrim ? series.slice(0, seriesTrim) + "…" : series;
                        return (
                            <tr key={series} className="odd:bg-surface/30">
                                <th
                                    className="sticky left-0 z-10 bg-panel px-2 py-1 text-left font-medium text-textDim border-t border-border truncate max-w-[120px]"
                                    title={series}
                                >
                                    {shortLabel}
                                </th>

                                {algs.map((alg) => {
                                    const it = pickItem(pivot, series, alg);
                                    if (!it)
                                        return (
                                            <td
                                                key={alg}
                                                className="px-1 py-1 text-center border-t border-l border-border text-textDim/50"
                                                style={{ width: cw }}
                                            >
                                                —
                                            </td>
                                        );

                                    const hasError = it.error != null;
                                    const nStep =
                                        hasError && typeof it.error?.data?.n === "number"
                                            ? it.error!.data!.n
                                            : null;

                                    return (
                                        <td
                                            key={alg}
                                            title={
                                                hasError
                                                    ? it.error!.description
                                                    : `lim=${fmtLim(it.series.seriesLim)}`
                                            }
                                            className={[
                                                "px-1 py-1 text-center border-t border-l border-border font-semibold tabular-nums",
                                                hasError
                                                    ? "bg-red-900/30 text-red-300"
                                                    : "bg-emerald-900/20 text-emerald-300",
                                            ].join(" ")}
                                            style={{ width: cw }}
                                        >
                                            {hasError
                                                ? (nStep ?? "err")
                                                : fmtLim(it.series.seriesLim)}
                                        </td>
                                    );
                                })}
                            </tr>
                        );
                    })}
                </tbody>
            </table>
        </div>
    );
};

function useAutoColWidth(
    cols: number,
    opts: { rowHdrW?: number; minW?: number; maxW?: number } = {}
) {
    const { rowHdrW = 220, minW = 64, maxW = 160 } = opts;
    const ref = React.useRef<HTMLDivElement | null>(null);
    const [cw, setCw] = React.useState<number>(Math.min(Math.max(minW, 96), maxW));

    React.useLayoutEffect(() => {
        if (!ref.current || cols <= 0) return;
        const el = ref.current;

        const recalc = () => {
            const W = el.clientWidth;
            const avail = Math.max(0, W - rowHdrW);
            const raw = Math.floor(avail / cols);
            const next = Math.max(minW, Math.min(maxW, raw || minW));
            setCw(next);
        };

        recalc();
        const ro = new ResizeObserver(recalc);
        ro.observe(el);
        return () => ro.disconnect();
    }, [cols, rowHdrW, minW, maxW]);

    return { ref, cw };
}
