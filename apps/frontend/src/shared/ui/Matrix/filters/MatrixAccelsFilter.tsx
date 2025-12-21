// src/shared/ui/Matrix/filters/MatrixAccelsFilter.tsx

import React from "react";
import type { Accel } from "@/entities/experiment/model/experiment";
import { MatrixAxisFilter, type FilterMode, type Group } from "./MatrixAxisFilter";

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

    /** real filters: m */
    mMinText: string;
    mMaxText: string;
    onMMinText: (v: string) => void;
    onMMaxText: (v: string) => void;

    /** real filters: args key/value */
    argKey: string;
    argValue: string;
    onArgKey: (v: string) => void;
    onArgValue: (v: string) => void;

    onResetParams: () => void;
}

export function MatrixAccelsFilter(props: MatrixAccelsFilterProps) {
    const {
        mMinText,
        mMaxText,
        onMMinText,
        onMMaxText,
        argKey,
        argValue,
        onArgKey,
        onArgValue,
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

                    <div className="mt-2 grid grid-cols-2 gap-2">
                        <div className="flex flex-col gap-1">
                            <div className="text-[10px] text-textDim/70">args key</div>
                            <input
                                value={argKey}
                                onChange={(e) => onArgKey(e.target.value)}
                                placeholder="например alpha"
                                className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                            />
                        </div>
                        <div className="flex flex-col gap-1">
                            <div className="text-[10px] text-textDim/70">args value</div>
                            <input
                                value={argValue}
                                onChange={(e) => onArgValue(e.target.value)}
                                placeholder="например 0.5"
                                className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                            />
                        </div>
                    </div>
                </div>
            }
        />
    );
}
