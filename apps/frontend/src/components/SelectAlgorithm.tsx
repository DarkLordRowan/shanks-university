import { useEffect, useId, useMemo, useState, useCallback, useRef } from "react";
import type { AlgorithmNode } from "../data/algorithms";
import { ALGORITHMS } from "../data/algorithms";

type Props = {
    value: AlgorithmNode | null;
    onChange: (alg: AlgorithmNode | null) => void;
    disabled?: boolean;
    options?: AlgorithmNode[];
};

function norm(s: string) { return s.toLowerCase(); }

export function SelectAlgorithm({ value, onChange, disabled, options }: Props) {
    const id = useId();
    const source = options ?? ALGORITHMS;

    // свёрнут, если алгоритм уже выбран
    const [collapsed, setCollapsed] = useState<boolean>(value != null);
    useEffect(() => { setCollapsed(value != null); }, [value]);

    // локально выбранный id до подтверждения
    const [selectedId, setSelectedId] = useState<string | null>(value?.id ?? null);
    useEffect(() => { setSelectedId(value?.id ?? null); }, [value?.id]);

    // строка поиска
    const [q, setQ] = useState<string>("");

    // фильтрация и сортировка
    const items = useMemo(() => {
        const qq = norm(q).trim();
        const scored = source.map(a => {
            const hay = `${a.title} ${a.subtitle ?? ""} ${a.python_id} ${a.id}`.toLowerCase();
            let score = 0;
            if (qq.length) {
                if (hay.includes(qq)) score += 10;
                if (a.python_id.toLowerCase().startsWith(qq)) score += 5;
                if (a.title.toLowerCase().startsWith(qq)) score += 5;
            }
            // лёгкий приоритет по title
            return { a, score };
        });
        scored.sort((x, y) => y.score - x.score || x.a.title.localeCompare(y.a.title));
        return scored.map(s => s.a);
    }, [source, q]);

    const current = useMemo(
        () => (selectedId ? items.find(a => a.id === selectedId) ?? source.find(a => a.id === selectedId) ?? null : null),
        [items, source, selectedId]
    );

    const handleConfirm = useCallback(() => {
        if (!current) return;
        onChange(current);
        setCollapsed(true);
    }, [current, onChange]);

    const handleEdit = useCallback(() => {
        setCollapsed(false);
    }, []);

    // клавиатурная навигация по списку
    const listRef = useRef<HTMLDivElement>(null);
    const moveSelection = (dir: 1 | -1) => {
        const idx = items.findIndex(a => a.id === selectedId);
        const nextIdx = Math.min(items.length - 1, Math.max(0, idx < 0 ? 0 : idx + dir));
        const next = items[nextIdx];
        if (next) setSelectedId(next.id);
    };

    if (collapsed) {
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
                <span className="text-textDim">Алгоритм:</span>
                <span className="font-medium">
          {value ? value.title : "—"}
        </span>
                <span className="ml-2 text-xs text-textDim">{value?.python_id}</span>
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
            <div className="mb-2 text-xs font-semibold text-textDim">Выбор алгоритма</div>

            {/* Поиск */}
            <label htmlFor={`${id}-alg-q`} className="sr-only">Поиск алгоритма</label>
            <input
                id={`${id}-alg-q`}
                type="text"
                placeholder="Поиск: название, python_id, id…"
                value={q}
                onChange={e => setQ(e.target.value)}
                disabled={disabled}
                className="mb-2 w-full rounded-md border border-border bg-surface px-2 py-1 text-sm outline-none focus:ring-1 focus:ring-primary/50"
                onKeyDown={(e) => {
                    if (e.key === "ArrowDown") { e.preventDefault(); moveSelection(1); }
                    if (e.key === "ArrowUp") { e.preventDefault(); moveSelection(-1); }
                    if (e.key === "Enter") { e.preventDefault(); handleConfirm(); }
                    if (e.key === "Escape") { e.preventDefault(); setCollapsed(true); }
                }}
            />

            {/* Список */}
            <div
                role="listbox"
                aria-label="Список алгоритмов"
                ref={listRef}
                className="max-h-64 overflow-auto rounded-md border border-border"
            >
                {items.map(a => {
                    const active = a.id === selectedId;
                    return (
                        <button
                            key={a.id}
                            role="option"
                            aria-selected={active}
                            type="button"
                            onClick={() => setSelectedId(a.id)}
                            disabled={disabled}
                            className={[
                                "flex w-full items-start gap-2 border-b border-border px-3 py-2 text-left last:border-b-0",
                                active ? "bg-primary/15 ring-1 ring-primary/40" : "hover:bg-surface"
                            ].join(" ")}
                            title={a.python_id}
                        >
                            <div className="mt-0.5 h-2 w-2 shrink-0 rounded-full bg-primary/80" />
                            <div className="min-w-0">
                                <div className="truncate font-medium">{a.title}</div>
                                {a.subtitle && <div className="truncate text-xs text-textDim">{a.subtitle}</div>}
                                <div className="truncate text-[11px] text-textDim/80">{a.python_id}</div>
                            </div>
                        </button>
                    );
                })}
                {items.length === 0 && (
                    <div className="px-3 py-2 text-xs text-textDim">Ничего не найдено.</div>
                )}
            </div>

            {/* Действия */}
            <div className="mt-3 flex items-center gap-2 text-[11px]">
        <span className="text-textDim">
          {current ? `Выбрано: ${current.title}` : "Не выбрано"}
        </span>
                <button
                    type="button"
                    onClick={handleConfirm}
                    disabled={disabled || !current}
                    className="ml-auto rounded-md border border-border px-3 py-1 text-xs disabled:opacity-50"
                >
                    Подтвердить
                </button>
            </div>
        </div>
    );
}
