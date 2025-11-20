// AlgorithmSeriesDiffHeatmap.tsx

import { useEffect, useMemo, useRef, useState } from "react";
import html2canvas from "html2canvas";
import type {
    Experiment,
    Series,
    Accel,
    SeriesArgs,
    AccelArgs,
    SeriesAccel,
} from "@/types/experiment";

type SeriesKey = string;
type AlgoKey = string;

/* ------------ Series / Algo info ------------ */

interface SeriesInfo {
    key: SeriesKey;
    seriesName: string;
    xLabel: string;
    xSort: number | null;
}

interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: AccelArgs | null;
}

/* ------------ утилиты по аргументам ------------ */

type Scalar = number | string | boolean | null;

function toSortableNumber(v: Scalar): number | null {
    if (typeof v === "number") {
        return Number.isFinite(v) ? v : null;
    }
    if (typeof v === "string") {
        const n = Number(v);
        return Number.isFinite(n) ? n : null;
    }
    return null;
}

function parseX(args: SeriesArgs | null): { xLabel: string; xSort: number | null } {
    const raw = args?.x as Scalar | undefined;
    if (raw == null) {
        return { xLabel: "∅", xSort: null };
    }
    return {
        xLabel: String(raw),
        xSort: toSortableNumber(raw),
    };
}

function nonNullEntries<T extends Record<string, unknown>>(obj: T | null | undefined) {
    if (!obj) return [] as [string, unknown][];
    return Object.entries(obj).filter(
        ([, v]) => v !== null && v !== undefined,
    );
}

