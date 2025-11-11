import { type ApiJsonResult } from "../types";

export function buildJsonTable(jsonResult: ApiJsonResult | null) {
    if (!Array.isArray(jsonResult)) return { headers: [] as string[], rows: [] as (string | number)[][] };

    const headers = [
        "id",
        "series_name",
        "series_x",
        "series_lim",
        "accel_name",
        "m",
        "n",
        "series_value",
        "partial_sum",
        "partial_sum_deviation",
        "accel_value",
        "accel_value_deviation",
        "error",
    ];

    const rows: (string | number)[][] = [];

    for (const block of jsonResult) {
        const s = block?.series ?? {};
        const a = block?.accel ?? {};
        const blkId = typeof block?.id === "string" ? block.id : "";
        const err = block?.error?.description ? String(block.error.description) : "";
        const seriesName = s?.name ?? "";
        const seriesX =
            s?.arguments && typeof s.arguments === "object" && "x" in s.arguments ? (s.arguments.x as any) : "";
        const seriesLim = typeof s?.lim === "number" ? s.lim : "";
        const accelName = a?.name ?? "";
        const m = a?.m_value ?? "";
        const items: any[] = Array.isArray(block?.computed) ? block.computed : [];

        for (const it of items) {
            rows.push([
                blkId,
                seriesName,
                seriesX,
                seriesLim,
                accelName,
                m,
                it?.n ?? "",
                it?.series_value ?? "",
                it?.partial_sum ?? "",
                it?.partial_sum_deviation ?? "",
                it?.accel_value ?? "",
                it?.accel_value_deviation ?? "",
                err,
            ]);
        }
    }

    return { headers, rows };
}

export type Table = { headers: string[]; rows: any[][] };

export function filterByError(table: Table, mode: "all" | "onlyGood" | "onlyBad"): Table {
    const i = table.headers.indexOf("error_description");
    const j = i === -1 ? table.headers.indexOf("error") : i;
    if (j === -1 || mode === "all") return table;

    const hasErr = (r: any[]) => String(r[j] ?? "").trim().length > 0;
    const rows = mode === "onlyBad" ? table.rows.filter(hasErr) : table.rows.filter((r) => !hasErr(r));
    return { headers: table.headers, rows };
}
