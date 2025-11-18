import { useEffect, useMemo, useRef, useState } from "react";
import type { DiffCell } from "../types/diffCell";
import html2canvas from "html2canvas";

type SeriesKey = string;
type AlgoKey = string;

interface SeriesInfo {
    key: SeriesKey;
    seriesName: string;
    x: number;
}

interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: DiffCell["algorithmArgs"] | null;
}

function buildSeriesLabel(info: SeriesInfo): string {
    return `${info.seriesName}\n x = ${info.x}`;
}

function buildArgsSummary(args: DiffCell["algorithmArgs"] | null): string {
    if (!args || Object.keys(args).length === 0) return "";
    const entries = Object.entries(args).sort(([a, b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

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
 * Нормализация состояния стороны:
 *  - none  : пары нет
 *  - ok    : есть пара, нет ошибки
 *  - err0  : ошибка, но n <= 0 или n неизвестно
 *  - errN  : ошибка на шаге n > 0
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
 * Полная классификация по всем случаям из ТЗ.
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

    // 3.1 none / none / появления / исчезновения

    // 1) none → none
    if (statePrev === "none" && stateNext === "none") {
        return {
            bgClass: "bg-slate-900/60",
            borderClass: "border-slate-700/60",
            label: "нет пары ни в одном JSON",
        };
    }

    // 2) none → ok
    if (statePrev === "none" && stateNext === "ok") {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-emerald-300/80",
            label: "пара появилась и сразу ok",
        };
    }

    // 3) ok → none
    if (statePrev === "ok" && stateNext === "none") {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-amber-300/80",
            label: "пара исчезла, раньше была ok",
        };
    }

    // 4) none → err*
    if (statePrev === "none" && (stateNext === "err0" || stateNext === "errN")) {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-red-300/80",
            label: "пара появилась с ошибкой",
        };
    }

    // 5) err* → none
    if ((statePrev === "err0" || statePrev === "errN") && stateNext === "none") {
        return {
            bgClass: "bg-slate-800/60",
            borderClass: "border-emerald-300/80",
            label: "пара исчезла, раньше была с ошибкой",
        };
    }

    // дальше пары, где обе стороны существуют (ok/err)

    // 3.2 ok / ok, ok / err, err / ok

    // 6) ok → ok — золотой
    if (statePrev === "ok" && stateNext === "ok") {
        return {
            bgClass: "bg-amber-400/90 hover:bg-amber-300/90",
            borderClass: "border-amber-200",
            label: "ok → ok (ошибки нет в обоих)",
        };
    }

    // 7) ok → err*
    if (statePrev === "ok" && (stateNext === "err0" || stateNext === "errN")) {
        return {
            bgClass: "bg-purple-500/80 hover:bg-purple-400/80",
            borderClass: "border-purple-200/90",
            label: `ok → error`,
        };
    }

    // 8) err* → ok
    if ((statePrev === "err0" || statePrev === "errN") && stateNext === "ok") {
        return {
            bgClass: "bg-green-500/80 hover:bg-green-400/80",
            borderClass: "border-green-200/90",
            label: `error → ok`,
        };
    }

    // 3.3 err0 / err0 и переходы из 0

    // 9) err0 → err0 — белый
    if (statePrev === "err0" && stateNext === "err0") {
        return {
            bgClass: "bg-white",
            borderClass: "border-border/60",
            label: "error(0) → error(0)",
        };
    }

    // 10) err0 → errN
    if (statePrev === "err0" && stateNext === "errN") {
        return {
            bgClass: "bg-sky-500/70 hover:bg-sky-400/80",
            borderClass: "border-sky-200/90",
            label: `error(0) → error(n=${q ?? 0})`,
        };
    }

    // 11) errN → err0
    if (statePrev === "errN" && stateNext === "err0") {
        return {
            bgClass: "bg-red-500/70 hover:bg-red-400/80",
            borderClass: "border-red-200/90",
            label: `error(n=${p ?? 0}) → error(0)`,
        };
    }

    // 3.4 errN / errN (n > 0)

    // здесь statePrev === "errN" и stateNext === "errN"
    const n1 = p ?? 0;
    const n2 = q ?? 0;

    // 12) n2 = n1 — жёлтый
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

    // 13) n2 > n1 — улучшение (зелёный градиент)
    if (delta > 0) {
        return {
            bgClass: `${GREEN_BY_LEVEL[levelKey]} hover:bg-green-300/80`,
            borderClass: "border-green-200/90",
            label: `error(n=${n1}) → error(n=${n2}), ошибка позже (Δn=+${delta})`,
        };
    }

    // 14) n2 < n1 — ухудшение (красный градиент)
    return {
        bgClass: `${RED_BY_LEVEL[levelKey]} hover:bg-red-300/80`,
        borderClass: "border-red-200/90",
        label: `error(n=${n1}) → error(n=${n2}), ошибка раньше (Δn=${delta})`,
    };
}

export function classifyErrorChange(
    hasPrevErr: boolean,
    nPrev: number | null,
    hasNextErr: boolean,
    nNext: number | null,
): ColorSpec {
    return classifyErrorChangeFull(
        true,
        true,
        hasPrevErr,
        nPrev,
        hasNextErr,
        nNext,
    );
}

