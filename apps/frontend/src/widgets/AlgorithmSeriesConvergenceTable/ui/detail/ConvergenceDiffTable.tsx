import React from "react";
import { type DetailPoint } from "../../model/types";

interface ConvergenceDiffTableProps {
    points: DetailPoint[];
}

export const ConvergenceDiffTable: React.FC<ConvergenceDiffTableProps> = ({ points }) => {
    const hasDiff = points.some((p) => p.diffNorm != null);

    return (
        <div className="mt-4 border-t border-border/60 pt-3">
            <div className="mb-2 text-[11px] font-semibold text-textDim">
                Разности Aₙ − Aₙ₋₁
            </div>

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
                                    |Aₙ − Aₙ₋₁|
                                </th>
                            </tr>
                        </thead>
                        <tbody>
                            {points
                                .filter((p) => p.diffNorm != null)
                                .slice(0, 200)
                                .map((p) => (
                                    <tr
                                        key={`diff-${p.n}`}
                                        className="odd:bg-surface/40 even:bg-surface/20"
                                    >
                                        <td className="border-t border-border px-2 py-1 font-mono">
                                            {p.n}
                                        </td>
                                        <td className="border-t border-border px-2 py-1 font-mono">
                                            {p.diffRe != null && Number.isFinite(p.diffRe)
                                                ? p.diffRe.toExponential(4)
                                                : "∅"}
                                        </td>
                                        <td className="border-t border-border px-2 py-1 font-mono">
                                            {p.diffIm != null && Number.isFinite(p.diffIm)
                                                ? p.diffIm.toExponential(4)
                                                : "∅"}
                                        </td>
                                        <td className="border-t border-border px-2 py-1 font-mono">
                                            {p.diffNorm != null && Number.isFinite(p.diffNorm)
                                                ? p.diffNorm.toExponential(4)
                                                : "∅"}
                                        </td>
                                    </tr>
                                ))}
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
