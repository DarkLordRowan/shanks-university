import React, { useEffect, useMemo, useState } from "react";
import type { Accel, Series } from "@/entities/experiment/model/experiment.ts";
import { MatrixAccelsFilter } from "@/shared/ui/Matrix/filters/MatrixAccelsFilter.tsx";
import { MatrixSeriesFilter } from "@/shared/ui/Matrix/filters/MatrixSeriesFilter.tsx";
import type { FilterMode, Group } from "@/shared/ui/Matrix/filters/MatrixAxisFilter.tsx";

function normalize(s: string): string {
    return (s ?? "").trim().toLowerCase();
}

function formatArgs(args: Record<string, any> | null | undefined): string {
    if (!args) return "";
    const entries = Object.entries(args).filter(([, v]) => v !== null && v !== undefined);
    if (entries.length === 0) return "";
    return entries.map(([k, v]) => `${k}=${typeof v === "string" ? v : String(v)}`).join(", ");
}

function groupByKey<T>(
    items: T[],
    getKey: (x: T) => string,
    getTitle: (x: T) => string
): Group<T>[] {
    const map = new Map<string, { title: string; items: T[] }>();
    for (const it of items) {
        const key = getKey(it);
        const title = getTitle(it);
        const cur = map.get(key);
        if (!cur) map.set(key, { title, items: [it] });
        else cur.items.push(it);
    }
    return Array.from(map.entries())
        .map(([key, v]) => ({ key, title: v.title, items: v.items }))
        .sort((a, b) => a.title.localeCompare(b.title));
}

function applyGroupFilter<T>(
    groups: Group<T>[],
    selectedGroupKeys: Set<string>,
    mode: FilterMode
): T[] {
    if (mode === "whitelist")
        return groups.filter((g) => selectedGroupKeys.has(g.key)).flatMap((g) => g.items);
    return groups.filter((g) => !selectedGroupKeys.has(g.key)).flatMap((g) => g.items);
}

function applyIdFilter<T extends { id: string }>(
    items: T[],
    selectedIds: Set<string>,
    mode: FilterMode
): T[] {
    if (selectedIds.size === 0) return items;
    if (mode === "whitelist") return items.filter((x) => selectedIds.has(x.id));
    return items.filter((x) => !selectedIds.has(x.id));
}

type AxisState = {
    query: string;
    groupMode: FilterMode;
    selectedGroupKeys: Set<string>;
    idMode: FilterMode;
    selectedIds: Set<string>;
};

export type MatrixAlgoSeriesFilterState = {
    accel: AxisState;
    series: AxisState;
};

export interface MatrixAlgoSeriesFilterProps {
    accelList: Accel[];
    seriesList: Series[];

    /** сброс фильтра при смене ключа (например experiment.id) */
    resetKey?: string | number;

    /** кастомная группировка (по умолчанию normalize(name)) */
    groupAccelsBy?: (a: Accel) => { key: string; title?: string };
    groupSeriesBy?: (s: Series) => { key: string; title?: string };

    /** начальное состояние */
    initialState?: Partial<MatrixAlgoSeriesFilterState>;

    /** отрендерить матрицу/контент по отфильтрованным спискам */
    children: (args: {
        filteredAccels: Accel[];
        filteredSeries: Series[];
        state: MatrixAlgoSeriesFilterState;
        setState: React.Dispatch<React.SetStateAction<MatrixAlgoSeriesFilterState>>;
    }) => React.ReactNode;
}

