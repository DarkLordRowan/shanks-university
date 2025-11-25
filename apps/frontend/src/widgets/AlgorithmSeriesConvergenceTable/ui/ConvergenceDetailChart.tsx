import React from "react";
import { type SelectedDetail, type DetailPoint } from "../model/types";
import { formatMonotonicityShort, formatSideShort } from "../model/convergenceUtils";

function buildErrorChart(points: DetailPoint[]) {
    const errPoints = points.filter(
        (p) => p.err != null && Number.isFinite(p.err) && (p.err as number) >= 0
    );

    if (!errPoints.length) {
        return { chart: null as React.ReactNode, hasCurve: false };
    }

    let errMinN = errPoints[0].n;
    let errMaxN = errPoints[errPoints.length - 1].n;

    let errMinE = errPoints.reduce(
        (acc, p) => Math.min(acc, p.err as number),
        errPoints[0].err as number
    );
    let errMaxE = errPoints.reduce(
        (acc, p) => Math.max(acc, p.err as number),
        errPoints[0].err as number
    );

    if (errMaxE === errMinE) {
        errMaxE = errMinE + 1;
    }

    const width = 600;
    const height = 220;
    const marginLeft = 40;
    const marginRight = 10;
    const marginTop = 10;
    const marginBottom = 25;

    const innerW = width - marginLeft - marginRight;
    const innerH = height - marginTop - marginBottom;

    const xScale = (n: number) => marginLeft + (innerW * (n - errMinN)) / (errMaxN - errMinN || 1);
    const yScale = (e: number) =>
        marginTop + innerH - (innerH * (e - errMinE)) / (errMaxE - errMinE || 1);

    const parts: string[] = [];
    for (let i = 0; i < errPoints.length; i++) {
        const p = errPoints[i];
        const x = xScale(p.n);
        const y = yScale(p.err as number);
        parts.push(`${i === 0 ? "M" : "L"} ${x.toFixed(2)} ${y.toFixed(2)}`);
    }
    const pathD = parts.join(" ");

    const circles = errPoints.map((p, idx) => {
        const x = xScale(p.n);
        const y = yScale(p.err as number);
        return { x, y, p, key: idx };
    });

    const chart = (
        <svg
            viewBox="0 0 600 220"
            className="w-full rounded bg-surface/80"
            preserveAspectRatio="none"
        >
            <rect x={0} y={0} width={600} height={220} className="fill-surface/80" />
            <line x1={40} y1={20} x2={40} y2={195} className="stroke-border" strokeWidth={0.5} />
            <line x1={40} y1={195} x2={590} y2={195} className="stroke-border" strokeWidth={0.5} />

            <path d={pathD} className="stroke-primary" strokeWidth={1.2} fill="none" />

            {circles.map((c) => (
                <circle key={c.key} cx={c.x} cy={c.y} r={5} className="fill-primary">
                    <title>
                        {`n=${c.p.n}, |Aₙ − lim|≈${(c.p.err as number).toExponential(4)}`}
                    </title>
                </circle>
            ))}

            <text x={320} y={210} className="fill-textDim/70 text-[9px]" textAnchor="middle">
                n (номер шага)
            </text>
            <text x={15} y={15} className="fill-textDim/70 text-[9px]" textAnchor="start">
                |Aₙ − lim|
            </text>

            <text x={45} y={207} className="fill-textDim/60 text-[8px]" textAnchor="start">
                n={errMinN}
            </text>
            <text x={585} y={207} className="fill-textDim/60 text-[8px]" textAnchor="end">
                n={errMaxN}
            </text>

            <text x={45} y={32} className="fill-textDim/60 text-[8px]" textAnchor="start">
                err≈{errMinE.toExponential(2)}
            </text>
            <text x={45} y={190} className="fill-textDim/60 text-[8px]" textAnchor="start">
                err≈{errMaxE.toExponential(2)}
            </text>
        </svg>
    );

    return { chart, hasCurve: true };
}

