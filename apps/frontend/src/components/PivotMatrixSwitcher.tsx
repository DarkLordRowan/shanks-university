import React from "react";
import { PivotMatrix } from "./PivotMatrix";
import type { PivotByX } from "../types/pivotByX.ts";

type Props = { pivots: PivotByX[]; initialIndex?: number };

export const PivotMatrixSwitcher: React.FC<Props> = ({ pivots, initialIndex = 0 }) => {

    if (pivots.length === 0) {
        return <div className="p-3 text-textDim bg-panel border border-border rounded-xl2">Нет данных</div>;
    }

    return (
        <div className="space-y-3">
            <PivotMatrix pivot={pivots[initialIndex]} />
        </div>
    );
};