export function AlgorithmSeriesDiffHeatmap({
                                               cells,
                                               maxSeries,
                                           }: {
    cells: DiffCell[];
    maxSeries?: number;
}) {
    const containerRef = useRef<HTMLDivElement | null>(null);

    const {seriesList, algoList, cellMap} = useMemo(() => {
        const seriesMap = new Map<SeriesKey, SeriesInfo>();
        const algoMap = new Map<AlgoKey, AlgoInfo>();
        const cellMap = new Map<string, DiffCell>();

        for (const c of cells) {
            if (!seriesMap.has(c.seriesKey)) {
                seriesMap.set(c.seriesKey, {
                    key: c.seriesKey,
                    seriesName: c.seriesName,
                    x: c.x,
                });
            }

            if (!algoMap.has(c.algoKey)) {
                const args = c.algorithmArgs ?? null;
                algoMap.set(c.algoKey, {
                    key: c.algoKey,
                    algorithmName: c.algorithmName,
                    m: c.m,
                    argsSummary: buildArgsSummary(args),
                    algorithmArgs: args,
                });
            }

            const cellKey = `${c.algoKey}||${c.seriesKey}`;
            if (!cellMap.has(cellKey)) {
                cellMap.set(cellKey, c);
            }
        }

        const seriesList = Array.from(seriesMap.values()).sort(
            (a, b) =>
                a.seriesName.localeCompare(b.seriesName) || a.x - b.x,
        );

        const algoList = Array.from(algoMap.values()).sort(
            (a, b) =>
                a.algorithmName.localeCompare(b.algorithmName) ||
                (a.m ?? 0) - (b.m ?? 0),
        );

        return {seriesList, algoList, cellMap};
    }, [cells]);

    const [page, setPage] = useState(0);

    const pageSize =
        maxSeries && maxSeries > 0 ? maxSeries : seriesList.length || 1;

    const totalPages = Math.max(
        1,
        Math.ceil(seriesList.length / pageSize || 1),
    );

    const handleExportPng = async () => {
        const el = containerRef.current;
        if (!el) return;

        const canvas = await html2canvas(el, {
            backgroundColor: "#020617", // твой общий фон (tailwind bg-slate-950 примерно)
            scale: 2,                   // х2 для чёткости
            scrollX: 0,
            scrollY: -window.scrollY,   // чтобы не повлияло прокрученное окно
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

    return (
        <div className="space-y-2">
            <div className="flex items-center justify-between text-xs text-textDim">
                <h2 className="text-sm font-semibold text-textDim">
                    Хитмапа разницы по шагам ошибок: алгоритмы × ряды
                </h2>
                <div className="flex items-center gap-3">
                    <div>
                        Алгоритмы: {algoList.length} · Ряды:{" "}
                        {seriesListShown.length} из {seriesList.length}
                    </div>

                    {/* кнопка экспортa */}
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

            {/* всё, что попадает в PNG */}
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
                                title={buildSeriesLabel(s)}
                            >
                                <div className="flex h-28 w-[40px] flex-col items-center justify-end gap-1">
                                    <div
                                        className="text-[9px] leading-tight"
                                        style={{
                                            writingMode: "vertical-rl",
                                            textOrientation: "mixed",
                                            // чтобы текст не лип к краю
                                            paddingInline: "2px",
                                        }}
                                    >
                                        {s.seriesName}
                                    </div>
                                    <div className="text-[8px] text-textDim/70">
                                        x={s.x}
                                    </div>
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
                                    lines.push(
                                        `Алгоритм: ${algo.algorithmName}`,
                                    );
                                    lines.push(
                                        `m = ${
                                            algo.m != null ? algo.m : "∅"
                                        }`,
                                    );
                                    if (
                                        algo.algorithmArgs &&
                                        Object.keys(
                                            algo.algorithmArgs,
                                        ).length > 0
                                    ) {
                                        lines.push("Аргументы:");
                                        for (const [k, v] of Object.entries(
                                            algo.algorithmArgs,
                                        ).sort(([a, b]) =>
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
                                                ? `m=${algo.m}`
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
                                const cell = cellMap.get(cellKey) ?? null;

                                const hasPrevErr = !!cell?.hasErrorPrev;
                                const hasNextErr = !!cell?.hasErrorNext;
                                const nPrev = cell?.errorNPrev ?? null;
                                const nNext = cell?.errorNNext ?? null;

                                const {bgClass, borderClass, label} =
                                    classifyErrorChange(
                                        hasPrevErr,
                                        nPrev,
                                        hasNextErr,
                                        nNext,
                                    );

                                let deltaText: string | null = null;
                                if (!hasPrevErr && !hasNextErr) {
                                    deltaText = "ok → ok";
                                } else if (hasPrevErr && !hasNextErr) {
                                    deltaText = "err → ok";
                                } else if (!hasPrevErr && hasNextErr) {
                                    deltaText = "ok → err";
                                } else if (nPrev != null && nNext != null) {
                                    const delta = nNext - nPrev;
                                    deltaText = `n: ${nPrev} → ${nNext} (Δ=${delta >= 0 ? "+" : ""}${delta})`;
                                } else {
                                    deltaText = `n: ${
                                        nPrev ?? "∅"
                                    } → ${nNext ?? "∅"}`;
                                }

                                const tooltipLines: string[] = [];

                                tooltipLines.push(
                                    `Ряд: ${s.seriesName}, x=${s.x}`,
                                );
                                tooltipLines.push(
                                    `Алгоритм: ${algo.algorithmName}, m=${algo.m ?? "∅"}`,
                                );
                                tooltipLines.push("");

                                tooltipLines.push(
                                    `Старая ошибка: ${
                                        hasPrevErr
                                            ? nPrev != null
                                                ? `n = ${nPrev}`
                                                : "есть, n неизвестно"
                                            : "нет ошибки"
                                    }`,
                                );
                                tooltipLines.push(
                                    `Новая ошибка: ${
                                        hasNextErr
                                            ? nNext != null
                                                ? `n = ${nNext}`
                                                : "есть, n неизвестно"
                                            : "нет ошибки"
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
