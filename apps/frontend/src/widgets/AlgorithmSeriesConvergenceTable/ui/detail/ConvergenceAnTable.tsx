import React from "react";
import { type DetailPoint } from "../../model/types";

interface ConvergenceAnTableProps {
    points: DetailPoint[];
}

export const ConvergenceAnTable: React.FC<ConvergenceAnTableProps> = ({ points }) => {
    if (!points.length) {
        return null;
    }

    return (
        <>
            <div className="mt-2 max-h-64 overflow-auto rounded border border-border bg-surface/60">
                <table className="min-w-full border-collapse text-[10px]">
                    <thead className="bg-surface/80">
                        <tr>
                            <th className="border-b border-border px-2 py-1 text-left">n</th>
                            <th className="border-b border-border px-2 py-1 text-left">Re(Aₙ)</th>
                            <th className="border-b border-border px-2 py-1 text-left">Im(Aₙ)</th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                |Aₙ − lim|
                            </th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                sgn(Re(Aₙ − lim))
                            </th>
                        </tr>
                    </thead>
                    <tbody>
                        {points.slice(0, 200).map((p) => (
                            <tr key={p.n} className="odd:bg-surface/40 even:bg-surface/20">
                                <td className="border-t border-border px-2 py-1 font-mono">{p.n}</td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.valueRe != null && Number.isFinite(p.valueRe)
                                        ? p.valueRe.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.valueIm != null && Number.isFinite(p.valueIm)
                                        ? p.valueIm.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.err != null && Number.isFinite(p.err)
                                        ? p.err.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.sign === 1 ? "+" : p.sign === -1 ? "−" : "0 / ?"}
                                </td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
            {points.length > 200 && (
                <div className="mt-1 text-[10px] text-textDim/60">
                    Показаны первые 200 точек из {points.length}.
                </div>
            )}
        </>
    );
};
