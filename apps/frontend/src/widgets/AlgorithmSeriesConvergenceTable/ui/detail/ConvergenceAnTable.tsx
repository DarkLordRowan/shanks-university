import React from "react";
import { type DetailPoint } from "../../model/types";

interface ConvergenceAnTableProps {
    points: DetailPoint[];
    useAbs: boolean;
}

const formatNumber = (v: number | null): string => {
    if (v == null || !Number.isFinite(v)) return "∅";
    const abs = Math.abs(v);
    if (abs >= 1e5 || abs < 1e-4) return v.toExponential(4);
    return v.toFixed(12).replace(/0+$/g, "").replace(/\.$/, "");
};

export const ConvergenceAnTable: React.FC<ConvergenceAnTableProps> = ({ points, useAbs }) => {
    if (!points.length) {
        return null;
    }

    const headerErr = useAbs ? "|Aₙ − lim|" : "sgn·|Aₙ − lim|";

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
                                {headerErr}
                            </th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                sgn(Re(Aₙ − lim))
                            </th>
                        </tr>
                    </thead>
                    <tbody>
                        {points.slice(0, 200).map((p) => {
                            const { n, valueRe, valueIm, err, sign } = p;

                            let errValue: number | null = null;
                            if (err != null && Number.isFinite(err)) {
                                if (useAbs || !sign || sign === 0) {
                                    errValue = err;
                                } else {
                                    errValue = err * sign;
                                }
                            }

                            return (
                                <tr key={n} className="odd:bg-surface/40 even:bg-surface/20">
                                    <td className="border-t border-border px-2 py-1 font-mono">
                                        {n}
                                    </td>
                                    <td className="border-t border-border px-2 py-1 font-mono">
                                        {formatNumber(
                                            valueRe != null && Number.isFinite(valueRe)
                                                ? valueRe
                                                : null
                                        )}
                                    </td>
                                    <td className="border-t border-border px-2 py-1 font-mono">
                                        {formatNumber(
                                            valueIm != null && Number.isFinite(valueIm)
                                                ? valueIm
                                                : null
                                        )}
                                    </td>
                                    <td className="border-t border-border px-2 py-1 font-mono">
                                        {formatNumber(errValue)}
                                    </td>
                                    <td className="border-t border-border px-2 py-1 font-mono">
                                        {sign === 1 ? "+" : sign === -1 ? "−" : "0 / ?"}
                                    </td>
                                </tr>
                            );
                        })}
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
