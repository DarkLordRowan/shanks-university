import { useCallback, useId, useMemo, useState } from "react";
import type { AlgorithmNode } from "../data/algorithms";

type Props = {
    value: AlgorithmNode[];                       // выбранные алгоритмы
    onChange: (algs: AlgorithmNode[]) => void;    // вернуть массив
    disabled?: boolean;
    options: AlgorithmNode[];                     // список доступных алгоритмов
};

function norm(s: string) { return s.toLowerCase(); }

export function SelectAlgorithms({ value, onChange, disabled, options }: Props) {
    const id = useId();

    const [collapsed, setCollapsed] = useState<boolean>(value.length > 0);

    const [q, setQ] = useState("");
    const selectedIds = useMemo(() => new Set(value.map(v => v.id)), [value]);

    const items = useMemo(() => {
        const qq = norm(q).trim();
        const scored = options.map(a => {
            const hay = `${a.title} ${a.subtitle ?? ""} ${a.python_id} ${a.id}`.toLowerCase();
            let score = 0;
            if (qq.length) {
                if (hay.includes(qq)) score += 10;
                if (a.python_id.toLowerCase().startsWith(qq)) score += 5;
                if (a.title.toLowerCase().startsWith(qq)) score += 5;
            }
            return { a, score };
        });
        scored.sort((x, y) => y.score - x.score || x.a.title.localeCompare(y.a.title));
        return scored.map(s => s.a);
    }, [options, q]);

    const toggle = useCallback((alg: AlgorithmNode) => {
        if (selectedIds.has(alg.id)) {
            onChange(value.filter(v => v.id !== alg.id));
        } else {
            onChange([...value, alg]);
        }
    }, [onChange, selectedIds, value]);

    const selectAll = useCallback(() => onChange(items), [items, onChange]);
    const clearAll  = useCallback(() => onChange([]), [onChange]);

    const handleConfirm = useCallback(() => {
        setCollapsed(true);
    }, []);

    const handleEdit = useCallback(() => setCollapsed(false), []);

    if (collapsed) {
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
                <span className="text-textDim">Алгоритмы:</span>
                <span className="font-medium">
          {value.length === 0 ? "—"
              : value.length === 1 ? value[0].title
                  : `${value.length} выбрано`}
        </span>
                {value.length === 1 && <span className="ml-2 text-xs text-textDim">{value[0].python_id}</span>}
                <button
                    type="button"
                    className="ml-auto rounded-md border border-border px-2 py-1 text-xs"
                    onClick={handleEdit}
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
                <div className="text-xs font-semibold text-textDim">Выбор алгоритмов</div>
                <div className="flex gap-2">
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={selectAll} disabled={disabled}>Выбрать все</button>
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={clearAll} disabled={disabled}>Снять все</button>
                </div>
            </div>

            <label htmlFor={`${id}-alg-q`} className="sr-only">Поиск алгоритма</label>
            <input
                id={`${id}-alg-q`}
                type="text"
                placeholder="Поиск: название, python_id, id…"
                value={q}
                onChange={e => setQ(e.target.value)}
                disabled={disabled}
                className="mb-2 w-full rounded-md border border-border bg-surface px-2 py-1 text-sm outline-none focus:ring-1 focus:ring-primary/50"
            />

            <div className="max-h-64 overflow-auto rounded-md border border-border">
                {items.map(a => {
                    const active = selectedIds.has(a.id);
                    return (
                        <label key={a.id}
                               className={[
                                   "flex w-full cursor-pointer items-start gap-2 border-b border-border px-3 py-2 last:border-b-0",
                                   active ? "bg-primary/15 ring-1 ring-primary/30" : "hover:bg-surface"
                               ].join(" ")}>
                            <input
                                type="checkbox"
                                className="mt-1"
                                checked={active}
                                onChange={() => toggle(a)}
                                disabled={disabled}
                            />
                            <div className="min-w-0">
                                <div className="truncate font-medium">{a.title}</div>
                                {a.subtitle && <div className="truncate text-xs text-textDim">{a.subtitle}</div>}
                                <div className="truncate text-[11px] text-textDim/80">{a.python_id}</div>
                            </div>
                        </label>
                    );
                })}
                {items.length === 0 && (
                    <div className="px-3 py-2 text-xs text-textDim">Ничего не найдено.</div>
                )}
            </div>

            <div className="mt-3 flex items-center gap-2 text-[11px]">
        <span className="text-textDim">
          Выбрано: {value.length}
        </span>
                <button
                    type="button"
                    onClick={handleConfirm}
                    disabled={disabled}
                    className="ml-auto rounded-md border border-border px-3 py-1 text-xs"
                >
                    Подтвердить
                </button>
            </div>
        </div>
    );
}
