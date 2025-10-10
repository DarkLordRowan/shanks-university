import React from "react";
import { PivotMatrix } from "./PivotMatrix";
import type { PivotByX } from "../types/pivotByX.ts";

type Props = { pivots: PivotByX[]; initialIndex?: number };

export const PivotMatrixSwitcher: React.FC<Props> = ({ pivots, initialIndex = 0 }) => {
    const [idx, setIdx] = React.useState(() =>
        Math.min(Math.max(0, initialIndex), Math.max(0, pivots.length - 1))
    );

    if (pivots.length === 0) {
        return <div className="p-3 text-textDim bg-panel border border-border rounded-xl2">Нет данных</div>;
    }

    return (
        <div className="space-y-3">
            <div className="flex items-center gap-3">
                <label htmlFor="pivot-x" className="text-sm text-textDim">
                    x:
                </label>
                <select
                    id="pivot-x"
                    value={idx}
                    onChange={(e) => setIdx(Number(e.target.value))}
                    className="form-select rounded-xl2 border-border bg-panel text-textDim focus:border-primary focus:ring-primary"
                >
                    {pivots.map((p, i) => (
                        <option key={`${p.x}_${i}`} value={i}>
                            {Number.isNaN(p.x) ? "NaN" : p.x}
                        </option>
                    ))}
                </select>
            </div>

            <PivotMatrix pivot={pivots[idx]} />
        </div>
    );
};
