// src/shared/ui/Matrix/filters/MatrixAlgoSeriesFilter.tsx

import React, { useEffect, useMemo, useRef } from "react";
import type { Accel, Series } from "@/entities/experiment/model/experiment.ts";
import { useInMemorySessionState } from "@/shared/lib/inMemorySessionState";
import {
    MatrixAccelsFilter,
    type AccelVariantMode,
    type ArgsOp,
    type ArgClause,
} from "@/shared/ui/Matrix/filters/MatrixAccelsFilter.tsx";
import {
    MatrixSeriesFilter,
    type SeriesNoiseMode,
} from "@/shared/ui/Matrix/filters/MatrixSeriesFilter.tsx";
import type { FilterMode, Group } from "@/shared/ui/Matrix/filters/MatrixAxisFilter.tsx";

function normalize(s: string): string {
    return (s ?? "").trim().toLowerCase();
}

function formatArgs(args: Record<string, unknown> | null | undefined): string {
    if (!args) return "";
    const entries = Object.entries(args).filter(([, v]) => v !== null && v !== undefined);
    if (entries.length === 0) return "";
    return entries.map(([k, v]) => `${k}=${typeof v === "string" ? v : String(v)}`).join(", ");
}

function stripFilteredSuffix(name: string): string {
    return name.replace(/\s+\[filtered:.*\]$/, "");
}

export function getDefaultAccelGroupIdentity(
    accel: Accel,
    accelById: Map<string, Accel>
): { key: string; title: string } {
    const baseAccel = accel.baseAccelId ? accelById.get(accel.baseAccelId) : null;
    const title = stripFilteredSuffix(baseAccel?.name ?? accel.name);

    return {
        key: normalize(title),
        title,
    };
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
    if (key === "name_args") return `${s.name}::${s.precision ?? ""}::${formatArgs(s.args)}`;
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
        [s.name, s.precision ?? "", formatArgs(s.args)].filter(Boolean).join(" ")
    );
}

/* -------------------- real filters -------------------- */

