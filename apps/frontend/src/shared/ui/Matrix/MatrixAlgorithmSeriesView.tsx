import React, { useMemo } from "react";
import * as XLSX from "xlsx-js-style";
import type { Accel, Series } from "@/entities/experiment/model/experiment";
import type { MatrixAxisItem, MatrixProps } from "@/shared/ui/Matrix/Matrix";
import { MatrixPaged } from "@/shared/ui/Matrix/MatrixPaged";

type AlgoRowMeta = { accel: Accel; accelIndex: number };
type SeriesColMeta = { series: Series; seriesIndex: number };

export type PagerInfo = {
    page: number;
    totalPages: number;
    pageSize: number;
    startIndex: number;
    endIndex: number;
    totalCols: number;
};

function formatArgs(args: Record<string, any> | null | undefined): string {
    if (!args) return "";
    const entries = Object.entries(args).filter(([, v]) => v !== null && v !== undefined);
    if (entries.length === 0) return "";
    return entries.map(([k, v]) => `${k}=${typeof v === "string" ? v : String(v)}`).join(", ");
}

export interface MatrixAlgorithmSeriesViewProps
    extends Omit<
        React.ComponentProps<typeof MatrixPaged<AlgoRowMeta, SeriesColMeta>>,
        "rows" | "cols" | "renderRowHeader" | "renderColHeader" | "renderCell" | "export"
    > {
    accelList: Accel[];
    seriesList: Series[];

    rowWidth?: number | string;
    colWidth?: number | string;

    renderCell: (
        accel: Accel,
        series: Series,
        rowIndex: number,
        colIndex: number
    ) => React.ReactNode;

    renderAlgoHeader?: (accel: Accel, rowIndex: number) => React.ReactNode;
    renderSeriesHeader?: (series: Series, colIndex: number) => React.ReactNode;
    renderCorner?: () => React.ReactNode;

    export?: {
        fileBaseName: string;
        enablePng?: boolean;
        enableXlsx?: boolean;
        buildWorkbook: (args: {
            accelList: Accel[];
            seriesList: Series[];
            pager: PagerInfo;
        }) => XLSX.WorkBook;
    };
}

export function MatrixAlgorithmSeriesView(props: MatrixAlgorithmSeriesViewProps) {
    const {
        accelList,
        seriesList,
        rowWidth = 160,
        colWidth = 50,
        renderCell,
        renderAlgoHeader,
        renderSeriesHeader,
        renderCorner,
        export: exportCfg,
        ...rest
    } = props;

    const rows: MatrixAxisItem<AlgoRowMeta>[] = useMemo(
        () =>
            (accelList ?? []).map((accel, accelIndex) => ({
                id: accel.id,
                meta: { accel, accelIndex },
            })),
        [accelList]
    );

    const cols: MatrixAxisItem<SeriesColMeta>[] = useMemo(
        () =>
            (seriesList ?? []).map((series, seriesIndex) => ({
                id: series.id,
                meta: { series, seriesIndex },
            })),
        [seriesList]
    );

    const corner: MatrixProps<AlgoRowMeta, SeriesColMeta>["renderCorner"] = () => {
        if (renderCorner) return renderCorner();

        return (
            <div className="text-left text-[10px] text-textDim">
                <span className="font-medium">Алгоритм</span>
                <span className="text-textDim/60"> \ </span>
                <span className="font-medium">Ряд</span>
            </div>
        );
    };

    const rowHeader: MatrixProps<AlgoRowMeta, SeriesColMeta>["renderRowHeader"] = (row, i) => {
        const a = row.meta!.accel;
        if (renderAlgoHeader) return renderAlgoHeader(a, i);

        const args = formatArgs(a.args);
        return (
            <div className="flex flex-col leading-tight">
                <span className="font-medium text-textDim">{a.name}</span>
                <span className="text-[10px] text-textDim/70">
                    {a.m !== null && a.m !== undefined ? `m=${a.m}` : ""}
                    {a.m !== null && a.m !== undefined && args ? " · " : ""}
                    {args}
                </span>
            </div>
        );
    };

    const colHeader: MatrixProps<AlgoRowMeta, SeriesColMeta>["renderColHeader"] = (col, j) => {
        const s = col.meta!.series;
        if (renderSeriesHeader) return renderSeriesHeader(s, j);

        const args = formatArgs(s.args as any);
        return (
            <div
                className="flex flex-col items-center justify-end gap-1 px-1 py-1"
                title={`${s.name}\n prec = ${s.precision}\n args: ${args}`}
            >
                <span
                    className="text-[9px] leading-tight text-center whitespace-nowrap"
                    style={{
                        writingMode: "vertical-rl",
                        textOrientation: "mixed",
                        transform: "rotate(180deg)",
                    }}
                >
                    {s.name}
                </span>

                <span className="text-[8px] leading-tight text-textDim/60 whitespace-nowrap">
                    {s.precision}
                </span>

                <span className="text-[8px] leading-tight text-textDim/70 whitespace-nowrap">
                    {args}
                </span>
            </div>
        );
    };

    return (
        <MatrixPaged<AlgoRowMeta, SeriesColMeta>
            {...rest}
            rows={rows}
            cols={cols}
            rowWidth={rowWidth}
            colWidth={colWidth}
            renderCorner={corner}
            renderRowHeader={rowHeader}
            renderColHeader={colHeader}
            renderCell={(row, col, i, j) => renderCell(row.meta!.accel, col.meta!.series, i, j)}
            export={
                exportCfg
                    ? {
                          fileBaseName: exportCfg.fileBaseName,
                          enablePng: exportCfg.enablePng,
                          enableXlsx: exportCfg.enableXlsx,
                          buildWorkbook: ({ pager }) =>
                              exportCfg.buildWorkbook({
                                  accelList,
                                  seriesList,
                                  pager: pager as PagerInfo,
                              }),
                      }
                    : undefined
            }
        />
    );
}
