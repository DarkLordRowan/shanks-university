import { useCallback, useEffect, useMemo, useState } from "react";

type Props = {
    value: number[] ;                    // выбранные m
    onChange: (ms: number[]) => void;   // вернуть массив
    disabled?: boolean;
};

function isValidM(n: number) { return Number.isInteger(n) && n >= 0 && n <= 100 && n % 2 === 0; }

export function SelectMs({ value, onChange, disabled }: Props) {
    const [collapsed, setCollapsed] = useState<boolean>(value.length > 0);
    useEffect(() => { setCollapsed(value.length > 0); }, [value.length]);

    const all = useMemo(() => Array.from({length: 51}, (_, k) => 2*k), []);
    const set = useMemo(() => new Set(value), [value]);

    const toggle = useCallback((m: number) => {
        if (!isValidM(m)) return;
        if (set.has(m)) onChange(value.filter(v => v !== m));
        else onChange([...value, m].sort((a,b) => a-b));
    }, [onChange, set, value]);

    const selectAll = useCallback(() => onChange(all), [all, onChange]);
    const clearAll  = useCallback(() => onChange([]), [onChange]);

    if (collapsed) {
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
                <span className="text-textDim">m:</span>
                <span className="font-mono">
          {value.length === 0 ? "—" : value.join(", ")}
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
                <div className="text-xs font-semibold text-textDim">Выбор m ∈ {`{0,2,…,100}`}</div>
                <div className="flex gap-2">
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={selectAll} disabled={disabled}>Все</button>
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={clearAll} disabled={disabled}>Очистить</button>
                    <button type="button" className="rounded border border-border px-2 py-0.5 text-xs"
                            onClick={() => setCollapsed(true)} disabled={disabled}>Подтвердить</button>
                </div>
            </div>

            {/* сетка чипов */}
            <div className="grid grid-cols-8 gap-2 max-sm:grid-cols-4">
                {all.map(m => {
                    const active = set.has(m);
                    return (
                        <button
                            key={m}
                            type="button"
                            onClick={() => toggle(m)}
                            disabled={disabled}
                            className={[
                                "rounded-md border px-2 py-1 text-xs",
                                active ? "border-primary ring-1 ring-primary/50 bg-primary/10"
                                    : "border-border hover:border-primary"
                            ].join(" ")}
                        >
                            {m}
                        </button>
                    );
                })}
            </div>
        </div>
    );
}
