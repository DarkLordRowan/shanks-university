import { applyItemsFilter, buildFilterOptions, type ItemsFilterState } from "@/utils/filters";
import React, { useMemo, useState } from "react";
import type { Item } from "@/types/item.ts";

interface ItemsFilterPanelProps {
    items: Item[];
    onChange(filtered: Item[]): void;
    className?: string;
}

function toggleInArray<T>(arr: T[], value: T): T[] {
    return arr.includes(value)
        ? arr.filter((v) => v !== value)
        : [...arr, value];
}

const EMPTY_FILTER: ItemsFilterState = {
    seriesNames: [],
    algorithmNames: [],
    mValues: [],
    xMin: undefined,
    xMax: undefined,
    hasError: null,
};

export const ItemsFilterPanel: React.FC<ItemsFilterPanelProps> = ({
                                                                      items,
                                                                      onChange,
                                                                      className,
                                                                  }) => {
    const options = useMemo(() => buildFilterOptions(items), [items]);

    // Черновой фильтр (UI)
    const [draftFilter, setDraftFilter] = useState<ItemsFilterState>(EMPTY_FILTER);

    // Применённый фильтр (для отображения статуса)
    const [appliedFilter, setAppliedFilter] =
        useState<ItemsFilterState>(EMPTY_FILTER);

    const handleSeriesNameToggle = (name: string) => {
        setDraftFilter((prev) => ({
            ...prev,
            seriesNames: toggleInArray(prev.seriesNames, name),
        }));
    };

    const handleAlgorithmNameToggle = (name: string) => {
        setDraftFilter((prev) => ({
            ...prev,
            algorithmNames: toggleInArray(prev.algorithmNames, name),
        }));
    };

    const handleMToggle = (m: number | null) => {
        setDraftFilter((prev) => ({
            ...prev,
            mValues: toggleInArray(prev.mValues, m),
        }));
    };

    const handleXMinChange: React.ChangeEventHandler<HTMLInputElement> = (e) => {
        const v = e.target.value;
        setDraftFilter((prev) => ({
            ...prev,
            xMin: v === "" ? undefined : Number(v),
        }));
    };

    const handleXMaxChange: React.ChangeEventHandler<HTMLInputElement> = (e) => {
        const v = e.target.value;
        setDraftFilter((prev) => ({
            ...prev,
            xMax: v === "" ? undefined : Number(v),
        }));
    };

    const handleHasErrorChange: React.ChangeEventHandler<HTMLSelectElement> = (e) => {
        const v = e.target.value;
        let hasError: boolean | null;
        if (v === "with") hasError = true;
        else if (v === "without") hasError = false;
        else hasError = null;

        setDraftFilter((prev) => ({
            ...prev,
            hasError,
        }));
    };

    const handleReset = () => {
        setDraftFilter(EMPTY_FILTER);
        setAppliedFilter(EMPTY_FILTER);
        // снаружи очищаем фильтрованное состояние (по желанию: [] или items)
        onChange([]);
        // onChange(items); // если хочешь показывать все данные после сброса
    };

    // превью результата чернового фильтра
    const preview = useMemo(() => {
        const filtered = applyItemsFilter(items, draftFilter);

        const seriesSet = new Set<string>();
        const xSet = new Set<number>();

        for (const it of filtered) {
            seriesSet.add(it.series.seriesName);
            xSet.add(it.series.x);
        }

        return {
            count: filtered.length,
            seriesCount: seriesSet.size,
            xCount: xSet.size,
        };
    }, [items, draftFilter]);

    const previewCount = preview.count;

    const appliedCount = useMemo(
        () => applyItemsFilter(items, appliedFilter).length,
        [items, appliedFilter],
    );

    const isDirty =
        JSON.stringify(draftFilter) !== JSON.stringify(appliedFilter);

    const canApplySingleSeriesX =
        preview.seriesCount === 1 && preview.xCount === 1 && preview.count > 0;

    const canApply = isDirty && canApplySingleSeriesX;

    const handleApply = () => {
        if (!canApply) return;
        setAppliedFilter(draftFilter);
        const filtered = applyItemsFilter(items, draftFilter);
        onChange(filtered); // только здесь реально меняем filteredItems снаружи
    };

    const totalCount = items.length;

    return (
        <div
            className={
                className ??
                "rounded-xl2 border border-border bg-panel/90 shadow-panel p-4 text-xs text-textDim space-y-4"
            }
        >
            <div className="flex items-center gap-2">
                <div className="font-semibold text-sm text-white">
                    Фильтры
                </div>

                <div className="ml-auto text-[11px] text-textDim flex flex-col items-end">
                    <div>
                        Текущие:{" "}
                        <span className="text-primary">
                            {appliedCount}
                        </span>{" "}
                        / {totalCount}
                    </div>
                    <div className="text-[10px] text-textDim/70">
                        После применения: {previewCount}
                    </div>
                    {!canApplySingleSeriesX && previewCount > 0 && (
                        <div className="mt-0.5 text-[10px] text-secondary">
                            Нужно 1 ряд и 1&nbsp;x. Сейчас: рядов {preview.seriesCount},
                            x {preview.xCount}.
                        </div>
                    )}
                    {previewCount === 0 && isDirty && (
                        <div className="mt-0.5 text-[10px] text-secondary">
                            Фильтр не возвращает ни одного элемента.
                        </div>
                    )}
                </div>

                <button
                    type="button"
                    onClick={handleApply}
                    disabled={!canApply}
                    className={[
                        "ml-2 inline-flex items-center rounded-md px-2 py-1 text-[11px] transition-colors",
                        canApply
                            ? "bg-primary/80 text-white hover:bg-primary"
                            : "bg-surface/60 text-textDim/60 cursor-default",
                    ].join(" ")}
                >
                    Применить
                </button>

                <button
                    type="button"
                    onClick={handleReset}
                    className="ml-2 inline-flex items-center rounded-md border border-border px-2 py-1 text-[11px] text-textDim hover:border-primary hover:text-primary transition-colors"
                >
                    Сброс
                </button>
            </div>

            {/* seriesName */}
            <div>
                <div className="mb-1 text-[11px] font-semibold text-white/90">
                    Ряд (seriesName)
                </div>
                <div className="flex flex-wrap gap-2">
                    {options.seriesNames.map((name) => {
                        const active = draftFilter.seriesNames.includes(name);
                        return (
                            <button
                                type="button"
                                key={name}
                                onClick={() => handleSeriesNameToggle(name)}
                                className={[
                                    "inline-flex items-center rounded-full border px-2 py-1 text-[11px]",
                                    "transition-colors",
                                    active
                                        ? "border-primary bg-primary/20 text-primary"
                                        : "border-border bg-surface/50 text-textDim hover:border-primary/60 hover:text-primary/80",
                                ].join(" ")}
                            >
                                {name}
                            </button>
                        );
                    })}
                    {options.seriesNames.length === 0 && (
                        <div className="text-[11px] text-textDim/60">
                            Нет значений
                        </div>
                    )}
                </div>
            </div>

            {/* algorithmName */}
            <div>
                <div className="mb-1 text-[11px] font-semibold text-white/90">
                    Алгоритм (algorithmName)
                </div>
                <div className="flex flex-wrap gap-2">
                    {options.algorithmNames.map((name) => {
                        const active =
                            draftFilter.algorithmNames.includes(name);
                        return (
                            <button
                                type="button"
                                key={name}
                                onClick={() => handleAlgorithmNameToggle(name)}
                                className={[
                                    "inline-flex items-center rounded-full border px-2 py-1 text-[11px]",
                                    "transition-colors",
                                    active
                                        ? "border-secondary bg-secondary/20 text-secondary"
                                        : "border-border bg-surface/50 text-textDim hover:border-secondary/60 hover:text-secondary/80",
                                ].join(" ")}
                            >
                                {name}
                            </button>
                        );
                    })}
                    {options.algorithmNames.length === 0 && (
                        <div className="text-[11px] text-textDim/60">
                            Нет значений
                        </div>
                    )}
                </div>
            </div>

            {/* m */}
            <div>
                <div className="mb-1 text-[11px] font-semibold text-white/90">
                    Параметр m
                </div>
                <div className="flex flex-wrap gap-2">
                    {options.mValues.map((m) => {
                        const label = m === null ? "m = ∅" : `m = ${m}`;
                        const active = draftFilter.mValues.includes(m);
                        return (
                            <button
                                type="button"
                                key={m === null ? "null" : String(m)}
                                onClick={() => handleMToggle(m)}
                                className={[
                                    "inline-flex items-center rounded-full border px-2 py-1 text-[11px]",
                                    "transition-colors",
                                    active
                                        ? "border-accent bg-accent/20 text-accent"
                                        : "border-border bg-surface/50 text-textDim hover:border-accent/60 hover:text-accent/80",
                                ].join(" ")}
                            >
                                {label}
                            </button>
                        );
                    })}
                    {options.mValues.length === 0 && (
                        <div className="text-[11px] text-textDim/60">
                            Нет значений
                        </div>
                    )}
                </div>
            </div>

            {/* x диапазон */}
            <div className="flex flex-wrap items-end gap-4">
                <div className="flex flex-col">
                    <label
                        className="text-[11px] font-semibold text-white/90"
                        htmlFor="xMin"
                    >
                        x min
                    </label>
                    <input
                        id="xMin"
                        type="number"
                        value={draftFilter.xMin ?? ""}
                        onChange={handleXMinChange}
                        className="form-input mt-1 w-32 rounded-md border-border bg-surface/80 text-xs text-textDim placeholder:text-textDim/40 focus:border-primary focus:ring-primary"
                        placeholder={
                            options.xMin !== null ? String(options.xMin) : ""
                        }
                    />
                </div>
                <div className="flex flex-col">
                    <label
                        className="text-[11px] font-semibold text-white/90"
                        htmlFor="xMax"
                    >
                        x max
                    </label>
                    <input
                        id="xMax"
                        type="number"
                        value={draftFilter.xMax ?? ""}
                        onChange={handleXMaxChange}
                        className="form-input mt-1 w-32 rounded-md border-border bg-surface/80 text-xs text-textDim placeholder:text-textDim/40 focus:border-primary focus:ring-primary"
                        placeholder={
                            options.xMax !== null ? String(options.xMax) : ""
                        }
                    />
                </div>
            </div>

            {/* наличие ошибки */}
            <div className="flex items-center gap-3">
                <label
                    className="text-[11px] font-semibold text-white/90"
                    htmlFor="hasError"
                >
                    Ошибка (error)
                </label>
                <select
                    id="hasError"
                    value={
                        draftFilter.hasError === true
                            ? "with"
                            : draftFilter.hasError === false
                                ? "without"
                                : "any"
                    }
                    onChange={handleHasErrorChange}
                    className="form-select rounded-md border-border bg-surface/80 text-xs text-textDim focus:border-primary focus:ring-primary"
                >
                    <option value="any">Любые</option>
                    <option value="with">Только с error</option>
                    <option value="without">Только без error</option>
                </select>
            </div>
        </div>
    );
};
