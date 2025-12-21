// src/shared/ui/Matrix/filters/MatrixAccelsFilter.tsx

import type { Accel } from "@/entities/experiment/model/experiment";
import { MatrixAxisFilter, type FilterMode, type Group } from "./MatrixAxisFilter";

export type ArgsOp = "and" | "or";

export type ArgClause = {
    key: string;
    value: string;
};

export interface MatrixAccelsFilterProps {
    query: string;
    onQuery: (v: string) => void;

    groups: Group<Accel>[];

    groupMode: FilterMode;
    onGroupMode: (m: FilterMode) => void;

    selectedGroupKeys: Set<string>;
    onToggleGroup: (key: string) => void;
    onSelectAllGroups: () => void;
    onClearGroups: () => void;

    // m filter
    mMinText: string;
    mMaxText: string;
    onMMinText: (v: string) => void;
    onMMaxText: (v: string) => void;

    // args clauses
    argsOp: ArgsOp;
    onArgsOp: (op: ArgsOp) => void;
    argClauses: ArgClause[];
    onChangeClause: (index: number, patch: Partial<ArgClause>) => void;
    onAddClause: () => void;
    onRemoveClause: (index: number) => void;

    onResetParams: () => void;
}

export function MatrixAccelsFilter(props: MatrixAccelsFilterProps) {
    const {
        mMinText,
        mMaxText,
        onMMinText,
        onMMaxText,
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
        <MatrixAxisFilter<Accel>
            title="Алгоритмы"
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

                    {/* m */}
                    <div className="mt-2 grid grid-cols-2 gap-2">
                        <div className="flex flex-col gap-1">
                            <div className="text-[10px] text-textDim/70">m min</div>
                            <input
                                value={mMinText}
                                onChange={(e) => onMMinText(e.target.value)}
                                placeholder="например 2"
                                className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                            />
                        </div>
                        <div className="flex flex-col gap-1">
                            <div className="text-[10px] text-textDim/70">m max</div>
                            <input
                                value={mMaxText}
                                onChange={(e) => onMMaxText(e.target.value)}
                                placeholder="например 10"
                                className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                            />
                        </div>
                    </div>

                    {/* args: AND/OR + multiple clauses */}
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
