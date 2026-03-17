import * as XLSX from "xlsx-js-style";

type KeyValue = {
    key: string;
    value: string | number | boolean | null;
};

type SheetDecoratorArgs = {
    rowIndex: number;
    colIndex: number;
    cell: XLSX.CellObject;
};

interface BuildSheetOptions {
    cols?: Array<{ wch?: number }>;
    rows?: Array<{ hpt?: number }>;
    headerRows?: number;
    rowHeaderCols?: number;
    autofilter?: boolean;
    decorateCell?: (args: SheetDecoratorArgs) => void;
}

const BORDER: XLSX.Borders = {
    top: { style: "thin", color: { rgb: "374151" } },
    bottom: { style: "thin", color: { rgb: "374151" } },
    left: { style: "thin", color: { rgb: "374151" } },
    right: { style: "thin", color: { rgb: "374151" } },
};

function cloneStyle(style: XLSX.CellStyle): XLSX.CellStyle {
    return {
        ...style,
        fill: style.fill ? { ...style.fill } : undefined,
        font: style.font ? { ...style.font } : undefined,
        alignment: style.alignment ? { ...style.alignment } : undefined,
        border: style.border
            ? {
                  top: style.border.top ? { ...style.border.top } : undefined,
                  bottom: style.border.bottom ? { ...style.border.bottom } : undefined,
                  left: style.border.left ? { ...style.border.left } : undefined,
                  right: style.border.right ? { ...style.border.right } : undefined,
              }
            : undefined,
    };
}

export const XLSX_THEME = {
    header(): XLSX.CellStyle {
        return {
            fill: { patternType: "solid", fgColor: { rgb: "0B1220" } },
            font: { color: { rgb: "E5E7EB" }, bold: true },
            alignment: { horizontal: "center", vertical: "center", wrapText: true },
            border: BORDER,
        };
    },
    rowHeader(): XLSX.CellStyle {
        return {
            fill: { patternType: "solid", fgColor: { rgb: "0F172A" } },
            font: { color: { rgb: "E5E7EB" }, bold: true },
            alignment: { horizontal: "left", vertical: "top", wrapText: true },
            border: BORDER,
        };
    },
    body(): XLSX.CellStyle {
        return {
            alignment: { horizontal: "left", vertical: "top", wrapText: true },
            border: BORDER,
        };
    },
    bodyCentered(): XLSX.CellStyle {
        return {
            alignment: { horizontal: "center", vertical: "center", wrapText: true },
            border: BORDER,
        };
    },
};

export function createWorkbook(title: string, subject?: string): XLSX.WorkBook {
    const workbook = XLSX.utils.book_new();
    workbook.Props = {
        Title: title,
        Subject: subject ?? title,
        Author: "Codex",
        Company: "Codex",
        CreatedDate: new Date(),
    };
    return workbook;
}

export function buildSheetFromAoa(
    aoa: Array<Array<string | number | boolean | null>>,
    options: BuildSheetOptions = {}
): XLSX.WorkSheet {
    const {
        cols,
        rows,
        headerRows = 1,
        rowHeaderCols = 0,
        autofilter = true,
        decorateCell,
    } = options;

    const worksheet = XLSX.utils.aoa_to_sheet(aoa);

    if (cols) worksheet["!cols"] = cols;
    if (rows) worksheet["!rows"] = rows;

    const ref = worksheet["!ref"] ?? "A1:A1";
    if (autofilter) {
        worksheet["!autofilter"] = { ref };
    }

    const range = XLSX.utils.decode_range(ref);

    for (let rowIndex = range.s.r; rowIndex <= range.e.r; rowIndex++) {
        for (let colIndex = range.s.c; colIndex <= range.e.c; colIndex++) {
            const addr = XLSX.utils.encode_cell({ r: rowIndex, c: colIndex });
            const cell = worksheet[addr];
            if (!cell) continue;

            let style = XLSX_THEME.body();
            if (rowIndex < headerRows) {
                style = XLSX_THEME.header();
            } else if (colIndex < rowHeaderCols) {
                style = XLSX_THEME.rowHeader();
            }

            cell.s = cloneStyle(style);
            decorateCell?.({ rowIndex, colIndex, cell });
        }
    }

    return worksheet;
}

export function buildKeyValueSheet(
    rows: KeyValue[],
    label = "field",
    valueLabel = "value"
): XLSX.WorkSheet {
    const aoa: Array<Array<string | number | boolean | null>> = [
        [label, valueLabel],
        ...rows.map((row) => [row.key, row.value]),
    ];

    return buildSheetFromAoa(aoa, {
        cols: [{ wch: 24 }, { wch: 36 }],
        headerRows: 1,
        rowHeaderCols: 1,
        autofilter: false,
    });
}

export function appendSheet(
    workbook: XLSX.WorkBook,
    worksheet: XLSX.WorkSheet,
    name: string
): XLSX.WorkBook {
    XLSX.utils.book_append_sheet(workbook, worksheet, name);
    return workbook;
}
