// src/shared/ui/Matrix/filters/MatrixAlgoSeriesFilter.tsx

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

/* -------------------- sorting -------------------- */

type SortKey = "name" | "name_args" | "id";

function compareNullableNumber(a: number | null | undefined, b: number | null | undefined) {
    const aa = a ?? Number.POSITIVE_INFINITY;
    const bb = b ?? Number.POSITIVE_INFINITY;
    return aa - bb;
}

function compareStrings(a: string, b: string) {
    return a.localeCompare(b);
}

function accelSortKey(a: Accel, key: SortKey): string {
    if (key === "id") return a.id;
    if (key === "name_args") return `${a.name}::${formatArgs(a.args)}`;
    return a.name;
}

function seriesSortKey(s: Series, key: SortKey): string {
    if (key === "id") return s.id;
    if (key === "name_args") return `${s.name}::${s.precision ?? ""}::${formatArgs(s.args as any)}`;
    return s.name;
}

function sortAccels(list: Accel[], key: SortKey): Accel[] {
    const out = [...list];
    out.sort((a, b) => {
        const c0 = compareStrings(accelSortKey(a, key), accelSortKey(b, key));
        if (c0 !== 0) return c0;

        const c1 = compareNullableNumber(a.m, b.m);
        if (c1 !== 0) return c1;

        return compareStrings(a.id, b.id);
    });
    return out;
}

function sortSeries(list: Series[], key: SortKey): Series[] {
    const out = [...list];
    out.sort((a, b) => {
        const c0 = compareStrings(seriesSortKey(a, key), seriesSortKey(b, key));
        if (c0 !== 0) return c0;

        const c1 = compareStrings(a.precision ?? "", b.precision ?? "");
        if (c1 !== 0) return c1;

        return compareStrings(a.id, b.id);
    });
    return out;
}

/* -------------------- search text -------------------- */

function accelSearchText(a: Accel): string {
    return normalize(
        [a.name, a.m != null ? String(a.m) : "", formatArgs(a.args)].filter(Boolean).join(" ")
    );
}

function seriesSearchText(s: Series): string {
    return normalize(
        [s.name, s.precision ?? "", formatArgs(s.args as any)].filter(Boolean).join(" ")
    );
}

/* -------------------- real filters helpers -------------------- */

function applyArgsKVFilter<T extends { args: Record<string, any> | null }>(
    list: T[],
    argKey: string,
    argValue: string
): T[] {
    const kq = normalize(argKey);
    const vq = normalize(argValue);
    if (!kq && !vq) return list;

    return list.filter((x) => {
        const args = x.args;
        if (!args) return false;

        // value-only: any value contains vq
        if (!kq && vq) {
            for (const [, v] of Object.entries(args)) {
                if (v == null) continue;
                if (normalize(String(v)).includes(vq)) return true;
            }
            return false;
        }

        // key specified (substring match)
        if (kq) {
            const keys = Object.keys(args);
            const matchedKeys = keys.filter((kk) => normalize(kk).includes(kq));
            if (matchedKeys.length === 0) return false;

            if (!vq) return true;

            for (const kk of matchedKeys) {
                const v = (args as any)[kk];
                if (v == null) continue;
                if (normalize(String(v)).includes(vq)) return true;
            }
            return false;
        }

        return true;
    });
}

function applySeriesPrecisionFilter(
    list: Series[],
    selectedPrecisions: Set<string>,
    mode: FilterMode
): Series[] {
    if (selectedPrecisions.size === 0) return list;
    if (mode === "whitelist") return list.filter((s) => selectedPrecisions.has(s.precision ?? ""));
    return list.filter((s) => !selectedPrecisions.has(s.precision ?? ""));
}

function applyAccelMFilter(list: Accel[], mMin: number | null, mMax: number | null): Accel[] {
    if (mMin == null && mMax == null) return list;
    return list.filter((a) => {
        const m = a.m;
        if (m == null) return false;
        if (mMin != null && m < mMin) return false;
        if (mMax != null && m > mMax) return false;
        return true;
    });
}

