import React, { memo, useCallback, useEffect, useId, useMemo, useRef, useState, } from "react";

export type ComboboxProps<T> = {
    items: T[];
    value: T | null;
    onChange: (next: T) => void;

    getKey: (x: T) => string;
    getSearchText: (x: T) => string;

    renderOption: (x: T, state: { active: boolean; selected: boolean }) => React.ReactNode;
    renderSelected?: (x: T) => React.ReactNode;

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
    const [isOpen, setIsOpen] = useState(false);
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
            if (!rootRef.current?.contains(e.target as Node)) setIsOpen(false);
        };
        document.addEventListener("pointerdown", onOutside);
        return () => document.removeEventListener("pointerdown", onOutside);
    }, []);

    useEffect(() => {
        if (!isOpen) return;
        const el = listRef.current?.children[activeIndex] as HTMLElement | undefined;
        el?.scrollIntoView({block: "nearest"});
    }, [activeIndex, isOpen]);

    const commit = useCallback((index: number) => {
        const pick = filtered[index];
        if (!pick) return;
        onChange(pick);
        setQuery("");
        setIsOpen(false);
    }, [filtered, onChange]);

    const onKeyDown = useCallback((e: React.KeyboardEvent<HTMLInputElement>) => {
        if (!isOpen && (e.key === "ArrowDown" || e.key === "ArrowUp")) {
            setIsOpen(true);
            requestAnimationFrame(() => setActiveIndex(0));
            return;
        }
        if (!isOpen) return;

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
            setIsOpen(false);
        }
    }, [isOpen, filtered.length, commit, activeIndex]);

    const selectedKey = value ? getKey(value) : null;
    const showSearch = isOpen || !value || query.length > 0;

    return (
        <div ref={rootRef} className={`relative w-full ${className ?? ""}`}>
            {showSearch ? (
                <HeaderSearch
                    inputRef={inputRef}
                    listboxId={listboxId}
                    isOpen={isOpen}
                    placeholder={placeholder}
                    query={query}
                    setQuery={setQuery}
                    setIsOpen={setIsOpen}
                    onKeyDown={onKeyDown}
                />
            ) : (
                <HeaderSelected
                    value={value!}
                    renderSelected={renderSelected}
                    renderOption={renderOption}
                    setIsOpen={setIsOpen}
                    inputRef={inputRef}
                    listboxId={listboxId}
                />
            )}

            {isOpen && (
                <OptionsList<T>
                    listRef={listRef}
                    listboxId={listboxId}
                    items={filtered}
                    getKey={getKey}
                    selectedKey={selectedKey}
                    activeIndex={activeIndex}
                    setActiveIndex={setActiveIndex}
                    onPick={commit}
                    renderOption={renderOption}
                />
            )}
        </div>
    );
}

type HeaderSearchProps = {
    inputRef: React.RefObject<HTMLInputElement>;
    listboxId: string;
    isOpen: boolean;
    placeholder: string;
    query: string;
    setQuery: (s: string) => void;
    setIsOpen: (v: boolean) => void;
    onKeyDown: (e: React.KeyboardEvent<HTMLInputElement>) => void;
};

const HeaderSearch = ({
                          inputRef,
                          listboxId,
                          isOpen,
                          placeholder,
                          query,
                          setQuery,
                          setIsOpen,
                          onKeyDown,
                      }: HeaderSearchProps) => (
    <div
        className="group flex items-center gap-2 rounded-xl2
               bg-panel/80 backdrop-blur-sm px-3 py-2 shadow-panel
               transition-[box-shadow,background-color]
               hover:bg-panel/90
               focus-within:shadow-[0_0_24px_rgba(59,130,246,0.18)]">

        <input
            ref={inputRef}
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            onKeyDown={onKeyDown}
            onFocus={() => setIsOpen(true)}
            placeholder={placeholder}
            className="w-full bg-transparent text-sm text-textDim
                 placeholder:text-textDim/45 transition-colors
                 border-0 focus:border-0 ring-0 focus:ring-0
                 outline-none focus:outline-none focus-visible:outline-none
                 appearance-none shadow-none"
            role="combobox"
            aria-autocomplete="list"
            aria-controls={listboxId}
            aria-expanded={isOpen}
        />
    </div>
);

type HeaderSelectedProps<T> = {
    value: T;
    renderSelected?: (x: T) => React.ReactNode;
    renderOption: (x: T, s: { active: boolean; selected: boolean }) => React.ReactNode;
    setIsOpen: (v: boolean) => void;
    inputRef: React.RefObject<HTMLInputElement>;
    listboxId: string;
};

function HeaderSelected<T>({
                               value,
                               renderSelected,
                               renderOption,
                               setIsOpen,
                               inputRef,
                               listboxId,
                           }: HeaderSelectedProps<T>) {
    return (
        <button
            type="button"
            onClick={() => {
                setIsOpen(true);
                requestAnimationFrame(() => inputRef.current?.focus());
            }}
            aria-haspopup="listbox"
            aria-controls={listboxId}
            className="w-full rounded-xl2 bg-panel/85 px-3 py-2 text-left
                 shadow-panel hover:bg-panel/90
                 focus:outline-none
                 focus:shadow-[0_0_24px_rgba(59,130,246,0.18)]"
            title="Изменить выбор"
        >
            {renderSelected
                ? renderSelected(value)
                : renderOption(value, {active: false, selected: true})}
        </button>
    );
}

type OptionsListProps<T> = {
    listRef: React.RefObject<HTMLUListElement>;
    listboxId: string;
    items: T[];
    getKey: (x: T) => string;
    selectedKey: string | null;
    activeIndex: number;
    setActiveIndex: (i: number) => void;
    onPick: (index: number) => void;
    renderOption: (x: T, s: { active: boolean; selected: boolean }) => React.ReactNode;
};

function OptionsList<T>({
                            listRef,
                            listboxId,
                            items,
                            getKey,
                            selectedKey,
                            activeIndex,
                            setActiveIndex,
                            onPick,
                            renderOption,
                        }: OptionsListProps<T>) {
    return (
        <ul
            ref={listRef}
            id={listboxId}
            role="listbox"
            className="absolute z-50 mt-2 max-h-72 w-full overflow-auto rounded-xl2
                 border border-border bg-panel/95 backdrop-blur-sm"
        >
            {items.length === 0 && (
                <li className="px-3 py-2 text-sm text-textDim/70">Ничего не найдено</li>
            )}

            {items.map((item, i) => {
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
                            onPick(i);
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
    );
}

export const ComboboxRow = memo(
    function ComboboxRow(
        {
            left, right, sub,
        }:
        { left: React.ReactNode; right?: React.ReactNode; sub?: React.ReactNode }) {
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
