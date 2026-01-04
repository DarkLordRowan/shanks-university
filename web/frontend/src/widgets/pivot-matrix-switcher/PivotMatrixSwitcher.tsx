import React from "react";
import type { PivotByX } from "@/entities/pivot/model/pivotByX.ts";
import { PivotMatrix } from "../pivot-matrix/PivotMatrix";

type Props = { pivots: PivotByX[]; initialIndex?: number };

export const PivotMatrixSwitcher: React.FC<Props> = ({ pivots, initialIndex = 0 }) => {
    if (pivots.length === 0) {
        return (
            <div className="p-3 text-textDim bg-panel border border-border rounded-xl2">
                Нет данных
            </div>
        );
    }

    return (
        <div className="space-y-3">
            <div>{pivots[0].allSeries.length}</div>
            <PivotMatrix pivot={pivots[initialIndex]} />
        </div>
    );
};
