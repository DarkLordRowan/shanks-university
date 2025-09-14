import React from "react";
import { type XY } from "../types";
import { linePath, niceLogTicks, niceTicks } from "../utils/chartUtils";

type Series = { label: string; data: XY[]; color: string; dashed?: boolean };
type HLine = { y: number; label?: string; color?: string; dashed?: boolean };

export const SimpleLineChart: React.FC<{
    width?: number;
    height?: number;
    title: string;
    xLabel?: string;
    yLabel?: string;
    series: Series[];
    horizontalLines?: HLine[];
    logY?: boolean;
    annotateMaxText?: string | ((v: number) => string);
}> = ({
          width = 420,
          height = 220,
          title,
          xLabel,
          yLabel,
          series,
          horizontalLines = [],
          logY = false,
          annotateMaxText,
      }) => {
    const PADL = 40,
        PADR = 8,
        PADT = 24,
        PADB = 30;
    const W = width,
        H = height;
    const all = series.flatMap((s) => s.data);
    if (!all.length) return null;

    const minX = Math.min(...all.map((p) => p.x));
    const maxX = Math.max(...all.map((p) => p.x));
    const xScale = (x: number) => {
        if (maxX === minX) return PADL + (W - PADL - PADR) / 2;
        return PADL + ((x - minX) / (maxX - minX)) * (W - PADL - PADR);
    };

    const allY = series.flatMap((s) => s.data.map((d) => d.y)).concat(horizontalLines.map((l) => l.y));
    let minY = Math.min(...allY),
        maxY = Math.max(...allY);
    if (!logY) {
        const pad = (maxY - minY) * 0.05 || 1;
        minY -= pad;
        maxY += pad;
    } else {
        const pos = allY.filter((v) => v > 0);
        const lo = Math.min(...pos);
        const hi = Math.max(...pos);
        minY = lo;
        maxY = hi;
    }

    const yLinear = (y: number) => {
        if (maxY === minY) return H - PADB - (H - PADT - PADB) / 2;
        return H - PADB - ((y - minY) / (maxY - minY)) * (H - PADT - PADB);
    };
    const yLog = (y: number) => {
        const ly = Math.log10(y),
            lmin = Math.log10(minY),
            lmax = Math.log10(maxY);
        if (lmax === lmin) return H - PADB - (H - PADT - PADB) / 2;
        return H - PADB - ((ly - lmin) / (lmax - lmin)) * (H - PADT - PADB);
    };
    const yScale = logY ? yLog : yLinear;

    const xticks = niceTicks(minX, maxX, 6);
    const yticks = logY ? niceLogTicks(minY, maxY) : niceTicks(minY, maxY, 5);

    const Legend: React.FC = () => (
        <div className="mt-1 flex flex-wrap items-center gap-3 text-[11px] text-textDim">
            {series.map((s) => (
                <span key={s.label} className="inline-flex items-center gap-1">
                    <span style={{background: s.color}} className="inline-block h-2 w-3 rounded-sm"/>
                    {s.label}
                </span>
            ))}
        </div>
    );

    let maxPoint: XY | null = null;
    if (annotateMaxText) {
        const allPts = series.flatMap((s) => s.data);
        if (allPts.length) {
            maxPoint = allPts.reduce((a, b) => (a.y > b.y ? a : b));
        }
    }

    return (
        <div className="rounded-lg border border-border/60 bg-surface p-2">
            <div className="mb-1 text-sm">{title}</div>
            <div className="relative w-full" style={{aspectRatio: `${W}/${H}`}}>
                <svg viewBox={`0 0 ${W} ${H}`} preserveAspectRatio="xMidYMid meet"
                     className="absolute inset-0 block h-full w-full">
                    {/* оси */}
                    <path d={`M${PADL},${H - PADB} L${W - PADR},${H - PADB}`} stroke="currentColor"
                          strokeOpacity="0.4"/>
                    <path d={`M${PADL},${PADT} L${PADL},${H - PADB}`} stroke="currentColor" strokeOpacity="0.4"/>
                    {/* сетка */}
                    {xticks.map((t, i) => (
                        <g key={`x${i}`}>
                            <line x1={xScale(t)} y1={PADT} x2={xScale(t)} y2={H - PADB} stroke="currentColor"
                                  strokeOpacity="0.08"/>
                            <text x={xScale(t)} y={H - PADB + 14} textAnchor="middle"
                                  className="fill-current text-[10px]">
                                {t}
                            </text>
                        </g>
                    ))}
                    {yticks.map((t, i) => (
                        <g key={`y${i}`}>
                            <line x1={PADL} y1={yScale(t)} x2={W - PADR} y2={yScale(t)} stroke="currentColor"
                                  strokeOpacity="0.08"/>
                            <text x={PADL - 6} y={yScale(t) + 3} textAnchor="end" className="fill-current text-[10px]">
                                {logY ? `1e${Math.log10(t).toFixed(0)}` : t}
                            </text>
                        </g>
                    ))}

                    {/* горизонтальные линии (предел) */}
                    {horizontalLines.map((l, i) => (
                        <g key={`h${i}`}>
                            <line
                                x1={PADL}
                                x2={W - PADR}
                                y1={yScale(l.y)}
                                y2={yScale(l.y)}
                                stroke={l.color ?? "#ef4444"}
                                strokeDasharray={l.dashed ? "6 4" : undefined}
                                strokeWidth={1.2}
                            />
                            {l.label && (
                                <text x={W - PADR - 4} y={yScale(l.y) - 4} textAnchor="end"
                                      className="fill-current text-[10px] opacity-80">
                                    {l.label}
                                </text>
                            )}
                        </g>
                    ))}

                    {/* линии серий */}
                    {series.map((s, idx) => (
                        <g key={idx}>
                            <path d={linePath(s.data, xScale, yScale)} fill="none" stroke={s.color} strokeWidth={1.8}
                                  strokeDasharray={s.dashed ? "6 4" : undefined}/>
                            {s.data.map((p, i) => (
                                <circle key={i} cx={xScale(p.x)} cy={yScale(p.y)} r={2.2} fill={s.color}/>
                            ))}
                        </g>
                    ))}

                    {/* подписи осей */}
                    {xLabel && (
                        <text x={(PADL + W - PADR) / 2} y={H - 6} textAnchor="middle"
                              className="fill-current text-[10px]">
                            {xLabel}
                        </text>
                    )}
                    {yLabel && (
                        <text
                            x={12}
                            y={(PADT + H - PADB) / 2}
                            transform={`rotate(-90 12 ${(PADT + H - PADB) / 2})`}
                            textAnchor="middle"
                            className="fill-current text-[10px]"
                        >
                            {yLabel}
                        </text>
                    )}

                    {/* аннотация максимума */}
                    {annotateMaxText &&
                        maxPoint &&
                        (() => {
                            const txt = typeof annotateMaxText === "function" ? annotateMaxText(maxPoint!.y) : annotateMaxText;
                            const bx = Math.min(W - PADR - 76, Math.max(PADL, xScale(maxPoint!.x) + 6));
                            const by = Math.max(PADT + 14, yScale(maxPoint!.y) - 6);
                            return (
                                <>
                                    <rect x={bx} y={by - 12} width="76" height="16" rx="3" className="fill-current"
                                          opacity="0.12"/>
                                    <text x={bx + 4} y={by - 0.5} className="fill-current text-[10px]">
                                        {txt}
                                    </text>
                                </>
                            );
                        })()}
                </svg>
            </div>
            <Legend/>
        </div>
    );
};
