// src/shared/ui/Matrix/filters/MatrixSeriesFilter.tsx

import React from "react";
import type { Series } from "@/entities/experiment/model/experiment";
import { MatrixAxisFilter, type FilterMode, type Group } from "./MatrixAxisFilter";

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

    /** real filters: precision */
    precisionOptions: string[];
    precisionMode: FilterMode;
    onPrecisionMode: (m: FilterMode) => void;

    selectedPrecisions: Set<string>;
    onTogglePrecision: (p: string) => void;
    onSelectAllPrecisions: () => void;
    onClearPrecisions: () => void;

    /** real filters: args key/value */
    argKey: string;
    argValue: string;
    onArgKey: (v: string) => void;
    onArgValue: (v: string) => void;

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
        argKey,
        argValue,
        onArgKey,
        onArgValue,
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

                    <div className="mt-2 flex items-center gap-2 text-[10px] text-textDim">
                        <span className="text-textDim/70">precision:</span>

                        <button
                            type="button"
                            className={`rounded border border-border px-2 py-[2px] ${
                                precisionMode === "whitelist"
                                    ? "bg-panel"
                                    : "bg-surface hover:bg-panel"
                            }`}
                            onClick={() => onPrecisionMode("whitelist")}
                            title="Показывать только выбранные precision"
                        >
                            only
                        </button>

                        <button
                            type="button"
                            className={`rounded border border-border px-2 py-[2px] ${
                                precisionMode === "blacklist"
                                    ? "bg-panel"
                                    : "bg-surface hover:bg-panel"
                            }`}
                            onClick={() => onPrecisionMode("blacklist")}
                            title="Скрывать выбранные precision"
                        >
                            hide
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

                    <div className="mt-2 grid grid-cols-2 gap-2">
                        <div className="flex flex-col gap-1">
                            <div className="text-[10px] text-textDim/70">args key</div>
                            <input
                                value={argKey}
                                onChange={(e) => onArgKey(e.target.value)}
                                placeholder="например a"
                                className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                            />
                        </div>
                        <div className="flex flex-col gap-1">
                            <div className="text-[10px] text-textDim/70">args value</div>
                            <input
                                value={argValue}
                                onChange={(e) => onArgValue(e.target.value)}
                                placeholder="например 1e-6"
                                className="w-full rounded border border-border bg-panel px-2 py-1 text-[11px] text-textDim outline-none"
                            />
                        </div>
                    </div>
                </div>
            }
        />
    );
}