function buildArgsSummary(args: AccelArgs | null): string {
    const entries = nonNullEntries(args);
    if (entries.length === 0) return "";
    entries.sort(([a], [b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

/* ------------ классификация цветов ------------ */

interface ColorSpec {
    bgClass: string;
    borderClass: string;
    label: string;
}

// градация зелёного (улучшение)
const GREEN_BY_LEVEL: Record<"huge" | "big" | "mid" | "small", string> = {
    huge: "bg-green-400/90",
    big: "bg-green-400/70",
    mid: "bg-green-500/60",
    small: "bg-green-500/40",
};

// градация красного (ухудшение)
const RED_BY_LEVEL: Record<"huge" | "big" | "mid" | "small", string> = {
    huge: "bg-red-400/90",
    big: "bg-red-400/70",
    mid: "bg-red-500/60",
    small: "bg-red-500/40",
};

type SideState = "none" | "ok" | "err0" | "errN";

/**
 * Нормализация состояния стороны.
 */
function sideState(hasCell: boolean, hasErr: boolean, n: number | null): SideState {
    if (!hasCell) return "none";
    if (!hasErr) return "ok";

    if (n == null || !Number.isFinite(n) || n <= 0) {
        return "err0";
    }
    return "errN";
}

/**
 * Полная классификация по всем случаям.
 */
export function classifyErrorChangeFull(
    hasPrevCell: boolean,
    hasNextCell: boolean,
    hasPrevErr: boolean,
    nPrev: number | null,
    hasNextErr: boolean,
    nNext: number | null,
): ColorSpec {
    const statePrev = sideState(hasPrevCell, hasPrevErr, nPrev);
    const stateNext = sideState(hasNextCell, hasNextErr, nNext);

    const p = statePrev === "errN" || statePrev === "err0" ? (nPrev ?? 0) : null;
    const q = stateNext === "errN" || stateNext === "err0" ? (nNext ?? 0) : null;

    // none / none / появления / исчезновения

    if (statePrev === "none" && stateNext === "none") {
        return {
            bgClass: "bg-slate-900/60",
            borderClass: "border-slate-700/60",
            label: "нет пары ни в одном precision",
        };
    }

    if (statePrev === "none" && stateNext === "ok") {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-emerald-300/80",
            label: "пара появилась и сразу ok",
        };
    }

    if (statePrev === "ok" && stateNext === "none") {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-amber-300/80",
            label: "пара исчезла, раньше была ok",
        };
    }

    if (statePrev === "none" && (stateNext === "err0" || stateNext === "errN")) {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-red-300/80",
            label: "пара появилась с ошибкой",
        };
    }

    if ((statePrev === "err0" || statePrev === "errN") && stateNext === "none") {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-emerald-300/80",
            label: "пара исчезла, раньше была с ошибкой",
        };
    }

    // обе стороны существуют

    if (statePrev === "ok" && stateNext === "ok") {
        return {
            bgClass: "bg-amber-400/90 hover:bg-amber-300/90",
            borderClass: "border-amber-200",
            label: "ok → ok (ошибки нет в обоих)",
        };
    }

    if (statePrev === "ok" && (stateNext === "err0" || stateNext === "errN")) {
        return {
            bgClass: "bg-purple-500/80 hover:bg-purple-400/80",
            borderClass: "border-purple-200/90",
            label: "ok → error",
        };
    }

    if ((statePrev === "err0" || statePrev === "errN") && stateNext === "ok") {
        return {
            bgClass: "bg-green-500/80 hover:bg-green-400/80",
            borderClass: "border-green-200/90",
            label: "error → ok",
        };
    }

    if (statePrev === "err0" && stateNext === "err0") {
        return {
            bgClass: "bg-white",
            borderClass: "border-border/60",
            label: "error(0) → error(0)",
        };
    }

    if (statePrev === "err0" && stateNext === "errN") {
        return {
            bgClass: "bg-sky-500/70 hover:bg-sky-400/80",
            borderClass: "border-sky-200/90",
            label: `error(0) → error(n=${q ?? 0})`,
        };
    }

    if (statePrev === "errN" && stateNext === "err0") {
        return {
            bgClass: "bg-red-500/70 hover:bg-red-400/80",
            borderClass: "border-red-200/90",
            label: `error(n=${p ?? 0}) → error(0)`,
        };
    }

    // errN → errN

    const n1 = p ?? 0;
    const n2 = q ?? 0;

    if (n1 === n2) {
        return {
            bgClass: "bg-yellow-500/80 hover:bg-yellow-400/80",
            borderClass: "border-yellow-200/90",
            label: `error(n=${n1}) без изменений`,
        };
    }

    const delta = n2 - n1;
    const absDelta = Math.abs(delta);

    const levelKey: "huge" | "big" | "mid" | "small" =
        absDelta >= 1000 ? "huge" :
            absDelta >= 100 ? "big" :
                absDelta >= 10 ? "mid" :
                    "small";

    if (delta > 0) {
        return {
            bgClass: `${GREEN_BY_LEVEL[levelKey]} hover:bg-green-300/80`,
            borderClass: "border-green-200/90",
            label: `error(n=${n1}) → error(n=${n2}), ошибка позже (Δn=+${delta})`,
        };
    }

    return {
        bgClass: `${RED_BY_LEVEL[levelKey]} hover:bg-red-300/80`,
        borderClass: "border-red-200/90",
        label: `error(n=${n1}) → error(n=${n2}), ошибка раньше (Δn=${delta})`,
    };
}

/* ------------ извлечение ошибок из SeriesAccel ------------ */

interface ErrorSideInfo {
    hasCell: boolean;
    hasErr: boolean;
    n: number | null;
}

function extractErrorInfo(sa: SeriesAccel | null | undefined): ErrorSideInfo {
    if (!sa) {
        return { hasCell: false, hasErr: false, n: null };
    }

    const errors = sa.errors ?? [];
    if (errors.length === 0) {
        return { hasCell: true, hasErr: false, n: null };
    }

    const ns = errors
        .map((e) => e.n)
        .filter((n) => typeof n === "number" && Number.isFinite(n));

    if (ns.length === 0) {
        return { hasCell: true, hasErr: true, n: null };
    }

    const nMin = Math.min(...ns);
    return { hasCell: true, hasErr: true, n: nMin };
}

/* ------------ дифф по (prevPrecision, nextPrecision) ------------ */

interface DiffRecord {
    prev: ErrorSideInfo;
    next: ErrorSideInfo;
}

type Side = "prev" | "next";

