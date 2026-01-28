// src/widgets/AlgorithmSeriesMatrixTable/ui/AlgorithmSeriesDetailChart.tsx

import React, { useMemo } from "react";
import type {
    Accel,
    Complex,
    ScalarArg,
    Series,
    SeriesAccel,
    SeriesAccelComputedPoint,
    SeriesAccelError,
    SeriesAccelEvent,
    SeriesComputedPoint,
} from "@/entities/experiment/model/experiment.ts";

type SelectedCell = { accelId: string; seriesId: string };

export interface AlgorithmSeriesDetailChartProps {
    experimentId: string;
    selectedCell: SelectedCell;
    selectedDetail: {
        series: Series | null;
        accel: Accel | null;
        seriesAccel: SeriesAccel | null;
    } | null;

    onScrollBackToSelectedCell: () => void;
}

function formatScalar(x: ScalarArg): string {
    if (x === null) return "null";
    if (typeof x === "string") return JSON.stringify(x);
    if (typeof x === "number") return Number.isFinite(x) ? String(x) : String(x);
    if (typeof x === "boolean") return x ? "true" : "false";
    return String(x);
}

function formatComplex(z: Complex | null): string {
    if (z === null) return "null";
    const re = z.re === null ? "null" : String(z.re);
    const im = z.im === null ? "null" : String(z.im);
    return `{re:${re}, im:${im}}`;
}

function formatComplexLike(z: { re: number | null; im: number | null } | null): string {
    if (z === null) return "null";
    const re = z.re === null ? "null" : String(z.re);
    const im = z.im === null ? "null" : String(z.im);
    return `{re:${re}, im:${im}}`;
}

function argsToLines(args: Record<string, ScalarArg> | null | undefined): string[] {
    if (!args) return [];
    const keys = Object.keys(args).sort();
    return keys.map((k) => `${k} = ${formatScalar(args[k])}`);
}

type NStats = {
    count: number;
    minN: number | null;
    maxN: number | null;
};

function nStatsFromNs(ns: number[]): NStats {
    if (ns.length === 0) return { count: 0, minN: null, maxN: null };
    let minN = ns[0];
    let maxN = ns[0];
    for (const n of ns) {
        if (n < minN) minN = n;
        if (n > maxN) maxN = n;
    }
    return { count: ns.length, minN, maxN };
}

function previewSlices<T>(
    arr: T[],
    head = 20,
    tail = 20
): { head: T[]; tail: T[]; hasGap: boolean } {
    if (arr.length <= head + tail) return { head: arr, tail: [], hasGap: false };
    return { head: arr.slice(0, head), tail: arr.slice(-tail), hasGap: true };
}

type ComputedStats = {
    total: number;
    nonNullValues: number;
    n: NStats;
    lastNonNull: { n: number; value: string } | null;
    deviation: { count: number; min: number | null; max: number | null } | null;
};

function seriesComputedStats(points: SeriesComputedPoint[] | undefined): ComputedStats {
    const pts = points ?? [];
    let nonNull = 0;
    const ns: number[] = [];
    let last: { n: number; value: string } | null = null;

    let devCount = 0;
    let devMin: number | null = null;
    let devMax: number | null = null;

    for (const p of pts) {
        ns.push(p.n);
        if (p.value !== null) {
            nonNull += 1;
            last = { n: p.n, value: formatComplex(p.value) };
        }
        if (p.deviation !== undefined && p.deviation !== null) {
            devCount += 1;
            if (devMin === null || p.deviation < devMin) devMin = p.deviation;
            if (devMax === null || p.deviation > devMax) devMax = p.deviation;
        }
    }

    return {
        total: pts.length,
        nonNullValues: nonNull,
        n: nStatsFromNs(ns),
        lastNonNull: last,
        deviation: { count: devCount, min: devMin, max: devMax },
    };
}

function seriesAccelComputedStats(points: SeriesAccelComputedPoint[] | undefined): ComputedStats {
    const pts = points ?? [];
    let nonNull = 0;
    const ns: number[] = [];
    let last: { n: number; value: string } | null = null;

    let devCount = 0;
    let devMin: number | null = null;
    let devMax: number | null = null;

    for (const p of pts) {
        ns.push(p.n);
        if (p.value !== null) {
            nonNull += 1;
            last = { n: p.n, value: formatComplexLike(p.value) };
        }
        if (p.deviation !== undefined && p.deviation !== null) {
            devCount += 1;
            if (devMin === null || p.deviation < devMin) devMin = p.deviation;
            if (devMax === null || p.deviation > devMax) devMax = p.deviation;
        }
    }

    return {
        total: pts.length,
        nonNullValues: nonNull,
        n: nStatsFromNs(ns),
        lastNonNull: last,
        deviation: { count: devCount, min: devMin, max: devMax },
    };
}