export function MatrixAlgoSeriesFilter(props: MatrixAlgoSeriesFilterProps) {
    const {
        accelList,
        seriesList,
        resetKey,
        groupAccelsBy,
        groupSeriesBy,
        initialState,
        children,
    } = props;

    const [state, setState] = useState<MatrixAlgoSeriesFilterState>(() => ({
        accel: {
            query: "",
            groupMode: "whitelist",
            selectedGroupKeys: new Set<string>(),
            idMode: "whitelist",
            selectedIds: new Set<string>(),
            ...(initialState?.accel ?? null),
        } as AxisState,
        series: {
            query: "",
            groupMode: "whitelist",
            selectedGroupKeys: new Set<string>(),
            idMode: "whitelist",
            selectedIds: new Set<string>(),
            ...(initialState?.series ?? null),
        } as AxisState,
    }));

    useEffect(() => {
        if (resetKey == null) return;
        setState((s) => ({
            accel: { ...s.accel, query: "", selectedGroupKeys: new Set(), selectedIds: new Set() },
            series: {
                ...s.series,
                query: "",
                selectedGroupKeys: new Set(),
                selectedIds: new Set(),
            },
        }));
    }, [resetKey]);

    const accelGroups = useMemo(() => {
        const get = groupAccelsBy ?? ((a: Accel) => ({ key: normalize(a.name), title: a.name }));
        return groupByKey(
            accelList ?? [],
            (a) => get(a).key,
            (a) => get(a).title ?? a.name
        );
    }, [accelList, groupAccelsBy]);

    const seriesGroups = useMemo(() => {
        const get = groupSeriesBy ?? ((s: Series) => ({ key: normalize(s.name), title: s.name }));
        return groupByKey(
            seriesList ?? [],
            (s) => get(s).key,
            (s) => get(s).title ?? s.name
        );
    }, [seriesList, groupSeriesBy]);

    // whitelist + пустой выбор => выбрать всё
    useEffect(() => {
        setState((s) => {
            if (s.accel.groupMode !== "whitelist") return s;
            if (s.accel.selectedGroupKeys.size > 0) return s;
            return {
                ...s,
                accel: { ...s.accel, selectedGroupKeys: new Set(accelGroups.map((g) => g.key)) },
            };
        });
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [accelGroups.length]);

    useEffect(() => {
        setState((s) => {
            if (s.series.groupMode !== "whitelist") return s;
            if (s.series.selectedGroupKeys.size > 0) return s;
            return {
                ...s,
                series: { ...s.series, selectedGroupKeys: new Set(seriesGroups.map((g) => g.key)) },
            };
        });
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [seriesGroups.length]);

    const filteredAccels = useMemo(() => {
        const q = normalize(state.accel.query);
        const base = q
            ? (accelList ?? []).filter(
                  (a) => normalize(a.name).includes(q) || normalize(formatArgs(a.args)).includes(q)
              )
            : (accelList ?? []);

        const byGroup = applyGroupFilter(
            accelGroups,
            state.accel.selectedGroupKeys,
            state.accel.groupMode
        );
        const byGroupIds = new Set(byGroup.map((a) => a.id));
        const afterGroup = base.filter((a) => byGroupIds.has(a.id));

        return applyIdFilter(afterGroup, state.accel.selectedIds, state.accel.idMode);
    }, [accelList, accelGroups, state.accel]);

    const filteredSeries = useMemo(() => {
        const q = normalize(state.series.query);
        const base = q
            ? (seriesList ?? []).filter(
                  (s) =>
                      normalize(s.name).includes(q) ||
                      normalize(formatArgs(s.args as any)).includes(q)
              )
            : (seriesList ?? []);

        const byGroup = applyGroupFilter(
            seriesGroups,
            state.series.selectedGroupKeys,
            state.series.groupMode
        );
        const byGroupIds = new Set(byGroup.map((s) => s.id));
        const afterGroup = base.filter((s) => byGroupIds.has(s.id));

        return applyIdFilter(afterGroup, state.series.selectedIds, state.series.idMode);
    }, [seriesList, seriesGroups, state.series]);

    return (
        <div className="flex flex-col gap-2">
            <div className="grid grid-cols-1 gap-2 md:grid-cols-2">
                <MatrixAccelsFilter
                    query={state.accel.query}
                    onQuery={(v) => setState((s) => ({ ...s, accel: { ...s.accel, query: v } }))}
                    groups={accelGroups}
                    groupMode={state.accel.groupMode}
                    onGroupMode={(m) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, groupMode: m } }))
                    }
                    selectedGroupKeys={state.accel.selectedGroupKeys}
                    onToggleGroup={(key) =>
                        setState((s) => {
                            const next = new Set(s.accel.selectedGroupKeys);
                            next.has(key) ? next.delete(key) : next.add(key);
                            return { ...s, accel: { ...s.accel, selectedGroupKeys: next } };
                        })
                    }
                    onSelectAllGroups={() =>
                        setState((s) => ({
                            ...s,
                            accel: {
                                ...s.accel,
                                selectedGroupKeys: new Set(accelGroups.map((g) => g.key)),
                            },
                        }))
                    }
                    onClearGroups={() =>
                        setState((s) => ({
                            ...s,
                            accel: { ...s.accel, selectedGroupKeys: new Set() },
                        }))
                    }
                    idMode={state.accel.idMode}
                    onIdMode={(m) => setState((s) => ({ ...s, accel: { ...s.accel, idMode: m } }))}
                    selectedIds={state.accel.selectedIds}
                    onClearIds={() =>
                        setState((s) => ({ ...s, accel: { ...s.accel, selectedIds: new Set() } }))
                    }
                />

                <MatrixSeriesFilter
                    query={state.series.query}
                    onQuery={(v) => setState((s) => ({ ...s, series: { ...s.series, query: v } }))}
                    groups={seriesGroups}
                    groupMode={state.series.groupMode}
                    onGroupMode={(m) =>
                        setState((s) => ({ ...s, series: { ...s.series, groupMode: m } }))
                    }
                    selectedGroupKeys={state.series.selectedGroupKeys}
                    onToggleGroup={(key) =>
                        setState((s) => {
                            const next = new Set(s.series.selectedGroupKeys);
                            next.has(key) ? next.delete(key) : next.add(key);
                            return { ...s, series: { ...s.series, selectedGroupKeys: next } };
                        })
                    }
                    onSelectAllGroups={() =>
                        setState((s) => ({
                            ...s,
                            series: {
                                ...s.series,
                                selectedGroupKeys: new Set(seriesGroups.map((g) => g.key)),
                            },
                        }))
                    }
                    onClearGroups={() =>
                        setState((s) => ({
                            ...s,
                            series: { ...s.series, selectedGroupKeys: new Set() },
                        }))
                    }
                />
            </div>

            {children({
                filteredAccels,
                filteredSeries,
                state,
                setState,
            })}
        </div>
    );
}
