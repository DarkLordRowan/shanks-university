import * as React from "react";
import type { Item } from "@/entities/item/model/item";
import {
    type Row, type SortState, type SortKey,
    buildRowsFromItems, collectArgColumns, sortRows, makeKey
} from "./items-selection-utils";

export function useItemsSelection(source: Item[]) {
    const initial = React.useMemo(() => buildRowsFromItems(source), [source]);
    const [rows, setRows] = React.useState<Row[]>(initial);
    const [sort, setSort] = React.useState<SortState>([
        { key: "algo", dir: 1 },
        { key: "m", dir: 1 },
    ]);

    React.useEffect(() => { setRows(buildRowsFromItems(source)); }, [source]);

    const sortedRows = React.useMemo(() => sortRows(rows, sort), [rows, sort]);
    const argCols = React.useMemo(() => collectArgColumns(sortedRows), [sortedRows]);

    const toggle = (id: string) =>
        setRows(rs => rs.map(r => r.id === id ? { ...r, enabled: !r.enabled } : r));
    const setAll = (val: boolean) =>
        setRows(rs => rs.map(r => ({ ...r, enabled: val })));
    const cycleSort = (key: SortKey) =>
        setSort(prev => {
            const i = prev.findIndex(s => s.key === key);
            if (i >= 0) {
                const copy = [...prev];
                copy[i] = { key, dir: copy[i].dir === 1 ? -1 : 1 };
                return copy;
            }
            return [{ key, dir: 1 }, ...prev];
        });

    const enabledKeys = React.useMemo(() => {
        const s = new Set<string>();
        for (const r of rows) if (r.enabled) s.add(r.id);
        return s;
    }, [rows]);

    const filteredItems = React.useMemo(
        () => source.filter(it => enabledKeys.has(makeKey(it))),
        [source, enabledKeys]
    );

    return { rows: sortedRows, argCols, toggle, setAll, cycleSort, filteredItems };
}
