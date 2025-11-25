import React from "react";
import { type DetailPoint } from "../../model/types";

interface ConvergenceDiffTableProps {
    points: DetailPoint[];
    useAbs: boolean;
}

const formatNumber = (v: number | null): string => {
    if (v == null || !Number.isFinite(v)) return "∅";
    const abs = Math.abs(v);
    if (abs >= 1e5 || abs < 1e-4) return v.toExponential(4);
    return v.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
};

export const ConvergenceDiffTable: React.FC<ConvergenceDiffTableProps> = ({ points, useAbs }) => {
    const hasDiff = points.some((p) => p.diffNorm != null);

    const headerNorm = useAbs ? "|Aₙ − Aₙ₋₁|" : "sgn·|Aₙ − Aₙ₋₁|";

    return (
        <div className="mt-4 border-t border-border/60 pt-3">
            <div className="mb-2 text-[11px] font-semibold text-textDim">Разности Aₙ − Aₙ₋₁</div>

            {hasDiff ? (
                <div className="max-h-64 overflow-auto rounded border border-border bg-surface/60">
                    <table className="min-w-full border-collapse text-[10px]">
                        <thead className="bg-surface/80">
                            <tr>
                                <th className="border-b border-border px-2 py-1 text-left">n</th>
                                <th className="border-b border-border px-2 py-1 text-left">
                                    Re(Aₙ − Aₙ₋₁)
                                </th>
                                <th className="border-b border-border px-2 py-1 text-left">
                                    Im(Aₙ − Aₙ₋₁)
                                </th>
                                <th className="border-b border-border px-2 py-1 text-left">
                                    {headerNorm}
                                </th>
                            </tr>
                        </thead>
                        <tbody>
                            {points
                                .filter((p) => p.diffNorm != null)
                                .slice(0, 200)
                                .map((p) => {
                                    const { n, diffRe, diffIm, diffNorm } = p;

                                    let normValue: number | null = null;
                                    if (diffNorm != null && Number.isFinite(diffNorm)) {
                                        if (!useAbs && diffRe != null && Number.isFinite(diffRe)) {
                                            normValue = diffRe >= 0 ? diffNorm : -diffNorm;
                                        } else {
                                            normValue = diffNorm;
                                        }
                                    }

                                    return (
                                        <tr
                                            key={`diff-${n}`}
                                            className="odd:bg-surface/40 even:bg-surface/20"
                                        >
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {n}
                                            </td>
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {formatNumber(
                                                    diffRe != null && Number.isFinite(diffRe)
                                                        ? diffRe
                                                        : null
                                                )}
                                            </td>
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {formatNumber(
                                                    diffIm != null && Number.isFinite(diffIm)
                                                        ? diffIm
                                                        : null
                                                )}
                                            </td>
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {formatNumber(normValue)}
                                            </td>
                                        </tr>
                                    );
                                })}
                        </tbody>
                    </table>
                </div>
            ) : (
                <div className="text-[11px] text-textDim/70">
                    Недостаточно данных для вычисления разностей Aₙ − Aₙ₋₁.
                </div>
            )}
        </div>
    );
};
