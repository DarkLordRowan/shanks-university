export function buildMatrixCellDomId(prefix: string, accelId: string, seriesId: string): string {
    return `${prefix}-${accelId}::${seriesId}`;
}
