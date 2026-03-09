import { buildMatrixCellDomId } from "@/shared/lib/dom/buildMatrixCellDomId";

export function getAlgorithmSeriesCellDomId(accelId: string, seriesId: string): string {
    return buildMatrixCellDomId("algo-series-cell", accelId, seriesId);
}
