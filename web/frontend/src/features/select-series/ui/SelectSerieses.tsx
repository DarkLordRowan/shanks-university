import { useCallback, useMemo, useState } from "react";
import type { SeriesNode } from "@/entities/experiment/model/seriesData";
import { Formula } from "@/shared/ui/formula/Formula";

type Props = {
    items: SeriesNode[];                  // доступные ряды
    value: SeriesNode[];                  // выбранные ряды
    onChange: (s: SeriesNode[]) => void;  // вернуть массив
    disabled?: boolean;
    placeholder?: string;
    searchField?: keyof Pick<SeriesNode, "title" | "formula">;
};

function searchText(x: SeriesNode, field: "title"|"formula") {
    const base = `${x.title} ${x.id} ${x.python_id} ${x.formula ?? ""}`;
    return field === "formula" ? `${x.formula ?? ""} ${base}` : base;
}

export function SelectSerieses({
                                   items,
                                   value,
                                   onChange,
                                   disabled,
                                   placeholder = "Поиск ряда…",
                                   searchField = "title",
                               }: Props) {
    const [collapsed, setCollapsed] = useState<boolean>(value.length > 0);

    const [q, setQ] = useState("");
    const selected = useMemo(() => new Set(value.map(v => v.id)), [value]);

    const filtered = useMemo(() => {
        const qq = q.toLowerCase().trim();
        if (!qq.length) return items.slice().sort((a,b) => a.num - b.num);
        return items
            .filter(x => searchText(x, searchField).toLowerCase().includes(qq))
            .sort((a,b) => a.num - b.num);
    }, [items, q, searchField]);

    const toggle = useCallback((s: SeriesNode) => {
        if (selected.has(s.id)) onChange(value.filter(v => v.id !== s.id));
        else onChange([...value, s].sort((a,b) => a.num - b.num));
    }, [onChange, selected, value]);

    const selectAll = useCallback(() => onChange(items.slice().sort((a,b)=>a.num-b.num)), [items, onChange]);
    const clearAll  = useCallback(() => onChange([]), [onChange]);

    if (collapsed) {
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
                <span className="text-textDim">Ряды:</span>
                <span className="font-medium">
          {value.length === 0 ? "—"
              : value.length === 1 ? `${value[0].num}. ${value[0].title}`
                  : `${value.length} выбрано`}
        </span>
                <button
                    type="button"
                    className="ml-auto rounded-md border border-border px-2 py-1 text-xs"
                    onClick={() => setCollapsed(false)}
                    disabled={disabled}
                >
                    Изменить
                </button>
            </div>
        );
    }

    return (
        <div className="rounded-xl2 border border-border bg-panel p-3 text-sm text-white shadow-panel">
            <div className="mb-2 flex items-center justify-between gap-2">
                <div className="text-xs font-semibold text-textDim">Выбор рядов</div>
                <div className="flex gap-2">
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={selectAll} disabled={disabled}>Все</button>
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={clearAll} disabled={disabled}>Очистить</button>
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={() => setCollapsed(true)} disabled={disabled}>Подтвердить</button>
                </div>
            </div>

            <input
                type="text"
                placeholder={placeholder}
                value={q}
                onChange={e => setQ(e.target.value)}
                disabled={disabled}
                className="mb-2 w-full rounded-md border border-border bg-surface px-2 py-1 text-sm outline-none focus:ring-1 focus:ring-primary/50"
            />

            <div className="max-h-64 overflow-auto rounded-md border border-border">
                {filtered.map(s => {
                    const active = selected.has(s.id);
                    return (
                        <label key={s.id}
                               className={[
                                   "flex w-full cursor-pointer items-start gap-2 border-b border-border px-3 py-2 last:border-b-0",
                                   active ? "bg-primary/15 ring-1 ring-primary/30" : "hover:bg-surface"
                               ].join(" ")}>
                            <input
                                type="checkbox"
                                className="mt-1"
                                checked={active}
                                onChange={() => toggle(s)}
                                disabled={disabled}
                            />
                            <div className="min-w-0">
                                <div className="truncate inline-flex items-baseline gap-1">
                                    <span className="text-textDim">{s.num}.</span>
                                    <Formula inline latex={s.title} />
                                </div>
                                <div className="truncate text-[11px] text-textDim/80">{s.python_id}</div>
                            </div>
                        </label>
                    );
                })}
                {filtered.length === 0 && (
                    <div className="px-3 py-2 text-xs text-textDim">Ничего не найдено.</div>
                )}
            </div>
        </div>
    );
}
