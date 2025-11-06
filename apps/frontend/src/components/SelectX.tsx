// SelectX.tsx
import { useCallback, useId, useMemo, useState } from "react";
import type { DomainSpec, SeriesNode } from "../data/series";
import { Formula } from "./Formula.tsx";
import { characteristicPoints, domainContains, intervalsFromDomain, latexToNumber, } from "../utils/domain-intervals";

type Props = {
    series: SeriesNode | null;
    value: number | null;
    onChange: (x: number | null) => void;
    disabled?: boolean;
};

export function SelectX({series, value, onChange, disabled}: Props) {
    const id = useId();
    const domain: DomainSpec | undefined = series?.domain;
    const [activeIntervalIdx, setActiveIntervalIdx] = useState<number | null>(0);
    const [collapsed, setCollapsed] = useState<boolean>(false);
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
        if (draft.trim() === "") return "empty";
        let v = Number(draft);
        if (!Number.isFinite(v)) {
            try {
                v = latexToNumber(draft.replace(/π/g, "\\pi"));
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
        let v = Number(draft);
        if (!Number.isFinite(v)) v = latexToNumber(draft.replace(/π/g, "\\pi"));
        onChange(v);
        setCollapsed(true);
    }, [draft, validity, onChange]);

    const handleEdit = useCallback(() => {
        setCollapsed(false);
        setDraft(value == null ? "" : String(value));
    }, [value]);

    if (!series || !domain) {
        return <div className="text-xs text-neutral-500">Сначала выберите ряд.</div>;
    }

    if (collapsed) {
        return (
            <div
                className="flex items-center gap-2 rounded-lg border border-gray-200 bg-white/95 p-2 text-sm shadow-sm dark:border-gray-700 dark:bg-neutral-900/95">
                <span className="text-neutral-500">x =</span>
                <span className="font-mono">
          {value == null
              ? "—"
              : (Number.isInteger(value) ? value.toString() : Number(value).toPrecision(12))}
        </span>
                <button
                    type="button"
                    className="ml-auto rounded-md border px-2 py-1 text-xs"
                    onClick={handleEdit}
                    disabled={disabled}
                >
                    Изменить
                </button>
            </div>
        );
    }

    return (
        <div
            className="rounded-lg border border-gray-200 bg-white/95 p-3 shadow-sm dark:border-gray-700 dark:bg-neutral-900/95">
            <div className="mb-2 text-xs font-semibold">Допустимые значения (x)</div>

            {/* Интервальные кнопки */}
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
                                    ? "border-blue-500 ring-1 ring-blue-500/60"
                                    : "border-gray-300 dark:border-gray-700"
                            ].join(" ")}
                        >
                            <Formula inline latex={iv.labelLatex}/>
                        </button>
                    ))}
                </div>
            )}

            {/* Быстрые точки */}
            {quick.length > 0 && (
                <div className="mb-2 flex flex-wrap items-center gap-2">
                    <span className="text-[11px] text-neutral-500">Характерные точки:</span>
                    {quick.map((p, i) => (
                        <button
                            key={i}
                            type="button"
                            onClick={() => setFromQuick(p)}
                            disabled={disabled}
                            className="rounded border px-2 py-0.5 text-[11px]"
                        >
                            {Number.isInteger(p) ? p.toString() : p.toPrecision(6)}
                        </button>
                    ))}
                </div>
            )}

            {/* Ввод значения */}
            <div className="flex flex-col gap-2">
                <label htmlFor={`${id}-x`} className="text-[11px] text-neutral-500">
                    Введите (x) числом или LaTeX-выражением (("\pi/2"), ("3/2"), ("2\pi"))
                </label>
                <input
                    id={`${id}-x`}
                    type="text"
                    inputMode="decimal"
                    placeholder="введите x"
                    value={draft}
                    onChange={e => setDraft(e.target.value)}
                    disabled={disabled}
                    className={[
                        "w-full rounded-md border px-2 py-1 text-sm outline-none",
                        validity === "in"
                            ? "border-emerald-500 focus:ring-1 focus:ring-emerald-500/60"
                            : validity === "out"
                                ? "border-red-500 focus:ring-1 focus:ring-red-500/60"
                                : "border-gray-300 focus:ring-1 focus:ring-blue-500/50 dark:border-gray-700"
                    ].join(" ")}
                />

                {/* Слайдер только для ограниченного активного интервала */}
                {active?.bounded && (
                    <input
                        type="range"
                        min={0}
                        max={1000}
                        step={1}
                        aria-label="Ползунок выбора x"
                        disabled={disabled}
                        value={(() => {
                            const v = ((): number => {
                                const raw = draft.trim();
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
                            if (!Number.isFinite(v)) return 500;
                            const clamped = Math.min(Math.max(v, active.a), active.b);
                            return ((clamped - active.a) / (active.b - active.a)) * 1000;
                        })()}
                        onChange={e => {
                            const p = Number(e.target.value) / 1000;
                            const v = active.a + (active.b - active.a) * p;
                            const shown = Number.isInteger(v) ? v.toString() : v.toPrecision(12);
                            setDraft(shown);
                        }}
                    />
                )}
            </div>

            <div className="mt-3 flex items-center gap-3">
                <span className="text-[11px]">
                    {validity === "in" && <span className="text-emerald-600">(x) допустим.</span>}
                    {validity === "out" && <span className="text-red-600">Вне области.</span>}
                    {validity === "empty" && <span className="text-neutral-500">Введите значение.</span>}
                </span>
                <button
                    type="button"
                    onClick={handleConfirm}
                    disabled={disabled || validity !== "in"}
                    className="ml-auto rounded-md border px-3 py-1 text-xs disabled:opacity-50"
                >
                    Подтвердить
                </button>
            </div>
        </div>
    );
}