function smallNumber(x: number | null | undefined): string {
    if (x === null || x === undefined) return "—";
    return String(x);
}

function JsonBlock({ value }: { value: unknown }) {
    return (
        <pre className="max-h-[360px] overflow-auto rounded border border-border/60 bg-surface p-2 text-[11px] leading-[1.35] text-textDim">
            {JSON.stringify(value, null, 2)}
        </pre>
    );
}

function SectionTitle({ children }: { children: React.ReactNode }) {
    return <div className="text-[12px] font-semibold">{children}</div>;
}

function KV({ k, v }: { k: React.ReactNode; v: React.ReactNode }) {
    return (
        <div className="grid grid-cols-[160px_1fr] gap-2">
            <div className="text-textDim">{k}</div>
            <div className="font-mono break-words">{v}</div>
        </div>
    );
}

function ComputedTableSeries({ points }: { points: SeriesComputedPoint[] }) {
    const { head, tail, hasGap } = previewSlices(points, 20, 20);
    return (
        <div className="overflow-auto rounded border border-border/60">
            <table className="w-full text-[11px]">
                <thead className="bg-surface">
                    <tr className="text-left text-textDim">
                        <th className="px-2 py-1 font-medium">n</th>
                        <th className="px-2 py-1 font-medium">re</th>
                        <th className="px-2 py-1 font-medium">im</th>
                    </tr>
                </thead>
                <tbody>
                    {head.map((p, idx) => (
                        <tr key={`h-${idx}-${p.n}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{p.n}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.re ?? "null"}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.im ?? "null"}</td>
                        </tr>
                    ))}
                    {hasGap && (
                        <tr className="border-t border-border/60">
                            <td className="px-2 py-1 text-textDim" colSpan={3}>
                                …
                            </td>
                        </tr>
                    )}
                    {tail.map((p, idx) => (
                        <tr key={`t-${idx}-${p.n}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{p.n}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.re ?? "null"}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.im ?? "null"}</td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
}

function ComputedTableSeriesAccel({ points }: { points: SeriesAccelComputedPoint[] }) {
    const { head, tail, hasGap } = previewSlices(points, 20, 20);
    return (
        <div className="overflow-auto rounded border border-border/60">
            <table className="w-full text-[11px]">
                <thead className="bg-surface">
                    <tr className="text-left text-textDim">
                        <th className="px-2 py-1 font-medium">n</th>
                        <th className="px-2 py-1 font-medium">re</th>
                        <th className="px-2 py-1 font-medium">im</th>
                        <th className="px-2 py-1 font-medium">deviation</th>
                    </tr>
                </thead>
                <tbody>
                    {head.map((p, idx) => (
                        <tr key={`h-${idx}-${p.n}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{p.n}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.re ?? "null"}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.im ?? "null"}</td>
                            <td className="px-2 py-1 font-mono">{p.deviation ?? "—"}</td>
                        </tr>
                    ))}
                    {hasGap && (
                        <tr className="border-t border-border/60">
                            <td className="px-2 py-1 text-textDim" colSpan={4}>
                                …
                            </td>
                        </tr>
                    )}
                    {tail.map((p, idx) => (
                        <tr key={`t-${idx}-${p.n}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{p.n}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.re ?? "null"}</td>
                            <td className="px-2 py-1 font-mono">{p.value?.im ?? "null"}</td>
                            <td className="px-2 py-1 font-mono">{p.deviation ?? "—"}</td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
}

function ErrorsTable({ errors }: { errors: SeriesAccelError[] }) {
    const { head, tail, hasGap } = previewSlices(errors, 20, 20);
    return (
        <div className="overflow-auto rounded border border-border/60">
            <table className="w-full text-[11px]">
                <thead className="bg-surface">
                    <tr className="text-left text-textDim">
                        <th className="px-2 py-1 font-medium">n</th>
                        <th className="px-2 py-1 font-medium">message</th>
                    </tr>
                </thead>
                <tbody>
                    {head.map((e, idx) => (
                        <tr key={`h-${idx}-${e.n}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{e.n}</td>
                            <td className="px-2 py-1 font-mono break-words">{e.message}</td>
                        </tr>
                    ))}
                    {hasGap && (
                        <tr className="border-t border-border/60">
                            <td className="px-2 py-1 text-textDim" colSpan={2}>
                                …
                            </td>
                        </tr>
                    )}
                    {tail.map((e, idx) => (
                        <tr key={`t-${idx}-${e.n}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{e.n}</td>
                            <td className="px-2 py-1 font-mono break-words">{e.message}</td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
}

function EventsTable({ events }: { events: SeriesAccelEvent[] }) {
    const { head, tail, hasGap } = previewSlices(events, 20, 20);
    return (
        <div className="overflow-auto rounded border border-border/60">
            <table className="w-full text-[11px]">
                <thead className="bg-surface">
                    <tr className="text-left text-textDim">
                        <th className="px-2 py-1 font-medium">n</th>
                        <th className="px-2 py-1 font-medium">name</th>
                        <th className="px-2 py-1 font-medium">description</th>
                    </tr>
                </thead>
                <tbody>
                    {head.map((e, idx) => (
                        <tr key={`h-${idx}-${e.n}-${e.name}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{e.n}</td>
                            <td className="px-2 py-1 font-mono break-words">{e.name}</td>
                            <td className="px-2 py-1 font-mono break-words">{e.description}</td>
                        </tr>
                    ))}
                    {hasGap && (
                        <tr className="border-t border-border/60">
                            <td className="px-2 py-1 text-textDim" colSpan={3}>
                                …
                            </td>
                        </tr>
                    )}
                    {tail.map((e, idx) => (
                        <tr key={`t-${idx}-${e.n}-${e.name}`} className="border-t border-border/60">
                            <td className="px-2 py-1 font-mono">{e.n}</td>
                            <td className="px-2 py-1 font-mono break-words">{e.name}</td>
                            <td className="px-2 py-1 font-mono break-words">{e.description}</td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
}

export const AlgorithmSeriesDetailChart: React.FC<AlgorithmSeriesDetailChartProps> = ({
    experimentId,
    selectedCell,
    selectedDetail,
    onScrollBackToSelectedCell,
}) => {
    const series = selectedDetail?.series ?? null;
    const accel = selectedDetail?.accel ?? null;
    const seriesAccel = selectedDetail?.seriesAccel ?? null;

    const seriesStats = useMemo(() => seriesComputedStats(series?.computed), [series?.computed]);
    const comboStats = useMemo(
        () => seriesAccelComputedStats(seriesAccel?.computed),
        [seriesAccel?.computed]
    );

    const accelArgsLines = useMemo(() => argsToLines(accel?.args ?? null), [accel?.args]);
    const seriesArgsLines = useMemo(() => argsToLines(series?.args ?? null), [series?.args]);

    const errors = seriesAccel?.errors ?? [];
    const events = seriesAccel?.events ?? [];

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 shadow-panel">
            <div className="mb-3 flex items-center justify-between gap-3">
                <div className="text-sm font-medium">Detail: experiment × (series, accel)</div>
                <button
                    type="button"
                    className="rounded border border-border bg-surface px-2 py-[2px] text-[10px] text-textDim hover:bg-panel"
                    onClick={onScrollBackToSelectedCell}
                >
                    Вернуться к выбранной ячейке
                </button>
            </div>

            <div className="grid gap-2 text-[12px]">
                <KV k="experimentId" v={experimentId} />
                <KV k="accelId (selected)" v={selectedCell.accelId} />
                <KV k="seriesId (selected)" v={selectedCell.seriesId} />

                <div className="mt-2 h-px bg-border/60" />

                <SectionTitle>Accel</SectionTitle>
                <KV k="id" v={accel?.id ?? "—"} />
                <KV k="name" v={accel?.name ?? "—"} />
                <KV k="m" v={accel?.m ?? "—"} />
                <KV k="args (count)" v={accelArgsLines.length} />
                {accelArgsLines.length > 0 && (
                    <div className="rounded border border-border/60 bg-surface p-2 font-mono text-[11px] text-textDim">
                        {accelArgsLines.map((x) => (
                            <div key={x}>{x}</div>
                        ))}
                    </div>
                )}
                <details className="rounded border border-border/60 bg-panel p-2">
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        Accel: raw JSON
                    </summary>
                    <div className="mt-2">
                        <JsonBlock value={accel} />
                    </div>
                </details>

                <div className="mt-2 h-px bg-border/60" />

                <SectionTitle>Series</SectionTitle>
                <KV k="id" v={series?.id ?? "—"} />
                <KV k="name" v={series?.name ?? "—"} />
                <KV k="precision" v={series?.precision ?? "—"} />
                <KV k="limit" v={series ? formatComplex(series.limit) : "—"} />
                <KV k="args (count)" v={seriesArgsLines.length} />
                {seriesArgsLines.length > 0 && (
                    <div className="rounded border border-border/60 bg-surface p-2 font-mono text-[11px] text-textDim">
                        {seriesArgsLines.map((x) => (
                            <div key={x}>{x}</div>
                        ))}
                    </div>
                )}

                <details className="rounded border border-border/60 bg-panel p-2">
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        Series.computed: stats + preview
                    </summary>
                    <div className="mt-2 grid gap-2">
                        <KV k="points" v={seriesStats.total} />
                        <KV k="nonNull values" v={seriesStats.nonNullValues} />
                        <KV
                            k="n range"
                            v={
                                seriesStats.n.minN === null
                                    ? "—"
                                    : `[${seriesStats.n.minN}, ${seriesStats.n.maxN}]`
                            }
                        />
                        <KV
                            k="last nonNull"
                            v={
                                seriesStats.lastNonNull
                                    ? `n=${seriesStats.lastNonNull.n} value=${seriesStats.lastNonNull.value}`
                                    : "—"
                            }
                        />
                        {series?.computed && series.computed.length > 0 ? (
                            <ComputedTableSeries points={series.computed} />
                        ) : (
                            <div className="text-[11px] text-textDim">
                                Series.computed отсутствует.
                            </div>
                        )}
                    </div>
                </details>

                <details className="rounded border border-border/60 bg-panel p-2">
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        Series: raw JSON
                    </summary>
                    <div className="mt-2">
                        <JsonBlock value={series} />
                    </div>
                </details>

                <div className="mt-2 h-px bg-border/60" />

                <SectionTitle>SeriesAccel (combination)</SectionTitle>
                <KV k="exists in seriesAccelList" v={seriesAccel ? "true" : "false"} />
                {seriesAccel && (
                    <>
                        <KV k="series_id" v={seriesAccel.series_id} />
                        <KV k="accel_id" v={seriesAccel.accel_id} />
                    </>
                )}
                {!seriesAccel && (
                    <div className="text-[11px] text-textDim">
                        Для выбранной пары нет объекта SeriesAccel (computed/errors/events
                        недоступны).
                    </div>
                )}

                <details className="rounded border border-border/60 bg-panel p-2" open>
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        SeriesAccel.computed: stats + preview
                    </summary>
                    <div className="mt-2 grid gap-2">
                        <KV k="points" v={comboStats.total} />
                        <KV k="nonNull values" v={comboStats.nonNullValues} />
                        <KV
                            k="n range"
                            v={
                                comboStats.n.minN === null
                                    ? "—"
                                    : `[${comboStats.n.minN}, ${comboStats.n.maxN}]`
                            }
                        />
                        <KV
                            k="last nonNull"
                            v={
                                comboStats.lastNonNull
                                    ? `n=${comboStats.lastNonNull.n} value=${comboStats.lastNonNull.value}`
                                    : "—"
                            }
                        />
                        <KV
                            k="deviation stats"
                            v={
                                comboStats.deviation
                                    ? `count=${comboStats.deviation.count}, min=${smallNumber(
                                          comboStats.deviation.min
                                      )}, max=${smallNumber(comboStats.deviation.max)}`
                                    : "—"
                            }
                        />

                        {seriesAccel?.computed && seriesAccel.computed.length > 0 ? (
                            <ComputedTableSeriesAccel points={seriesAccel.computed} />
                        ) : (
                            <div className="text-[11px] text-textDim">
                                SeriesAccel.computed отсутствует или пуст.
                            </div>
                        )}
                    </div>
                </details>

                <details className="rounded border border-border/60 bg-panel p-2">
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        SeriesAccel.errors: count + preview
                    </summary>
                    <div className="mt-2 grid gap-2">
                        <KV k="errors" v={errors.length} />
                        {errors.length > 0 ? (
                            <ErrorsTable errors={errors} />
                        ) : (
                            <div className="text-[11px] text-textDim">errors пуст.</div>
                        )}
                    </div>
                </details>

                <details className="rounded border border-border/60 bg-panel p-2">
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        SeriesAccel.events: count + preview
                    </summary>
                    <div className="mt-2 grid gap-2">
                        <KV k="events" v={events.length} />
                        {events.length > 0 ? (
                            <EventsTable events={events} />
                        ) : (
                            <div className="text-[11px] text-textDim">events пуст.</div>
                        )}
                    </div>
                </details>

                <details className="rounded border border-border/60 bg-panel p-2">
                    <summary className="cursor-pointer text-[11px] text-textDim">
                        SeriesAccel: raw JSON
                    </summary>
                    <div className="mt-2">
                        <JsonBlock value={seriesAccel} />
                    </div>
                </details>
            </div>
        </div>
    );
};
