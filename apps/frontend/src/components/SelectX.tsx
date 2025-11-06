import { useCallback, useEffect, useId, useMemo, useState } from "react";
import type { DomainSpec, SeriesNode } from "../data/series";
import { Formula } from "./Formula";
import {
    characteristicPoints,
    domainContains,
    intervalsFromDomain,
    latexToNumber,
} from "../utils/domain-intervals";

type Props = {
    series: SeriesNode | null;
    value: number | null;
    onChange: (x: number | null) => void;
    disabled?: boolean;
};

export function SelectX({ series, value, onChange, disabled }: Props) {
    const id = useId();
    const domain: DomainSpec | undefined = series?.domain;

    const [collapsed, setCollapsed] = useState<boolean>(value != null);

    useEffect(() => {
        setCollapsed(false);
    }, [series?.id]);

    const [activeIntervalIdx, setActiveIntervalIdx] = useState<number | null>(0);
    const [draft, setDraft] = useState<string>(value == null ? "" : String(value));

    const intervals = useMemo(
        () => (domain ? intervalsFromDomain(domain) : []),
        [domain]
    );

    const quick = useMemo(
        () => (domain ? characteristicPoints(domain) : []),
        [domain]
    );

    const active = useMemo(
        () => (activeIntervalIdx == null ? null : intervals[activeIntervalIdx] ?? null),
        [activeIntervalIdx, intervals]
    );

    const validity: "in" | "out" | "empty" = useMemo(() => {
        const s = draft.trim();
        if (s === "") return "empty";
        let v = Number(s);
        if (!Number.isFinite(v)) {
            try {
                v = latexToNumber(s.replace(/π/g, "\\pi"));
            } catch {
                return "out";
            }
        }
        if (!domain) return "out";
        return domainContains(domain, v) ? "in" : "out";
    }, [draft, domain]);

    const setFromQuick = useCallback((v: number) => {
        onChange(v);
        setDraft(Number.isInteger(v) ? v.toString() : v.toPrecision(12));
    }, [onChange]);

    const handleConfirm = useCallback(() => {
        if (validity !== "in") return;
        const s = draft.trim();
        let v = Number(s);
        if (!Number.isFinite(v)) v = latexToNumber(s.replace(/π/g, "\\pi"));
        onChange(v);
        setCollapsed(true);
    }, [draft, validity, onChange]);

    const handleEdit = useCallback(() => {
        setCollapsed(false);
        setDraft(value == null ? "" : String(value));
    }, [value]);

    if (!series || !domain) {
        return <div className="text-xs text-textDim">Сначала выберите ряд.</div>;
    }

    if (collapsed) {
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
        <span className="text-textDim">
          <Formula inline latex={"x="} />
        </span>
                <span className="font-mono">
          {value == null
              ? "—"
              : Number.isInteger(value)
                  ? value.toString()
                  : Number(value).toPrecision(12)}
        </span>
                <button
                    type="button"
                    className="ml-auto rounded-md border border-border px-2 py-1 text-xs"
                    onClick={handleEdit}
                    disabled={disabled}
                >
                    Изменить
                </button>
            </div>
        );
    }

    return (
        <div className="rounded-xl2 border border-border bg-panel p-3 text-sm text-white shadow-panel">
            <div className="mb-2 flex items-center gap-2 text-xs font-semibold text-textDim">
                <Formula inline latex={"\\text{Допустимые значения } x"} />
            </div>

            {intervals.length > 0 && (
                <div className="mb-2 flex flex-wrap gap-2">
                    {intervals.map((iv, idx) => (
                        <button
                            key={idx}
                            type="button"
                            aria-pressed={activeIntervalIdx === idx}
                            onClick={() => setActiveIntervalIdx(idx)}
                            disabled={disabled}
                            className={[
                                "inline-flex items-center gap-2 rounded-md border px-2 py-1 text-xs",
                                activeIntervalIdx === idx
                                    ? "border-primary ring-1 ring-primary/60"
                                    : "border-border"
                            ].join(" ")}
                            title="Интервал допустимых значений"
                        >
                            <Formula inline latex={iv.labelLatex} />
                        </button>
                    ))}
                </div>
            )}

            {quick.length > 0 && (
                <div className="mb-2 flex flex-wrap items-center gap-2">
                    <span className="text-[11px] text-textDim">Характерные точки:</span>
                    {quick.map((p, i) => (
                        <button
                            key={i}
                            type="button"
                            onClick={() => setFromQuick(p)}
                            disabled={disabled}
                            className="rounded border border-border px-2 py-0.5 text-[11px]"
                            title="Быстрый выбор допустимого значения"
                        >
                            {Number.isInteger(p) ? p.toString() : p.toPrecision(6)}
                        </button>
                    ))}
                </div>
            )}

            <div className="flex flex-col gap-2">
                <label htmlFor={`${id}-x`} className="flex items-center gap-2 text-[11px] text-textDim">
                    <span>Введите</span>
                    <Formula inline latex={"x"} />
                    <span>числом или LaTeX (\(\pi/2\), \(3/2\), \(2\\pi\))</span>
                </label>

                <input
                    id={`${id}-x`}
                    type="text"
                    inputMode="decimal"
                    placeholder="введите x"
                    value={draft}
                    onChange={(e) => setDraft(e.target.value)}
                    disabled={disabled}
                    className={[
                        "w-full rounded-md border px-2 py-1 text-sm outline-none bg-surface",
                        validity === "in"
                            ? "border-emerald-500 focus:ring-1 focus:ring-emerald-500/60"
                            : validity === "out"
                                ? "border-red-500 focus:ring-1 focus:ring-red-500/60"
                                : "border-border focus:ring-1 focus:ring-primary/50"
                    ].join(" ")}
                    aria-invalid={validity === "out"}
                    aria-describedby={`${id}-x-help`}
                />

                {active?.bounded && (
                    <input
                        type="range"
                        min={0}
                        max={1000}
                        step={1}
                        aria-label="Ползунок выбора x"
                        disabled={disabled}
                        value={(() => {
                            const raw = draft.trim();
                            const parsed = (() => {
                                if (raw === "") return Number.NaN;
                                let num = Number(raw);
                                if (!Number.isFinite(num)) {
                                    try {
                                        num = latexToNumber(raw.replace(/π/g, "\\pi"));
                                    } catch {
                                        return Number.NaN;
                                    }
                                }
                                return num;
                            })();
                            if (!Number.isFinite(parsed)) return 500;
                            const clamped = Math.min(Math.max(parsed, active.a), active.b);
                            return ((clamped - active.a) / (active.b - active.a)) * 1000;
                        })()}
                        onChange={(e) => {
                            const p = Number(e.target.value) / 1000;
                            const v = active.a + (active.b - active.a) * p;
                            setDraft(Number.isInteger(v) ? v.toString() : v.toPrecision(12));
                        }}
                        className="accent-primary"
                    />
                )}
            </div>

            <div
                id={`${id}-x-help`}
                className="mt-3 flex items-center gap-3 text-[11px]"
                aria-live="polite"
            >
                {validity === "in" && <span className="text-emerald-500">x допустим.</span>}
                {validity === "out" && <span className="text-red-500">Вне области.</span>}
                {validity === "empty" && <span className="text-textDim">Введите значение.</span>}
                <button
                    type="button"
                    onClick={handleConfirm}
                    disabled={disabled || validity !== "in"}
                    className="ml-auto rounded-md border border-border px-3 py-1 text-xs disabled:opacity-50"
                >
                    Подтвердить
                </button>
            </div>
        </div>
    );
}