export function ConvergenceDetailChart({ detail }: { detail: SelectedDetail }) {
    const { seriesInfo, algoInfo, analysis, limit, points } = detail;

    if (!seriesInfo || !algoInfo || !analysis) {
        return null;
    }

    if (!points.length) {
        return (
            <div className="mt-4 rounded-xl border border-border bg-panel p-3 text-xs text-textDim">
                Для выбранной пары нет вычисленных точек.
            </div>
        );
    }

    const { chart: errChart, hasCurve: hasErrorCurve } = buildErrorChart(points);

    const shortSide = formatSideShort(analysis.side);
    const shortMon = formatMonotonicityShort(analysis.monotonicity);

    return (
        <div className="mt-4 rounded-xl border border-border bg-panel p-4 shadow-panel text-xs text-textDim">
            <div className="mb-3 flex flex-wrap justify-between gap-3">
                <div>
                    <div className="text-sm font-semibold text-textDim">
                        Детальный график сходимости
                    </div>
                    <div className="mt-1 space-y-0.5 text-[11px] text-textDim/80">
                        <div>
                            Ряд: {seriesInfo.seriesName}, x={seriesInfo.xLabel}, prec=
                            {seriesInfo.precision}
                        </div>
                        <div>
                            Алгоритм: {algoInfo.algorithmName}
                            {algoInfo.m != null ? `, m=${algoInfo.m}` : ""}
                        </div>
                        {algoInfo.argsSummary && <div>Аргументы: {algoInfo.argsSummary}</div>}
                    </div>
                </div>
                <div className="space-y-1 text-[11px] text-textDim/80">
                    <div>
                        Тип: {shortSide} | {shortMon}
                    </div>
                    <div>
                        Смен знака: {analysis.signChangesCount}
                        {analysis.firstSignChangeN != null
                            ? `, первая при n=${analysis.firstSignChangeN}`
                            : ""}
                    </div>
                    <div>
                        Первый рост ошибки:{" "}
                        {analysis.firstGrowthN != null ? `n=${analysis.firstGrowthN}` : "не был"}
                    </div>
                    <div>Сравнено шагов (пар): {analysis.stepsAnalyzed}</div>
                </div>
            </div>

            {/* График |A_n - lim| */}
            {limit && hasErrorCurve ? (
                <div className="mb-3">
                    {errChart}
                    <div className="mt-1 text-[10px] text-textDim/70">
                        График |Aₙ − lim| по n. При наведении на точки видно номер шага и значение
                        ошибки.
                    </div>
                </div>
            ) : (
                <div className="mb-3 text-[11px] text-amber-300/80">
                    Невозможно построить график ошибки: нет предела или нет корректных значений |Aₙ
                    − lim|.
                </div>
            )}

            {/* Таблица значений A_n и ошибки */}
            <div className="mt-2 max-h-64 overflow-auto rounded border border-border bg-surface/60">
                <table className="min-w-full border-collapse text-[10px]">
                    <thead className="bg-surface/80">
                        <tr>
                            <th className="border-b border-border px-2 py-1 text-left">n</th>
                            <th className="border-b border-border px-2 py-1 text-left">Re(Aₙ)</th>
                            <th className="border-b border-border px-2 py-1 text-left">Im(Aₙ)</th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                |Aₙ − lim|
                            </th>
                            <th className="border-b border-border px-2 py-1 text-left">
                                sgn(Re(Aₙ − lim))
                            </th>
                        </tr>
                    </thead>
                    <tbody>
                        {points.slice(0, 200).map((p) => (
                            <tr key={p.n} className="odd:bg-surface/40 even:bg-surface/20">
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.n}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.valueRe != null && Number.isFinite(p.valueRe)
                                        ? p.valueRe.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.valueIm != null && Number.isFinite(p.valueIm)
                                        ? p.valueIm.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.err != null && Number.isFinite(p.err)
                                        ? p.err.toExponential(4)
                                        : "∅"}
                                </td>
                                <td className="border-t border-border px-2 py-1 font-mono">
                                    {p.sign === 1 ? "+" : p.sign === -1 ? "−" : "0 / ?"}
                                </td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
            {points.length > 200 && (
                <div className="mt-1 text-[10px] text-textDim/60">
                    Показаны первые 200 точек из {points.length}.
                </div>
            )}
            {/* Таблица разностей A_n - A_{n-1} */}
            <div className="mt-4 border-t border-border/60 pt-3">
                <div className="mb-2 text-[11px] font-semibold text-textDim">
                    Разности Aₙ − Aₙ₋₁
                </div>

                {points.some((p) => p.diffNorm != null) ? (
                    <div className="max-h-64 overflow-auto rounded border border-border bg-surface/60">
                        <table className="min-w-full border-collapse text-[10px]">
                            <thead className="bg-surface/80">
                                <tr>
                                    <th className="border-b border-border px-2 py-1 text-left">
                                        n
                                    </th>
                                    <th className="border-b border-border px-2 py-1 text-left">
                                        Re(Aₙ − Aₙ₋₁)
                                    </th>
                                    <th className="border-b border-border px-2 py-1 text-left">
                                        Im(Aₙ − Aₙ₋₁)
                                    </th>
                                    <th className="border-b border-border px-2 py-1 text-left">
                                        |Aₙ − Aₙ₋₁|
                                    </th>
                                </tr>
                            </thead>
                            <tbody>
                                {points
                                    .filter((p) => p.diffNorm != null)
                                    .slice(0, 200)
                                    .map((p) => (
                                        <tr
                                            key={`diff-${p.n}`}
                                            className="odd:bg-surface/40 even:bg-surface/20"
                                        >
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {p.n}
                                            </td>
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {p.diffRe != null && Number.isFinite(p.diffRe)
                                                    ? p.diffRe.toExponential(4)
                                                    : "∅"}
                                            </td>
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {p.diffIm != null && Number.isFinite(p.diffIm)
                                                    ? p.diffIm.toExponential(4)
                                                    : "∅"}
                                            </td>
                                            <td className="border-t border-border px-2 py-1 font-mono">
                                                {p.diffNorm != null && Number.isFinite(p.diffNorm)
                                                    ? p.diffNorm.toExponential(4)
                                                    : "∅"}
                                            </td>
                                        </tr>
                                    ))}
                            </tbody>
                        </table>
                    </div>
                ) : (
                    <div className="text-[11px] text-textDim/70">
                        Недостаточно данных для вычисления разностей Aₙ − Aₙ₋₁.
                    </div>
                )}
            </div>

        </div>
    );
}