export function AlgorithmSeriesDiffHeatmap({
                                               experiment,
                                               maxSeries,
                                           }: {
    experiment: Experiment | null;
    maxSeries?: number;
}) {
    const containerRef = useRef<HTMLDivElement | null>(null);

    /* базовые данные + список precision в порядке появления */

    const {
        seriesRaw,
        accelRaw,
        seriesAccelRaw,
        precisionsOrder,
    } = useMemo(() => {
        if (!experiment) {
            return {
                seriesRaw: [] as Series[],
                accelRaw: [] as Accel[],
                seriesAccelRaw: [] as SeriesAccel[],
                precisionsOrder: [] as string[],
            };
        }

        const seriesRaw = experiment.seriesList ?? [];
        const accelRaw = experiment.accelList ?? [];
        const seriesAccelRaw = experiment.seriesAccelList ?? [];

        const precisionsOrder: string[] = [];
        for (const s of seriesRaw) {
            if (!precisionsOrder.includes(s.precision)) {
                precisionsOrder.push(s.precision);
            }
        }

        return { seriesRaw, accelRaw, seriesAccelRaw, precisionsOrder };
    }, [experiment]);

    /* выбранные precision */

    const [prevPrecision, setPrevPrecision] = useState<string | null>(null);
    const [nextPrecision, setNextPrecision] = useState<string | null>(null);

    // при смене доступных precision — нормализуем выбор
    useEffect(() => {
        if (precisionsOrder.length === 0) {
            setPrevPrecision(null);
            setNextPrecision(null);
            return;
        }

        setPrevPrecision((old) => {
            if (old && precisionsOrder.includes(old)) return old;
            return precisionsOrder[0] ?? null;
        });

        setNextPrecision((old) => {
            if (old && precisionsOrder.includes(old)) return old;
            return precisionsOrder[1] ?? precisionsOrder[0] ?? null;
        });
    }, [precisionsOrder]);

    /* вычисление матрицы диффа под выбранную пару precision */

    const {
        seriesList,
        algoList,
        diffMap,
    } = useMemo(() => {
        const resultEmpty = {
            seriesList: [] as SeriesInfo[],
            algoList: [] as AlgoInfo[],
            diffMap: new Map<string, DiffRecord>(),
        };

        if (
            !seriesRaw.length ||
            !accelRaw.length ||
            !seriesAccelRaw.length ||
            !prevPrecision ||
            !nextPrecision ||
            prevPrecision === nextPrecision
        ) {
            return resultEmpty;
        }

        const seriesGroups = new Map<
            SeriesKey,
            {
                info: SeriesInfo;
                bySide: Partial<Record<Side, { seriesId: string }>>;
            }
        >();

        // группируем ряды по (name,x), но только с выбранными precision
        for (const s of seriesRaw) {
            if (s.precision !== prevPrecision && s.precision !== nextPrecision) {
                continue;
            }

            const { xLabel, xSort } = parseX(s.args ?? null);
            const baseKey: SeriesKey = `${s.name}||x=${xLabel}`;
            const side: Side = s.precision === prevPrecision ? "prev" : "next";

            let group = seriesGroups.get(baseKey);
            if (!group) {
                group = {
                    info: {
                        key: baseKey,
                        seriesName: s.name,
                        xLabel,
                        xSort,
                    },
                    bySide: {},
                };
                seriesGroups.set(baseKey, group);
            }

            group.bySide[side] = { seriesId: s.id };
        }

        if (seriesGroups.size === 0) {
            return resultEmpty;
        }

        const seriesIdMeta = new Map<
            string,
            { side: Side; baseKey: SeriesKey }
        >();
        for (const [baseKey, group] of seriesGroups.entries()) {
            for (const side of ["prev", "next"] as Side[]) {
                const meta = group.bySide[side];
                if (meta) {
                    seriesIdMeta.set(meta.seriesId, { side, baseKey });
                }
            }
        }

        const accelById = new Map<string, Accel>();
        for (const a of accelRaw) {
            accelById.set(a.id, a);
        }

        const algoMap = new Map<AlgoKey, AlgoInfo>();
        const diffMap = new Map<string, DiffRecord>();

        for (const sa of seriesAccelRaw) {
            const meta = seriesIdMeta.get(sa.series_id);
            if (!meta) continue;

            const algoKey: AlgoKey = sa.accel_id;
            const baseKey: SeriesKey = meta.baseKey;
            const side: Side = meta.side;

            if (!algoMap.has(algoKey)) {
                const accel = accelById.get(algoKey);
                if (!accel) continue;
                const args = accel.args ?? null;
                algoMap.set(algoKey, {
                    key: algoKey,
                    algorithmName: accel.name,
                    m: accel.m,
                    argsSummary: buildArgsSummary(args),
                    algorithmArgs: args,
                });
            }

            const key = `${algoKey}||${baseKey}`;
            let rec = diffMap.get(key);
            if (!rec) {
                rec = {
                    prev: { hasCell: false, hasErr: false, n: null },
                    next: { hasCell: false, hasErr: false, n: null },
                };
                diffMap.set(key, rec);
            }

            const info = extractErrorInfo(sa);
            if (side === "prev") {
                rec.prev = info;
            } else {
                rec.next = info;
            }
        }

        const seriesList = Array.from(seriesGroups.values())
            .map((g) => g.info)
            .sort((a, b) => {
                const byName = a.seriesName.localeCompare(b.seriesName);
                if (byName !== 0) return byName;

                const ax = a.xSort;
                const bx = b.xSort;
                if (ax != null && bx != null) return ax - bx;
                if (ax != null) return -1;
                if (bx != null) return 1;
                return a.xLabel.localeCompare(b.xLabel);
            });

        const algoList = Array.from(algoMap.values()).sort((a, b) => {
            const byName = a.algorithmName.localeCompare(b.algorithmName);
            if (byName !== 0) return byName;
            return (a.m ?? 0) - (b.m ?? 0);
        });

        return { seriesList, algoList, diffMap };
    }, [
        seriesRaw,
        accelRaw,
        seriesAccelRaw,
        prevPrecision,
        nextPrecision,
    ]);

    /* ------------ пагинация и экспорт ------------ */

    const [page, setPage] = useState(0);

    const pageSize =
        maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;

    const totalPages = Math.max(
        1,
        Math.ceil((seriesList.length || 1) / pageSize),
    );

    const handleExportPng = async () => {
        const el = containerRef.current;
        if (!el) return;

        const canvas = await html2canvas(el, {
            backgroundColor: "#020617",
            scale: 2,
            scrollX: 0,
            scrollY: -window.scrollY,
        });

        const dataUrl = canvas.toDataURL("image/png");
        const link = document.createElement("a");
        link.href = dataUrl;
        link.download = `heatmap_page-${page + 1}-of-${totalPages}.png`;
        link.click();
    };

    useEffect(() => {
        if (page > totalPages - 1) {
            setPage(totalPages - 1);
        }
    }, [page, totalPages]);

    const startIndex = page * pageSize;
    const endIndex = startIndex + pageSize;
    const seriesListShown =
        maxSeries && maxSeries > 0
            ? seriesList.slice(startIndex, endIndex)
            : seriesList;

    /* ------------ рендер ------------ */

    return (
        <div className="space-y-2">
            <div className="flex items-center justify-between text-xs text-textDim">
                <div className="flex flex-col gap-1">
                    <h2 className="text-sm font-semibold text-textDim">
                        Хитмапа разницы по шагам ошибок: алгоритмы × ряды
                    </h2>
                    {prevPrecision && nextPrecision && prevPrecision !== nextPrecision && (
                        <div className="text-[10px] text-textDim/80">
                            precision: {prevPrecision} → {nextPrecision}
                        </div>
                    )}
                </div>

                <div className="flex items-center gap-3">
                    {/* выбор precision */}
                    <div className="flex items-center gap-2 text-[10px]">
                        <label className="flex items-center gap-1">
                            <span>precision₁:</span>
                            <select
                                className="rounded border border-border bg-surface px-1 py-[1px]"
                                value={prevPrecision ?? ""}
                                onChange={(e) =>
                                    setPrevPrecision(
                                        e.target.value || null,
                                    )
                                }
                            >
                                {precisionsOrder.length === 0 && (
                                    <option value="">—</option>
                                )}
                                {precisionsOrder.map((p) => (
                                    <option key={p} value={p}>
                                        {p}
                                    </option>
                                ))}
                            </select>
                        </label>
                        <span>→</span>
                        <label className="flex items-center gap-1">
                            <span>precision₂:</span>
                            <select
                                className="rounded border border-border bg-surface px-1 py-[1px]"
                                value={nextPrecision ?? ""}
                                onChange={(e) =>
                                    setNextPrecision(
                                        e.target.value || null,
                                    )
                                }
                            >
                                {precisionsOrder.length === 0 && (
                                    <option value="">—</option>
                                )}
                                {precisionsOrder.map((p) => (
                                    <option key={p} value={p}>
                                        {p}
                                    </option>
                                ))}
                            </select>
                        </label>
                    </div>

                    <div>
                        Алгоритмы: {algoList.length} · Ряды:{" "}
                        {seriesListShown.length} из {seriesList.length}
                    </div>

                    <button
                        type="button"
                        onClick={handleExportPng}
                        className="rounded border border-border bg-surface px-2 py-1 text-[10px] hover:bg-panel"
                    >
                        Скачать PNG (стр. {page + 1}/{totalPages})
                    </button>

                    {maxSeries &&
                        maxSeries > 0 &&
                        seriesList.length > maxSeries && (
                            <div className="flex items-center gap-1 text-[10px]">
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() => setPage(0)}
                                    disabled={page === 0}
                                >
                                    «
                                </button>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() =>
                                        setPage((p) => Math.max(0, p - 1))
                                    }
                                    disabled={page === 0}
                                >
                                    ‹
                                </button>
                                <span className="px-1">
                                    стр. {page + 1} / {totalPages}
                                </span>
                                <span className="text-textDim/60">
                                    колонки {startIndex + 1}–
                                    {Math.min(endIndex, seriesList.length)}
                                </span>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() =>
                                        setPage((p) =>
                                            Math.min(totalPages - 1, p + 1),
                                        )
                                    }
                                    disabled={page >= totalPages - 1}
                                >
                                    ›
                                </button>
                                <button
                                    type="button"
                                    className="rounded border border-border bg-surface px-1 py-[1px] disabled:opacity-40 hover:bg-panel"
                                    onClick={() => setPage(totalPages - 1)}
                                    disabled={page >= totalPages - 1}
                                >
                                    »
                                </button>
                            </div>
                        )}
                </div>
            </div>

            <div
                ref={containerRef}
                className="overflow-auto rounded-xl2 border border-border bg-panel shadow-panel"
            >
                <table className="border-collapse text-[10px] leading-tight text-textDim">
                    <thead className="bg-surface/80">
                    <tr>
                        <th className="sticky left-0 top-0 z-20 border border-border bg-surface/90 px-1 py-1 text-left align-bottom text-[10px]">
                            Алгоритм \ Ряд
                        </th>

                        {seriesListShown.map((s) => (
                            <th
                                key={s.key}
                                className="border border-border px-0 py-0 text-center align-bottom"
                                title={`${s.seriesName}\n x = ${s.xLabel}`}
                            >
                                <div className="relative h-28 w-[40px] flex items-center justify-center">
                                    <span
                                        className="absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2 rotate-[-90deg] whitespace-nowrap text-[9px] leading-tight"
                                    >
                                        {s.seriesName}
                                    </span>
                                    <span className="absolute bottom-1 text-[8px] text-textDim/70">
                                        x={s.xLabel}
                                    </span>
                                </div>
                            </th>
                        ))}
                    </tr>
                    </thead>
                    <tbody>
                    {algoList.map((algo) => (
                        <tr key={algo.key}>
                            <th
                                className="sticky left-0 z-10 border border-border bg-panel px-1 py-[2px] text-left align-top"
                                title={(() => {
                                    const lines: string[] = [];
                                    lines.push(`Алгоритм: ${algo.algorithmName}`);
                                    lines.push(
                                        `m = ${
                                            algo.m != null ? String(algo.m) : "∅"
                                        }`,
                                    );
                                    const entries = nonNullEntries(
                                        algo.algorithmArgs,
                                    );
                                    if (entries.length > 0) {
                                        lines.push("Аргументы:");
                                        for (const [k, v] of entries.sort(
                                            ([a], [b]) =>
                                                a.localeCompare(b),
                                        )) {
                                            lines.push(`  ${k}: ${v}`);
                                        }
                                    }
                                    return lines.join("\n");
                                })()}
                            >
                                <div className="whitespace-pre leading-tight">
                                    <span className="block max-w-[150px] truncate">
                                        {algo.algorithmName}
                                    </span>
                                    <span className="text-[9px] text-textDim/70">
                                        {algo.m != null
                                            ? `m=${String(algo.m)}`
                                            : "m=∅"}
                                    </span>
                                    {algo.argsSummary && (
                                        <div className="mt-[1px] max-w-[150px] truncate text-[8px] text-textDim/60">
                                            {algo.argsSummary}
                                        </div>
                                    )}
                                </div>
                            </th>

                            {seriesListShown.map((s) => {
                                const cellKey = `${algo.key}||${s.key}`;
                                const rec = diffMap.get(cellKey);

                                const prevInfo = rec?.prev ?? {
                                    hasCell: false,
                                    hasErr: false,
                                    n: null,
                                };
                                const nextInfo = rec?.next ?? {
                                    hasCell: false,
                                    hasErr: false,
                                    n: null,
                                };

                                const { bgClass, borderClass, label } =
                                    classifyErrorChangeFull(
                                        prevInfo.hasCell,
                                        nextInfo.hasCell,
                                        prevInfo.hasErr,
                                        prevInfo.n,
                                        nextInfo.hasErr,
                                        nextInfo.n,
                                    );

                                let deltaText: string;

                                if (!prevInfo.hasCell && !nextInfo.hasCell) {
                                    deltaText = "∅ / ∅";
                                } else if (!prevInfo.hasCell && nextInfo.hasCell) {
                                    deltaText = "∅ → …";
                                } else if (prevInfo.hasCell && !nextInfo.hasCell) {
                                    deltaText = "… → ∅";
                                } else if (!prevInfo.hasErr && !nextInfo.hasErr) {
                                    deltaText = "ok → ok";
                                } else if (prevInfo.hasErr && !nextInfo.hasErr) {
                                    deltaText = "err → ok";
                                } else if (!prevInfo.hasErr && nextInfo.hasErr) {
                                    deltaText = "ok → err";
                                } else if (prevInfo.n != null && nextInfo.n != null) {
                                    const delta = nextInfo.n - prevInfo.n;
                                    deltaText = `n: ${prevInfo.n} → ${nextInfo.n} (Δ=${delta >= 0 ? "+" : ""}${delta})`;
                                } else {
                                    deltaText = `n: ${prevInfo.n ?? "∅"} → ${nextInfo.n ?? "∅"}`;
                                }

                                const tooltipLines: string[] = [];

                                tooltipLines.push(
                                    `Ряд: ${s.seriesName}, x=${s.xLabel}`,
                                );
                                tooltipLines.push(
                                    `Алгоритм: ${algo.algorithmName}, m=${algo.m ?? "∅"}`,
                                );
                                tooltipLines.push("");

                                tooltipLines.push(
                                    `Старый precision (${prevPrecision ?? "?"}): ${
                                        prevInfo.hasCell
                                            ? prevInfo.hasErr
                                                ? prevInfo.n != null
                                                    ? `ошибка, n=${prevInfo.n}`
                                                    : "ошибка, n неизвестно"
                                                : "ошибки нет"
                                            : "нет пары"
                                    }`,
                                );
                                tooltipLines.push(
                                    `Новый precision (${nextPrecision ?? "?"}): ${
                                        nextInfo.hasCell
                                            ? nextInfo.hasErr
                                                ? nextInfo.n != null
                                                    ? `ошибка, n=${nextInfo.n}`
                                                    : "ошибка, n неизвестно"
                                                : "ошибки нет"
                                            : "нет пары"
                                    }`,
                                );

                                tooltipLines.push("");
                                tooltipLines.push(label);

                                const title = tooltipLines.join("\n");

                                return (
                                    <td
                                        key={cellKey}
                                        title={title}
                                        className={`min-w-[30px] border px-[2px] py-[2px] text-center text-[10px] cursor-default ${borderClass} ${bgClass}`}
                                    >
                                        <div className="leading-tight">
                                            <div className="text-[9px]">
                                                {deltaText}
                                            </div>
                                        </div>
                                    </td>
                                );
                            })}
                        </tr>
                    ))}
                    </tbody>
                </table>
            </div>
        </div>
    );
}
