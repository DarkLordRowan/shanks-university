import React, { memo, useCallback, useEffect, useId, useMemo, useRef, useState, } from "react";

export type ComboboxProps<T> = {
    items: T[];
    value: T | null;
    onChange: (next: T) => void;

    // Проекции
    getKey: (x: T) => string;
    getSearchText: (x: T) => string;
    renderOption: (x: T, state: { active: boolean; selected: boolean }) => React.ReactNode;
    renderSelected?: (x: T) => React.ReactNode;

    // Внешний вид / а11y
    placeholder?: string;
    className?: string;
};

export function Combobox<T>({
                                items,
                                value,
                                onChange,
                                getKey,
                                getSearchText,
                                renderOption,
                                renderSelected,
                                placeholder = "Select…",
                                className,
                            }: ComboboxProps<T>) {
    const [open, setOpen] = useState(false);
    const [query, setQuery] = useState("");
    const [activeIndex, setActiveIndex] = useState(-1);

    const inputRef = useRef<HTMLInputElement | null>(null);
    const listRef = useRef<HTMLUListElement | null>(null);
    const rootRef = useRef<HTMLDivElement | null>(null);
    const listboxId = useId();

    const q = useMemo(() => query.trim().toLowerCase(), [query]);
    const filtered = useMemo(() => {
        if (!q) return items;
        return items.filter(x => getSearchText(x).toLowerCase().includes(q));
    }, [items, q, getSearchText]);

    useEffect(() => {
        if (filtered.length === 0) {
            setActiveIndex(-1);
            return;
        }
        setActiveIndex(i => (i < 0 || i >= filtered.length ? 0 : i));
    }, [filtered.length]);

    useEffect(() => {
        const onOutside = (e: PointerEvent) => {
            if (!rootRef.current?.contains(e.target as Node)) setOpen(false);
        };
        document.addEventListener("pointerdown", onOutside);
        return () => document.removeEventListener("pointerdown", onOutside);
    }, []);

    useEffect(() => {
        if (!open) return;
        const el = listRef.current?.children[activeIndex] as HTMLElement | undefined;
        el?.scrollIntoView({block: "nearest"});
    }, [activeIndex, open]);

    const commit = useCallback((index: number) => {
        const pick = filtered[index];
        if (!pick) return;
        onChange(pick);
        setQuery("");
        setOpen(false);
    }, [filtered, onChange]);

    const onKeyDown = useCallback((e: React.KeyboardEvent<HTMLInputElement>) => {
        if (!open && (e.key === "ArrowDown" || e.key === "ArrowUp")) {
            setOpen(true);
            requestAnimationFrame(() => setActiveIndex(0));
            return;
        }
        if (!open) return;

        if (e.key === "ArrowDown") {
            e.preventDefault();
            setActiveIndex(i => Math.min(i + 1, filtered.length - 1));
        } else if (e.key === "ArrowUp") {
            e.preventDefault();
            setActiveIndex(i => Math.max(i - 1, 0));
        } else if (e.key === "Enter") {
            e.preventDefault();
            commit(activeIndex);
        } else if (e.key === "Escape") {
            setOpen(false);
        }
    }, [open, filtered.length, commit, activeIndex]);

    const selectedKey = value ? getKey(value) : null;
    const showSearch = open || !value || query.length > 0;

    return (
        <div ref={rootRef} className={`relative w-full ${className ?? ""}`}>
            {showSearch ? (
                <div
                    className="group flex items-center gap-2 rounded-xl2 border border-border bg-panel px-3 py-2 shadow-panel focus-within:ring-2 focus-within:ring-primary">
                    <button
                        type="button"
                        aria-haspopup="listbox"
                        aria-expanded={open}
                        aria-controls={listboxId}
                        onClick={() => {
                            setOpen(v => !v);
                            requestAnimationFrame(() => inputRef.current?.focus());
                        }}
                        className="shrink-0 rounded-md border border-transparent px-2 py-1 text-textDim hover:border-border hover:bg-surface"
                        title="Открыть список"
                    >
                        ▾
                    </button>
                    <input
                        ref={inputRef}
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                        onKeyDown={onKeyDown}
                        onFocus={() => setOpen(true)}
                        placeholder={placeholder}
                        className="w-full bg-transparent text-sm text-textDim placeholder:text-textDim/60 focus:outline-none"
                        role="combobox"
                        aria-autocomplete="list"
                        aria-controls={listboxId}
                        aria-expanded={open}
                        aria-activedescendant={open && activeIndex >= 0 ? `${listboxId}-opt-${activeIndex}` : undefined}
                    />
                </div>
            ) : (
                <button
                    type="button"
                    onClick={() => {
                        setOpen(true);
                        requestAnimationFrame(() => inputRef.current?.focus());
                    }}
                    aria-haspopup="listbox"
                    aria-expanded={open}
                    aria-controls={listboxId}
                    className="w-full rounded-xl2 border border-border bg-panel px-3 py-2 text-left shadow-panel hover:bg-panel/90 focus:outline-none focus:ring-2 focus:ring-primary"
                    title="Изменить выбор"
                >
                    {renderSelected ? renderSelected(value!) : renderOption(value!, {active: false, selected: true})}
                </button>
            )}

            {open && (
                <ul
                    ref={listRef}
                    id={listboxId}
                    role="listbox"
                    className="absolute z-50 mt-2 max-h-72 w-full overflow-auto rounded-xl2 border border-border bg-panel/95 backdrop-blur-sm shadow-panel"
                >
                    {filtered.length === 0 && (
                        <li className="px-3 py-2 text-sm text-textDim/70">Ничего не найдено</li>
                    )}

                    {filtered.map((item, i) => {
                        const active = i === activeIndex;
                        const selected = selectedKey != null && getKey(item) === selectedKey;
                        return (
                            <li
                                id={`${listboxId}-opt-${i}`}
                                key={getKey(item)}
                                role="option"
                                aria-selected={selected}
                                onMouseEnter={() => setActiveIndex(i)}
                                onPointerDown={(e) => {
                                    e.preventDefault();
                                    commit(i);
                                }}
                                className={[
                                    "cursor-pointer px-3 py-2 text-sm",
                                    active ? "bg-primary/15" : "",
                                    selected ? "outline outline-1 outline-primary/60" : "",
                                ].join(" ")}
                            >
                                {renderOption(item, {active, selected})}
                            </li>
                        );
                    })}
                </ul>
            )}
        </div>
    );
}

export const ComboboxRow = memo(
    function ComboboxRow
    ({
         left,
         right,
         sub,
     }: {
        left: React.ReactNode;
        right?: React.ReactNode;
        sub?: React.ReactNode
    }) {
        return (
            <div className="flex items-start justify-between gap-3">
                <div className="min-w-0">
                    <div className="truncate font-medium text-textDim">{left}</div>
                    {sub && <div className="truncate text-xs text-textDim/70">{sub}</div>}
                </div>
                {right}
            </div>
        );
    }
);
