import React from "react";
import { type DetailPoint, type Complex } from "../../model/types";

interface ConvergenceErrorChartProps {
    points: DetailPoint[];
    limit: Complex | null;
}

export const ConvergenceErrorChart: React.FC<ConvergenceErrorChartProps> = ({
    points,
    limit,
}) => {
    const errPoints = points.filter(
        (p) => p.err != null && Number.isFinite(p.err) && (p.err as number) >= 0
    );

    const hasErrorCurve = limit != null && errPoints.length > 0;

    if (!hasErrorCurve) {
        return (
            <div className="mb-3 text-[11px] text-amber-300/80">
                Невозможно построить график ошибки: нет предела или нет корректных значений
                |Aₙ − lim|.
            </div>
        );
    }

    let errMinN = errPoints[0].n;
    let errMaxN = errPoints[errPoints.length - 1].n;

    let errMinE = errPoints[0].err as number;
    let errMaxE = errPoints[0].err as number;

    for (const p of errPoints) {
        const e = p.err as number;
        if (e < errMinE) errMinE = e;
        if (e > errMaxE) errMaxE = e;
    }

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

    const xScale = (n: number) =>
        marginLeft + (innerW * (n - errMinN)) / (errMaxN - errMinN || 1);
    const yScale = (e: number) =>
        marginTop + innerH - (innerH * (e - errMinE)) / (errMaxE - errMinE || 1);

    const pathD = errPoints
        .map((p, idx) => {
            const x = xScale(p.n);
            const y = yScale(p.err as number);
            return `${idx === 0 ? "M" : "L"} ${x.toFixed(2)} ${y.toFixed(2)}`;
        })
        .join(" ");

    const circles = errPoints.map((p, idx) => {
        const x = xScale(p.n);
        const y = yScale(p.err as number);
        return { x, y, p, key: idx };
    });

    return (
        <div className="mb-3">
            <svg
                viewBox="0 0 600 220"
                className="w-full rounded bg-surface/80"
                preserveAspectRatio="none"
            >
                <rect x={0} y={0} width={600} height={220} className="fill-surface/80" />
                <line x1={40} y1={20} x2={40} y2={195} className="stroke-border" strokeWidth={0.5} />
                <line
                    x1={40}
                    y1={195}
                    x2={590}
                    y2={195}
                    className="stroke-border"
                    strokeWidth={0.5}
                />

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
            <div className="mt-1 text-[10px] text-textDim/70">
                График |Aₙ − lim| по n. При наведении на точки видно номер шага и значение ошибки.
            </div>
        </div>
    );
};
