import { useMemo, useState } from "react";
import type { Item } from "../types/item";

// ваш StepsToToleranceBar из предыдущего сообщения
import { StepsToToleranceBar } from "./StepsToToleranceBar";

type Props = {
    items: Item[];
    defaultEps?: number;     // начальное ε
    pMin?: number;           // min(log10 ε), по умолчанию -16
    pMax?: number;           // max(log10 ε), по умолчанию -1
};

function clamp(n: number, lo: number, hi: number) {
    return Math.max(lo, Math.min(hi, n));
}

export function StepsToToleranceExplorer({
                                             items,
                                             defaultEps = 1e-4,
                                             pMin = -16,
                                             pMax = -1,
                                         }: Props) {
    // представляем ε как 10^p
    const initP = useMemo(() => Math.round(Math.log10(defaultEps)), [defaultEps]);
    const [p, setP] = useState<number>(clamp(initP, pMin, pMax));

    const eps = useMemo(() => Math.pow(10, p), [p]);

    return (
        <div className="space-y-3">
            {/* панель управления ε */}
            <div className="flex flex-wrap items-center gap-3 rounded-md border border-border/60 bg-panel/60 px-3 py-2 text-sm">
                <div className="text-xs text-textDim">ε = 10^p</div>
                <input
                    type="range"
                    min={pMin}
                    max={pMax}
                    step={1}
                    value={p}
                    onChange={(e) => setP(clamp(parseInt(e.target.value, 10), pMin, pMax))}
                    className="w-64"
                />
                <label className="flex items-center gap-1 text-xs">
                    p=
                    <input
                        type="number"
                        className="w-16 rounded border border-border bg-surface px-1 py-0.5"
                        value={p}
                        onChange={(e) => {
                            const v = Number(e.target.value);
                            if (Number.isFinite(v)) setP(clamp(Math.round(v), pMin, pMax));
                        }}
                    />
                    <span className="text-textDim">(от {pMin} до {pMax})</span>
                </label>
                <div className="ml-auto text-xs">
                    ε = {eps.toExponential(1)} ({eps})
                </div>
            </div>

            {/* сам график */}
            <StepsToToleranceBar items={items} eps={eps} />
        </div>
    );
}
