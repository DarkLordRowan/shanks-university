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
                    className="group flex items-center gap-2 rounded-xl2
               bg-panel/80 backdrop-blur-sm px-3 py-2 shadow-panel
               transition-[box-shadow,background-color]
               hover:bg-panel/90
               focus-within:shadow-[0_0_24px_rgba(59,130,246,0.18)]">
                    <button
                        type="button"
                        aria-haspopup="listbox"
                        aria-expanded={open}
                        aria-controls={listboxId}
                        onClick={() => {
                            setOpen(v => !v);
                            requestAnimationFrame(() => inputRef.current?.focus());
                        }}
                        className="shrink-0 rounded-md p-1
                 text-textDim/70 hover:text-primary
                 bg-transparent border-0 shadow-none outline-none"
                        title="Открыть список"
                    >
                        <svg xmlns="http://www.w3.org/2000/svg"
                             viewBox="0 0 20 20"
                             fill="currentColor"
                             className="h-4 w-4">
                            <path fillRule="evenodd"
                                  d="M5.23 7.21a.75.75 0 0 1 1.06.02L10 10.94l3.71-3.71a.75.75 0 1 1 1.06 1.06l-4.24 4.25a.75.75 0 0 1-1.06 0L5.25 8.27a.75.75 0 0 1-.02-1.06z"
                                  clipRule="evenodd" />
                        </svg>
                    </button>

                    <input
                        ref={inputRef}
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                        onKeyDown={onKeyDown}
                        onFocus={() => setOpen(true)}
                        placeholder={placeholder}
                        className="w-full bg-transparent text-sm text-textDim
                 placeholder:text-textDim/45 transition-colors
                 border-0 focus:border-0
                 ring-0 focus:ring-0
                 outline-none focus:outline-none focus-visible:outline-none
                 appearance-none shadow-none"
                        role="combobox"
                        aria-autocomplete="list"
                        aria-controls={listboxId}
                        aria-expanded={open}
                        aria-activedescendant={
                            open && activeIndex >= 0 ? `${listboxId}-opt-${activeIndex}` : undefined
                        }
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
                    className="w-full rounded-xl2 bg-panel/85 px-3 py-2 text-left
               shadow-panel hover:bg-panel/90
               focus:outline-none
               focus:shadow-[0_0_24px_rgba(59,130,246,0.18)]"
                    title="Изменить выбор"
                >
                    {renderSelected
                        ? renderSelected(value!)
                        : renderOption(value!, { active: false, selected: true })}
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
    function ComboboxRow(
        {
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
