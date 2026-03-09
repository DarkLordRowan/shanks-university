import React from "react";
import { formatArg } from "../model/items-selection-utils";
import type { useItemsSelection } from "../model/useItemsSelection";

type Rows = ReturnType<typeof useItemsSelection>["rows"];

export const ItemsSelectionMatrix: React.FC<{
    rows: Rows;
    argCols: string[];
    onToggle: (id: string) => void;
    onSetAll: (v: boolean) => void;
    onSort: (k: "algo" | "m" | "x" | "seriesName") => void;
}> = ({ rows, argCols, onToggle, onSetAll, onSort }) => (
    <div className="rounded-xl border border-gray-200 dark:border-neutral-700 overflow-auto">
        <div className="flex items-center gap-2 p-2 border-b border-gray-200 dark:border-neutral-700">
            <div className="text-xs">Всего: {rows.length}</div>
            <div className="text-xs">Включено: {rows.filter(r => r.enabled).length}</div>
            <div className="ml-auto flex gap-2">
                <button className="rounded-md border px-2 py-0.5 text-xs" onClick={() => onSetAll(true)}>Включить все</button>
                <button className="rounded-md border px-2 py-0.5 text-xs" onClick={() => onSetAll(false)}>Выключить все</button>
            </div>
        </div>

        <table className="min-w-full text-xs">
            <thead>
            <tr className="bg-gray-50 dark:bg-neutral-900/60">
                <Th text="ОТОБРАЗИТЬ?" />
                <Th text="Алгоритм" onClick={() => onSort("algo")} />
                <Th text="m" onClick={() => onSort("m")} />
                <Th text="x" onClick={() => onSort("x")} />
                <Th text="Ряд" onClick={() => onSort("seriesName")} />
                {argCols.map(k => <Th key={k} text={k} />)}
            </tr>
            </thead>
            <tbody>
            {rows.map(r => (
                <tr key={r.id} className="border-t border-gray-100 dark:border-neutral-800">
                    <td className="px-2 py-1">
                        <input type="checkbox" checked={r.enabled} onChange={() => onToggle(r.id)} />
                    </td>
                    <td className="px-2 py-1">{r.algo}</td>
                    <td className="px-2 py-1 tabular-nums">{r.m ?? "∅"}</td>
                    <td className="px-2 py-1 tabular-nums">{r.x}</td>
                    <td className="px-2 py-1">{r.seriesName}</td>
                    {argCols.map(k => (
                        <td key={k} className="px-2 py-1">{formatArg(r.args?.[k])}</td>
                    ))}
                </tr>
            ))}
            </tbody>
        </table>
    </div>
);

const Th: React.FC<{ text: string; onClick?: () => void }> = ({ text, onClick }) =>
    <th className="px-2 py-1 font-semibold text-left cursor-pointer select-none" onClick={onClick}>{text}</th>;