function parseNullableNumber(v: string): number | null {
    const t = v.trim();
    if (!t) return null;
    const n = Number(t);
    return Number.isFinite(n) ? n : null;
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

function applyAccelVariantFilter(list: Accel[], mode: AccelVariantMode): Accel[] {
    if (mode === "all") return list;
    return list.filter((accel) => (accel.variant ?? "raw") === mode);
}

function hasSeriesNoise(series: Series): boolean {
    const args = series.args;
    if (!args) return false;
    return Object.entries(args).some(([key, value]) => {
        if (!key.startsWith("noise_")) return false;
        if (value === null || value === undefined) return false;
        return String(value).trim() !== "";
    });
}

function applySeriesNoiseFilter(list: Series[], mode: SeriesNoiseMode): Series[] {
    if (mode === "all") return list;
    const shouldHaveNoise = mode === "noisy";
    return list.filter((series) => hasSeriesNoise(series) === shouldHaveNoise);
}

function applySeriesPrecisionFilter(
    list: Series[],
    selectedPrecisions: Set<string>,
    mode: FilterMode
): Series[] {
    if (selectedPrecisions.size === 0) return mode === "blacklist" ? list : [];
    if (mode === "whitelist") return list.filter((s) => selectedPrecisions.has(s.precision ?? ""));
    return list.filter((s) => !selectedPrecisions.has(s.precision ?? ""));
}

function areSetsEqual<T>(a: Set<T>, b: Set<T>): boolean {
    if (a.size !== b.size) return false;
    for (const v of a) {
        if (!b.has(v)) return false;
    }
    return true;
}

function parseScalarQuery(
    v: string
):
    | { kind: "num"; value: number }
    | { kind: "bool"; value: boolean }
    | { kind: "null" }
    | { kind: "str" } {
    const t = v.trim();
    if (!t) return { kind: "str" };

    const tl = t.toLowerCase();
    if (tl === "true") return { kind: "bool", value: true };
    if (tl === "false") return { kind: "bool", value: false };
    if (tl === "null") return { kind: "null" };

    const n = Number(t);
    if (Number.isFinite(n) && /^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$/.test(t)) {
        return { kind: "num", value: n };
    }

    return { kind: "str" };
}

function valueMatches(argVal: unknown, vqRaw: string): boolean {
    const q = parseScalarQuery(vqRaw);

    if (q.kind === "num") {
        if (typeof argVal === "number") return argVal === q.value;
        if (typeof argVal === "string") return normalize(argVal) === normalize(vqRaw);
        return false;
    }

    if (q.kind === "bool") {
        if (typeof argVal === "boolean") return argVal === q.value;
        if (typeof argVal === "string") return normalize(argVal) === (q.value ? "true" : "false");
        return false;
    }

    if (q.kind === "null") {
        return argVal === null || normalize(String(argVal)) === "null";
    }

    return normalize(String(argVal)).includes(normalize(vqRaw));
}

function clauseMatchesArgs(args: Record<string, unknown> | null, clause: ArgClause): boolean {
    const kq = normalize(clause.key);
    const vq = normalize(clause.value);

    if (!kq && !vq) return true; // empty rule does not restrict

    if (!args) return false;

    // value-only: any argument value contains vq
    if (!kq && vq) {
        for (const [, v] of Object.entries(args)) {
            if (v == null) continue;
            if (valueMatches(v, clause.value)) return true;
        }
        return false;
    }

    // key is set (substring match by key)
    if (kq) {
        const keys = Object.keys(args);
        const matchedKeys = keys.filter((kk) => normalize(kk).includes(kq));
        if (matchedKeys.length === 0) return false;

        if (!vq) return true;

        for (const kk of matchedKeys) {
            const v = args[kk];
            if (v == null) continue;
            if (valueMatches(v, clause.value)) return true;
        }
        return false;
    }

    return true;
}

function applyArgsClauses<T extends { args: Record<string, unknown> | null }>(
    list: T[],
    op: ArgsOp,
    clauses: ArgClause[]
): T[] {
    const effective = clauses.filter((c) => normalize(c.key) !== "" || normalize(c.value) !== "");
    if (effective.length === 0) return list;

    return list.filter((x) => {
        const matches = effective.map((c) => clauseMatchesArgs(x.args, c));
        return op === "and" ? matches.every(Boolean) : matches.some(Boolean);
    });
}

/* -------------------- state -------------------- */

type AxisStateBase = {
    query: string;
    groupMode: FilterMode;
    selectedGroupKeys: Set<string>;
    idMode: FilterMode;
    selectedIds: Set<string>;
};

type AccelAxisState = AxisStateBase & {
    variantMode: AccelVariantMode;
    mMinText: string;
    mMaxText: string;
    argsOp: ArgsOp;
    argClauses: ArgClause[];
};

type SeriesAxisState = AxisStateBase & {
    precisionMode: FilterMode;
    selectedPrecisions: Set<string>;
    noiseMode: SeriesNoiseMode;
    argsOp: ArgsOp;
    argClauses: ArgClause[];
};

export type MatrixAlgoSeriesFilterState = {
    accel: AccelAxisState;
    series: SeriesAxisState;
};

export type MatrixAlgoSeriesFilterInitialState = {
    accel?: Partial<AccelAxisState>;
    series?: Partial<SeriesAxisState>;
};

function cloneArgClauses(argClauses: ArgClause[] | undefined): ArgClause[] {
    if (!argClauses || argClauses.length === 0) {
        return [{ key: "", value: "" }];
    }
    return argClauses.map((clause) => ({ ...clause }));
}

function cloneStringSet(values: Iterable<string> | undefined): Set<string> {
    return new Set(values ?? []);
}

function createAccelAxisState(initialState?: Partial<AccelAxisState>): AccelAxisState {
    return {
        query: initialState?.query ?? "",
        groupMode: initialState?.groupMode ?? "whitelist",
        selectedGroupKeys: cloneStringSet(initialState?.selectedGroupKeys),
        idMode: initialState?.idMode ?? "whitelist",
        selectedIds: cloneStringSet(initialState?.selectedIds),
        variantMode: initialState?.variantMode ?? "all",
        mMinText: initialState?.mMinText ?? "",
        mMaxText: initialState?.mMaxText ?? "",
        argsOp: initialState?.argsOp ?? "and",
        argClauses: cloneArgClauses(initialState?.argClauses),
    };
}

function createSeriesAxisState(initialState?: Partial<SeriesAxisState>): SeriesAxisState {
    return {
        query: initialState?.query ?? "",
        groupMode: initialState?.groupMode ?? "whitelist",
        selectedGroupKeys: cloneStringSet(initialState?.selectedGroupKeys),
        idMode: initialState?.idMode ?? "whitelist",
        selectedIds: cloneStringSet(initialState?.selectedIds),
        precisionMode: initialState?.precisionMode ?? "whitelist",
        selectedPrecisions: cloneStringSet(initialState?.selectedPrecisions),
        noiseMode: initialState?.noiseMode ?? "all",
        argsOp: initialState?.argsOp ?? "and",
        argClauses: cloneArgClauses(initialState?.argClauses),
    };
}

export function createMatrixAlgoSeriesFilterState(
    initialState?: MatrixAlgoSeriesFilterInitialState
): MatrixAlgoSeriesFilterState {
    return {
        accel: createAccelAxisState(initialState?.accel),
        series: createSeriesAxisState(initialState?.series),
    };
}

export function cloneMatrixAlgoSeriesFilterState(
    state: MatrixAlgoSeriesFilterState
): MatrixAlgoSeriesFilterState {
    return createMatrixAlgoSeriesFilterState(state);
}

export interface MatrixAlgoSeriesFilterProps {
    accelList: Accel[];
    seriesList: Series[];
    resetKey?: string | number;
    sessionKey?: string;

    groupAccelsBy?: (a: Accel) => { key: string; title?: string };
    groupSeriesBy?: (s: Series) => { key: string; title?: string };

    initialState?: MatrixAlgoSeriesFilterInitialState;

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
        sessionKey,
        groupAccelsBy,
        groupSeriesBy,
        initialState,
        children,
    } = props;

    const [state, setState] = useInMemorySessionState<MatrixAlgoSeriesFilterState>({
        key: sessionKey,
        initialValue: () => createMatrixAlgoSeriesFilterState(initialState),
        clone: cloneMatrixAlgoSeriesFilterState,
    });
    const prevResetKeyRef = useRef<string | number | undefined>(resetKey);

    useEffect(() => {
        if (resetKey == null) return;
        if (prevResetKeyRef.current === resetKey) return;
        prevResetKeyRef.current = resetKey;
        setState(createMatrixAlgoSeriesFilterState(initialState));
    }, [initialState, resetKey, setState]);

    const accelGroups = useMemo(() => {
        const accelById = new Map((accelList ?? []).map((accel) => [accel.id, accel]));
        const get =
            groupAccelsBy ?? ((a: Accel) => getDefaultAccelGroupIdentity(a, accelById));
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

    const accelGroupKeys = useMemo(() => accelGroups.map((g) => g.key), [accelGroups]);
    const seriesGroupKeys = useMemo(() => seriesGroups.map((g) => g.key), [seriesGroups]);
    const precisionOptionSet = useMemo(() => new Set(precisionOptions), [precisionOptions]);

    // keep only group keys that still exist after data changes
    useEffect(() => {
        setState((s) => {

            const available = new Set(accelGroupKeys);
            const next = new Set(
                Array.from(s.accel.selectedGroupKeys).filter((key) => available.has(key))
            );

            if (!areSetsEqual(next, s.accel.selectedGroupKeys)) {
                return {
                    ...s,
                    accel: { ...s.accel, selectedGroupKeys: next },
                };
            }

            return s;
        });
    }, [accelGroupKeys, resetKey]);

    useEffect(() => {
        setState((s) => {

            const available = new Set(seriesGroupKeys);
            const next = new Set(
                Array.from(s.series.selectedGroupKeys).filter((key) => available.has(key))
            );

            if (!areSetsEqual(next, s.series.selectedGroupKeys)) {
                return {
                    ...s,
                    series: { ...s.series, selectedGroupKeys: next },
                };
            }

            return s;
        });
    }, [seriesGroupKeys, resetKey]);

    // keep only precision values that still exist after data changes
    useEffect(() => {
        setState((s) => {

            const next = new Set(
                Array.from(s.series.selectedPrecisions).filter((p) => precisionOptionSet.has(p))
            );

            if (!areSetsEqual(next, s.series.selectedPrecisions)) {
                return {
                    ...s,
                    series: { ...s.series, selectedPrecisions: next },
                };
            }

            return s;
        });
    }, [precisionOptionSet, resetKey]);

    const visibleAccelGroups = useMemo(() => {
        const q = normalize(state.accel.query);
        if (!q) return accelGroups;

        return accelGroups
            .map((g) => ({
                ...g,
                items: g.items.filter((a) => accelSearchText(a).includes(q)),
            }))
            .filter((g) => g.items.length > 0);
    }, [accelGroups, state.accel.query]);

    const visibleSeriesGroups = useMemo(() => {
        const q = normalize(state.series.query);
        if (!q) return seriesGroups;

        return seriesGroups
            .map((g) => ({
                ...g,
                items: g.items.filter((s) => seriesSearchText(s).includes(q)),
            }))
            .filter((g) => g.items.length > 0);
    }, [seriesGroups, state.series.query]);

    const filteredAccels = useMemo(() => {
        const q = normalize(state.accel.query);

        let out = q
            ? (accelList ?? []).filter((a) => accelSearchText(a).includes(q))
            : (accelList ?? []);

        // args clauses
        out = applyArgsClauses(out, state.accel.argsOp, state.accel.argClauses);

        // raw/filtered synthetic algorithms
        out = applyAccelVariantFilter(out, state.accel.variantMode);

        // m
        const mMin = parseNullableNumber(state.accel.mMinText);
        const mMax = parseNullableNumber(state.accel.mMaxText);
        out = applyAccelMFilter(out, mMin, mMax);

        // groups
        const byGroup = applyGroupFilter(
            accelGroups,
            state.accel.selectedGroupKeys,
            state.accel.groupMode
        );
        const byGroupIds = new Set(byGroup.map((a) => a.id));
        out = out.filter((a) => byGroupIds.has(a.id));

        // ids (if used elsewhere)
        out = applyIdFilter(out, state.accel.selectedIds, state.accel.idMode);

        return sortAccels(out, "name_args");
    }, [accelList, accelGroups, state.accel]);

    const filteredSeries = useMemo(() => {
        const q = normalize(state.series.query);

        let out = q
            ? (seriesList ?? []).filter((s) => seriesSearchText(s).includes(q))
            : (seriesList ?? []);

        // precision
        out = applySeriesPrecisionFilter(
            out,
            state.series.selectedPrecisions,
            state.series.precisionMode
        );

        // clean/noisy series imported from noise_* parquet arguments
        out = applySeriesNoiseFilter(out, state.series.noiseMode);

        // args clauses
        out = applyArgsClauses(out, state.series.argsOp, state.series.argClauses);

        // groups
        const byGroup = applyGroupFilter(
            seriesGroups,
            state.series.selectedGroupKeys,
            state.series.groupMode
        );
        const byGroupIds = new Set(byGroup.map((s) => s.id));
        out = out.filter((s) => byGroupIds.has(s.id));

        // ids
        out = applyIdFilter(out, state.series.selectedIds, state.series.idMode);

        return sortSeries(out, "name_args");
    }, [seriesList, seriesGroups, state.series]);

    return (
        <div className="flex flex-col gap-2">
            <div className="grid grid-cols-1 gap-2 md:grid-cols-2">
                <MatrixAccelsFilter
                    query={state.accel.query}
                    onQuery={(v) => setState((s) => ({ ...s, accel: { ...s.accel, query: v } }))}
                    groups={visibleAccelGroups}
                    groupMode={state.accel.groupMode}
                    onGroupMode={(m) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, groupMode: m } }))
                    }
                    selectedGroupKeys={state.accel.selectedGroupKeys}
                    onToggleGroup={(key) =>
                        setState((s) => {
                            const next = new Set(s.accel.selectedGroupKeys);
                            if (next.has(key)) {
                                next.delete(key);
                            } else {
                                next.add(key);
                            }
                            return { ...s, accel: { ...s.accel, selectedGroupKeys: next } };
                        })
                    }
                    onSelectAllGroups={() =>
                        setState((s) => ({
                            ...s,
                            accel: {
                                ...s.accel,
                                selectedGroupKeys: new Set(visibleAccelGroups.map((g) => g.key)),
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
                    variantMode={state.accel.variantMode}
                    onVariantMode={(mode) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, variantMode: mode } }))
                    }
                    argsOp={state.accel.argsOp}
                    onArgsOp={(op) =>
                        setState((s) => ({ ...s, accel: { ...s.accel, argsOp: op } }))
                    }
                    argClauses={state.accel.argClauses}
                    onChangeClause={(i, patch) =>
                        setState((s) => {
                            const next = s.accel.argClauses.map((c, idx) =>
                                idx === i ? { ...c, ...patch } : c
                            );
                            return { ...s, accel: { ...s.accel, argClauses: next } };
                        })
                    }
                    onAddClause={() =>
                        setState((s) => ({
                            ...s,
                            accel: {
                                ...s.accel,
                                argClauses: [...s.accel.argClauses, { key: "", value: "" }],
                            },
                        }))
                    }
                    onRemoveClause={(i) =>
                        setState((s) => {
                            const next = s.accel.argClauses.filter((_, idx) => idx !== i);
                            return {
                                ...s,
                                accel: {
                                    ...s.accel,
                                    argClauses: next.length ? next : [{ key: "", value: "" }],
                                },
                            };
                        })
                    }
                    onResetParams={() =>
                        setState((s) => ({
                            ...s,
                            accel: {
                                ...s.accel,
                                variantMode: "all",
                                mMinText: "",
                                mMaxText: "",
                                argsOp: "and",
                                argClauses: [{ key: "", value: "" }],
                            },
                        }))
                    }
                />

                <MatrixSeriesFilter
                    query={state.series.query}
                    onQuery={(v) => setState((s) => ({ ...s, series: { ...s.series, query: v } }))}
                    groups={visibleSeriesGroups}
                    groupMode={state.series.groupMode}
                    onGroupMode={(m) =>
                        setState((s) => ({ ...s, series: { ...s.series, groupMode: m } }))
                    }
                    selectedGroupKeys={state.series.selectedGroupKeys}
                    onToggleGroup={(key) =>
                        setState((s) => {
                            const next = new Set(s.series.selectedGroupKeys);
                            if (next.has(key)) {
                                next.delete(key);
                            } else {
                                next.add(key);
                            }
                            return { ...s, series: { ...s.series, selectedGroupKeys: next } };
                        })
                    }
                    onSelectAllGroups={() =>
                        setState((s) => ({
                            ...s,
                            series: {
                                ...s.series,
                                selectedGroupKeys: new Set(visibleSeriesGroups.map((g) => g.key)),
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
                            if (next.has(p)) {
                                next.delete(p);
                            } else {
                                next.add(p);
                            }
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
                    noiseMode={state.series.noiseMode}
                    onNoiseMode={(mode) =>
                        setState((s) => ({ ...s, series: { ...s.series, noiseMode: mode } }))
                    }
                    argsOp={state.series.argsOp}
                    onArgsOp={(op) =>
                        setState((s) => ({ ...s, series: { ...s.series, argsOp: op } }))
                    }
                    argClauses={state.series.argClauses}
                    onChangeClause={(i, patch) =>
                        setState((s) => {
                            const next = s.series.argClauses.map((c, idx) =>
                                idx === i ? { ...c, ...patch } : c
                            );
                            return { ...s, series: { ...s.series, argClauses: next } };
                        })
                    }
                    onAddClause={() =>
                        setState((s) => ({
                            ...s,
                            series: {
                                ...s.series,
                                argClauses: [...s.series.argClauses, { key: "", value: "" }],
                            },
                        }))
                    }
                    onRemoveClause={(i) =>
                        setState((s) => {
                            const next = s.series.argClauses.filter((_, idx) => idx !== i);
                            return {
                                ...s,
                                series: {
                                    ...s.series,
                                    argClauses: next.length ? next : [{ key: "", value: "" }],
                                },
                            };
                        })
                    }
                    onResetParams={() =>
                        setState((s) => ({
                            ...s,
                            series: {
                                ...s.series,
                                selectedPrecisions: new Set(precisionOptions),
                                noiseMode: "all",
                                argsOp: "and",
                                argClauses: [{ key: "", value: "" }],
                            },
                        }))
                    }
                />
            </div>

            {children({ filteredAccels, filteredSeries, state, setState })}
        </div>
    );
}


