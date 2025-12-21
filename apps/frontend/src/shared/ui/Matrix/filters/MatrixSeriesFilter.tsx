// src/shared/ui/Matrix/filters/MatrixSeriesFilter.tsx

import type { Series } from "@/entities/experiment/model/experiment";
import { MatrixAxisFilter, type FilterMode, type Group } from "./MatrixAxisFilter";
import type { ArgsOp, ArgClause } from "./MatrixAccelsFilter"; // можно вынести типы в общий файл при желании

export interface MatrixSeriesFilterProps {
    query: string;
    onQuery: (v: string) => void;

    groups: Group<Series>[];

    groupMode: FilterMode;
    onGroupMode: (m: FilterMode) => void;

    selectedGroupKeys: Set<string>;
    onToggleGroup: (key: string) => void;
    onSelectAllGroups: () => void;
    onClearGroups: () => void;

    // precision
    precisionOptions: string[];
    precisionMode: FilterMode;
    onPrecisionMode: (m: FilterMode) => void;

    selectedPrecisions: Set<string>;
    onTogglePrecision: (p: string) => void;
    onSelectAllPrecisions: () => void;
    onClearPrecisions: () => void;

    // args clauses
    argsOp: ArgsOp;
    onArgsOp: (op: ArgsOp) => void;
    argClauses: ArgClause[];
    onChangeClause: (index: number, patch: Partial<ArgClause>) => void;
    onAddClause: () => void;
    onRemoveClause: (index: number) => void;

    onResetParams: () => void;
}

export function MatrixSeriesFilter(props: MatrixSeriesFilterProps) {
    const {
        precisionOptions,
        precisionMode,
        onPrecisionMode,
        selectedPrecisions,
        onTogglePrecision,
        onSelectAllPrecisions,
        onClearPrecisions,
        argsOp,
        onArgsOp,
        argClauses,
        onChangeClause,
        onAddClause,
        onRemoveClause,
        onResetParams,
        ...axisProps
    } = props;

    return (
        <MatrixAxisFilter<Series>
            title="Ряды"
            hint="группировка по name"
            {...axisProps}
            extra={
                <div className="rounded border border-border bg-panel/30 p-2">
                    <div className="flex items-center justify-between">
                        <div className="text-[10px] font-semibold text-textDim/80">параметры</div>
                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-2 py-[2px] text-[10px] hover:bg-panel"
                            onClick={onResetParams}
                        >
                            reset
                        </button>
                    </div>

                    {/* precision (явно отделено от групп) */}
                    <div className="mt-2 flex items-center gap-2 text-[10px] text-textDim">
                        <span className="text-textDim/70">precision filter:</span>

                        <button
                            type="button"
                            className={`rounded border border-border px-2 py-[2px] ${
                                precisionMode === "whitelist"
                                    ? "bg-panel"
                                    : "bg-surface hover:bg-panel"
                            }`}
                            onClick={() => onPrecisionMode("whitelist")}
                            title="включать только выбранные precision"
                        >
                            include
                        </button>

                        <button
                            type="button"
                            className={`rounded border border-border px-2 py-[2px] ${
                                precisionMode === "blacklist"
                                    ? "bg-panel"
                                    : "bg-surface hover:bg-panel"
                            }`}
                            onClick={() => onPrecisionMode("blacklist")}
                            title="исключать выбранные precision"
                        >
                            exclude
                        </button>

                        <button
                            type="button"
                            className="ml-auto rounded border border-border bg-surface px-2 py-[2px] hover:bg-panel"
                            onClick={onSelectAllPrecisions}
                        >
                            all
                        </button>

                        <button
                            type="button"
                            className="rounded border border-border bg-surface px-2 py-[2px] hover:bg-panel"
                            onClick={onClearPrecisions}
                        >
                            none
                        </button>
                    </div>

                    <div className="mt-2 max-h-[120px] overflow-auto rounded border border-border bg-panel/40 p-1">
                        {precisionOptions.length === 0 ? (
                            <div className="px-2 py-2 text-[11px] text-textDim/70">
                                нет precision
                            </div>
                        ) : (
                            precisionOptions.map((p) => {
                                const checked = selectedPrecisions.has(p);
                                return (
                                    <label
                                        key={p}
                                        className="flex cursor-pointer items-center gap-2 rounded px-2 py-1 text-[11px] text-textDim hover:bg-panel"
                                    >
                                        <input
                                            type="checkbox"
                                            checked={checked}
                                            onChange={() => onTogglePrecision(p)}
                                        />
                                        <span className="flex-1 truncate">{p}</span>
                                    </label>
                                );
                            })
                        )}
                    </div>

                    {/* args AND/OR */}
                    <div className="mt-3 flex items-center gap-2 text-[10px] text-textDim">
                        <span className="text-textDim/70">args filter:</span>

                        <button
                            type="button"
                            className={`rounded border border-border px-2 py-[2px] ${
                                argsOp === "and" ? "bg-panel" : "bg-surface hover:bg-panel"
                            }`}
                            onClick={() => onArgsOp("and")}
                            title="все условия должны выполниться"
                        >
                            AND
                        </button>

                        <button
                            type="button"
                            className={`rounded border border-border px-2 py-[2px] ${
                                argsOp === "or" ? "bg-panel" : "bg-surface hover:bg-panel"
                            }`}
                            onClick={() => onArgsOp("or")}
                            title="достаточно одного условия"
                        >
                            OR
                        </button>

                        <button
                            type="button"
                            className="ml-auto rounded border border-border bg-surface px-2 py-[2px] hover:bg-panel"
                            onClick={onAddClause}
                        >
                            + rule
                        </button>
                    </div>

                    <div className="mt-2 flex flex-col gap-2">
                        {argClauses.map((c, i) => (
                            <div key={i} className="grid grid-cols-[1fr_1fr_auto] gap-2">
                                <input
                                    value={c.key}
                                    onChange={(e) => onChangeClause(i, { key: e.target.value })}
                                    placeholder="args key (можно пусто)"
                                    className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                                />
                                <input
                                    value={c.value}
                                    onChange={(e) => onChangeClause(i, { value: e.target.value })}
                                    placeholder="args value"
                                    className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                                />
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-2 py-1 text-[11px] hover:bg-panel"
                                    onClick={() => onRemoveClause(i)}
                                    title="удалить правило"
                                >
                                    ×
                                </button>
                            </div>
                        ))}
                        <div className="text-[10px] text-textDim/60">
                            key пустой ⇒ ищем value по любому аргументу.
                        </div>
                    </div>
                </div>
            }
        />
    );
}
