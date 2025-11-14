import { useMemo, useState } from "react";
import type { Item } from "../../types/item";
import { StepsToToleranceBarUniversal } from "./StepsToToleranceBar_Universal";

type Props = {
    items: Item[];
    defaultEps?: number;
    pMin?: number; // min(log10 ε)
    pMax?: number; // max(log10 ε)
};

function clamp(n: number, lo: number, hi: number) {
    return Math.max(lo, Math.min(hi, n));
}

export function StepsToToleranceExplorer({
                                             items,
                                             defaultEps = 1e-8,
                                             pMin = -16,
                                             pMax = -1,
                                         }: Props) {
    const initP = useMemo(
        () => Math.round(Math.log10(defaultEps)),
        [defaultEps],
    );
    const [p, setP] = useState<number>(clamp(initP, pMin, pMax));

    const eps = useMemo(() => Math.pow(10, p), [p]);

    return (
        <div className="space-y-3">
            {/* панель управления ε */}
            <div className="flex flex-wrap items-center gap-3 rounded-xl2 border border-border/60 bg-panel/60 px-3 py-2 text-xs text-textDim">
                <div className="text-[11px]">ε = 10^p</div>
                <input
                    type="range"
                    min={pMin}
                    max={pMax}
                    step={1}
                    value={p}
                    onChange={(e) =>
                        setP(
                            clamp(
                                parseInt(e.target.value, 10),
                                pMin,
                                pMax,
                            ),
                        )
                    }
                    className="w-64"
                />
                <label className="flex items-center gap-1 text-[11px]">
                    p=
                    <input
                        type="number"
                        className="w-16 rounded-md border border-border bg-surface px-1 py-0.5 text-xs text-textDim"
                        value={p}
                        onChange={(e) => {
                            const v = Number(e.target.value);
                            if (Number.isFinite(v)) {
                                setP(
                                    clamp(
                                        Math.round(v),
                                        pMin,
                                        pMax,
                                    ),
                                );
                            }
                        }}
                    />
                    <span className="text-textDim/70">
                        (от {pMin} до {pMax})
                    </span>
                </label>
                <div className="ml-auto text-[11px]">
                    ε = {eps.toExponential(1)} ({eps})
                </div>
            </div>

            {/* график для уже отфильтрованного набора items */}
            <StepsToToleranceBarUniversal items={items} eps={eps} />
        </div>
    );
}
