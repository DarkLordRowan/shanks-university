import  { useMemo } from "react";
import type { Item } from "@/types/item";
import { DataTable } from "@/components/ui/DataTable.tsx";

export function ItemsTable({ items }: { items: Item[] }) {
    const headers = useMemo(
        () => [
            "id",
            "seriesName",
            "x",
            "lim",
            "algo",
            "m",
            "computed points",
            "has error",
            "error n",
            "error description",
        ],
        [],
    );

    const rows = useMemo(
        () =>
            items.map((it) => {
                const hasError = it.error != null;
                const errorN = it.error?.data?.n ?? "";
                const errorDescription = it.error?.description ?? "";
                return [
                    it.id,
                    it.series.seriesName,
                    it.series.x,
                    it.series.seriesLim ?? "",
                    it.algorithm.algorithmName,
                    it.algorithm.m ?? "",
                    it.computed.length,
                    hasError ? "yes" : "no",
                    errorN,
                    errorDescription,
                ] as (string | number)[];
            }),
        [items],
    );

    return (
        <DataTable
            headers={headers}
            rows={rows}
            searchable={true}
            compact={true}
            enableSorting={true}
            enableColumnFilters={true}
            storageKey="items-table-v1"
            // можно подсказать типы явно
            columnTypes={[
                "text",   // id
                "text",   // seriesName
                "number", // x
                "number", // lim
                "text",   // algo
                "number", // m
                "number", // computed points
                "text",   // has error
                "number", // error n
                "text",   // error description
            ]}
        />
    );
}