function parseNullableNumber(v: string): number | null {
    const t = v.trim();
    if (!t) return null;
    const n = Number(t);
    return Number.isFinite(n) ? n : null;
}

/* -------------------- state -------------------- */

type AxisStateBase = {
    query: string;

    groupMode: FilterMode;
    selectedGroupKeys: Set<string>;

    idMode: FilterMode;
    selectedIds: Set<string>;

    argKey: string;
    argValue: string;
};

type AccelAxisState = AxisStateBase & {
    mMinText: string;
    mMaxText: string;
};

type SeriesAxisState = AxisStateBase & {
    precisionMode: FilterMode;
    selectedPrecisions: Set<string>;
};

export type MatrixAlgoSeriesFilterState = {
    accel: AccelAxisState;
    series: SeriesAxisState;
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
            argKey: "",
            argValue: "",
            mMinText: "",
            mMaxText: "",
            ...(initialState?.accel ?? null),
        } as AccelAxisState,
        series: {
            query: "",
            groupMode: "whitelist",
            selectedGroupKeys: new Set<string>(),
            idMode: "whitelist",
            selectedIds: new Set<string>(),
            argKey: "",
            argValue: "",
            precisionMode: "whitelist",
            selectedPrecisions: new Set<string>(),
            ...(initialState?.series ?? null),
        } as SeriesAxisState,
    }));

    useEffect(() => {
        if (resetKey == null) return;
        setState((s) => ({
            accel: {
                ...s.accel,
                query: "",
                selectedGroupKeys: new Set(),
                selectedIds: new Set(),
                argKey: "",
                argValue: "",
                mMinText: "",
                mMaxText: "",
            },
            series: {
                ...s.series,
                query: "",
                selectedGroupKeys: new Set(),
                selectedIds: new Set(),
                argKey: "",
                argValue: "",
                selectedPrecisions: new Set(),
            },
        }));
    }, [resetKey]);

    const accelGroups = useMemo(() => {
        const get = groupAccelsBy ?? ((a: Accel) => ({ key: normalize(a.name), title: a.name }));
        const groups = groupByKey(
            accelList ?? [],
            (a) => get(a).key,
            (a) => get(a).title ?? a.name
        );
        return groups.map((g) => ({ ...g, items: sortAccels(g.items, "name_args") }));
    }, [accelList, groupAccelsBy]);

    const seriesGroups = useMemo(() => {
        const get = groupSeriesBy ?? ((s: Series) => ({ key: normalize(s.name), title: s.name }));
        const groups = groupByKey(
            seriesList ?? [],
            (s) => get(s).key,
            (s) => get(s).title ?? s.name
        );
        return groups.map((g) => ({ ...g, items: sortSeries(g.items, "name_args") }));
    }, [seriesList, groupSeriesBy]);

    const precisionOptions = useMemo(() => {
        const set = new Set<string>();
        for (const s of seriesList ?? []) set.add(s.precision ?? "");
        return Array.from(set)
            .filter((x) => x !== "")
            .sort((a, b) => a.localeCompare(b));
    }, [seriesList]);

    // whitelist + пустой выбор => выбрать всё (группы)
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

    // whitelist + пустой выбор => выбрать всё (precision)
    useEffect(() => {
        setState((s) => {
            if (s.series.precisionMode !== "whitelist") return s;
            if (s.series.selectedPrecisions.size > 0) return s;
            if (precisionOptions.length === 0) return s;
            return {
                ...s,
                series: { ...s.series, selectedPrecisions: new Set(precisionOptions) },
            };
        });
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [precisionOptions.length]);

    const filteredAccels = useMemo(() => {
        const q = normalize(state.accel.query);

        let out = q
            ? (accelList ?? []).filter((a) => accelSearchText(a).includes(q))
            : (accelList ?? []);

        out = applyArgsKVFilter(out as any, state.accel.argKey, state.accel.argValue);

        const mMin = parseNullableNumber(state.accel.mMinText);
        const mMax = parseNullableNumber(state.accel.mMaxText);
        out = applyAccelMFilter(out, mMin, mMax);

        const byGroup = applyGroupFilter(
            accelGroups,
            state.accel.selectedGroupKeys,
            state.accel.groupMode
        );
        const byGroupIds = new Set(byGroup.map((a) => a.id));
        out = out.filter((a) => byGroupIds.has(a.id));

        out = applyIdFilter(out, state.accel.selectedIds, state.accel.idMode);

        return sortAccels(out, "name_args");
    }, [accelList, accelGroups, state.accel]);

    const filteredSeries = useMemo(() => {
        const q = normalize(state.series.query);

        let out = q
            ? (seriesList ?? []).filter((s) => seriesSearchText(s).includes(q))
            : (seriesList ?? []);

        out = applySeriesPrecisionFilter(
            out,
            state.series.selectedPrecisions,
            state.series.precisionMode
        );

        out = applyArgsKVFilter(out as any, state.series.argKey, state.series.argValue);

        const byGroup = applyGroupFilter(
            seriesGroups,
            state.series.selectedGroupKeys,
            state.series.groupMode
        );
        const byGroupIds = new Set(byGroup.map((s) => s.id));
        out = out.filter((s) => byGroupIds.has(s.id));

        out = applyIdFilter(out, state.series.selectedIds, state.series.idMode);

        return sortSeries(out, "name_args");
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
                    mMinText={state.accel.mMinText}
                    mMaxText={state.accel.mMaxText}
                    onMMinText={(v) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, mMinText: v } }))
                    }
                    onMMaxText={(v) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, mMaxText: v } }))
                    }
                    argKey={state.accel.argKey}
                    argValue={state.accel.argValue}
                    onArgKey={(v) => setState((s) => ({ ...s, accel: { ...s.accel, argKey: v } }))}
                    onArgValue={(v) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, argValue: v } }))
                    }
                    onResetParams={() =>
                        setState((s) => ({
                            ...s,
                            accel: {
                                ...s.accel,
                                mMinText: "",
                                mMaxText: "",
                                argKey: "",
                                argValue: "",
                            },
                        }))
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
                    precisionOptions={precisionOptions}
                    precisionMode={state.series.precisionMode}
                    onPrecisionMode={(m) =>
                        setState((s) => ({ ...s, series: { ...s.series, precisionMode: m } }))
                    }
                    selectedPrecisions={state.series.selectedPrecisions}
                    onTogglePrecision={(p) =>
                        setState((s) => {
                            const next = new Set(s.series.selectedPrecisions);
                            next.has(p) ? next.delete(p) : next.add(p);
                            return { ...s, series: { ...s.series, selectedPrecisions: next } };
                        })
                    }
                    onSelectAllPrecisions={() =>
                        setState((s) => ({
                            ...s,
                            series: { ...s.series, selectedPrecisions: new Set(precisionOptions) },
                        }))
                    }
                    onClearPrecisions={() =>
                        setState((s) => ({
                            ...s,
                            series: { ...s.series, selectedPrecisions: new Set() },
                        }))
                    }
                    argKey={state.series.argKey}
                    argValue={state.series.argValue}
                    onArgKey={(v) =>
                        setState((s) => ({ ...s, series: { ...s.series, argKey: v } }))
                    }
                    onArgValue={(v) =>
                        setState((s) => ({ ...s, series: { ...s.series, argValue: v } }))
                    }
                    onResetParams={() =>
                        setState((s) => ({
                            ...s,
                            series: {
                                ...s.series,
                                selectedPrecisions: new Set(precisionOptions),
                                argKey: "",
                                argValue: "",
                            },
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
