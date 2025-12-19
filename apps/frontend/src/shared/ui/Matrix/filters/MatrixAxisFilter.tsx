export type FilterMode = "whitelist" | "blacklist";

export type Group<T> = {
    key: string;
    title: string;
    items: T[];
};

export interface MatrixAxisFilterProps<T> {
    title: string;
    hint?: string;

    query: string;
    onQuery: (v: string) => void;

    groups: Group<T>[];

    groupMode: FilterMode;
    onGroupMode: (m: FilterMode) => void;

    selectedGroupKeys: Set<string>;
    onToggleGroup: (key: string) => void;
    onSelectAllGroups: () => void;
    onClearGroups: () => void;

    idMode: FilterMode;
    onIdMode: (m: FilterMode) => void;

    selectedIds: Set<string>;
    onClearIds: () => void;
}

export function MatrixAxisFilter<T>(props: MatrixAxisFilterProps<T>) {
    const {
        title,
        hint,
        query,
        onQuery,
        groups,
        groupMode,
        onGroupMode,
        selectedGroupKeys,
        onToggleGroup,
        onSelectAllGroups,
        onClearGroups,
        idMode,
        onIdMode,
        selectedIds,
        onClearIds,
    } = props;

    return (
        <div className="rounded-xl2 border border-border bg-surface p-2">
            <div className="flex items-center justify-between">
                <div className="text-[11px] font-semibold text-textDim">{title}</div>
                {hint ? <div className="text-[10px] text-textDim/70">{hint}</div> : null}
            </div>

            <div className="mt-2 flex items-center gap-2">
                <input
                    value={query}
                    onChange={(e) => onQuery(e.target.value)}
                    placeholder="поиск…"
                    className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                />
            </div>

            <div className="mt-2 flex items-center gap-2 text-[10px] text-textDim">
                <span className="text-textDim/70">группы:</span>

                <button
                    type="button"
                    className={`rounded border border-border px-2 py-[2px] ${
                        groupMode === "whitelist" ? "bg-panel" : "bg-surface hover:bg-panel"
                    }`}
                    onClick={() => onGroupMode("whitelist")}
                    title="Показывать только выбранные группы"
                >
                    only
                </button>

                <button
                    type="button"
                    className={`rounded border border-border px-2 py-[2px] ${
                        groupMode === "blacklist" ? "bg-panel" : "bg-surface hover:bg-panel"
                    }`}
                    onClick={() => onGroupMode("blacklist")}
                    title="Скрывать выбранные группы"
                >
                    hide
                </button>

                <button
                    type="button"
                    className="ml-auto rounded border border-border bg-surface px-2 py-[2px] hover:bg-panel"
                    onClick={onSelectAllGroups}
                >
                    all
                </button>

                <button
                    type="button"
                    className="rounded border border-border bg-surface px-2 py-[2px] hover:bg-panel"
                    onClick={onClearGroups}
                >
                    none
                </button>
            </div>

            <div className="mt-2 max-h-[160px] overflow-auto rounded border border-border bg-panel/40 p-1">
                {groups.map((g) => {
                    const checked = selectedGroupKeys.has(g.key);
                    return (
                        <label
                            key={g.key}
                            className="flex cursor-pointer items-center gap-2 rounded px-2 py-1 text-[11px] text-textDim hover:bg-panel"
                        >
                            <input
                                type="checkbox"
                                checked={checked}
                                onChange={() => onToggleGroup(g.key)}
                            />
                            <span className="flex-1 truncate">{g.title}</span>
                            <span className="tabular-nums text-[10px] text-textDim/70">
                                {g.items.length}
                            </span>
                        </label>
                    );
                })}
            </div>

            <div className="mt-2 flex items-center gap-2 text-[10px] text-textDim">
                <span className="text-textDim/70">ids:</span>

                <button
                    type="button"
                    className={`rounded border border-border px-2 py-[2px] ${
                        idMode === "whitelist" ? "bg-panel" : "bg-surface hover:bg-panel"
                    }`}
                    onClick={() => onIdMode("whitelist")}
                    title="Показывать только выбранные id"
                >
                    only
                </button>

                <button
                    type="button"
                    className={`rounded border border-border px-2 py-[2px] ${
                        idMode === "blacklist" ? "bg-panel" : "bg-surface hover:bg-panel"
                    }`}
                    onClick={() => onIdMode("blacklist")}
                    title="Скрывать выбранные id"
                >
                    hide
                </button>

                <button
                    type="button"
                    className="ml-auto rounded border border-border bg-surface px-2 py-[2px] hover:bg-panel disabled:opacity-50"
                    onClick={onClearIds}
                    disabled={selectedIds.size === 0}
                    title="Очистить id-выбор"
                >
                    clear
                </button>

                {selectedIds.size > 0 ? (
                    <span className="tabular-nums text-textDim/70">{selectedIds.size}</span>
                ) : null}
            </div>
        </div>
    );
}
