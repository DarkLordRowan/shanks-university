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
    value: number[];                     // выбранные x
    onChange: (xs: number[]) => void;    // вернуть массив
    disabled?: boolean;
};

const EPS = 1e-12;
const eq = (a: number, b: number) => Math.abs(a - b) <= EPS;

export function SelectXs({ series, value, onChange, disabled }: Props) {
    const id = useId();
    const domain: DomainSpec | undefined = series?.domain;

    const [collapsed, setCollapsed] = useState<boolean>(value.length > 0);
    useEffect(() => { setCollapsed(false); }, [series?.id]);
    useEffect(() => { setCollapsed(value.length > 0); }, [value.length]);

    const [activeIntervalIdx, setActiveIntervalIdx] = useState<number | null>(0);
    const [draft, setDraft] = useState<string>("");

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

    const parsedDraft = useMemo(() => {
        const s = draft.trim();
        if (!s) return Number.NaN;
        let v = Number(s);
        if (!Number.isFinite(v)) {
            try { v = latexToNumber(s.replace(/π/g, "\\pi")); } catch { return Number.NaN; }
        }
        return v;
    }, [draft]);

    const draftStatus: "valid" | "out" | "dup" | "empty" | "bad" = useMemo(() => {
        if (draft.trim() === "") return "empty";
        const v = parsedDraft;
        if (!Number.isFinite(v)) return "bad";
        if (!domain || !domainContains(domain, v)) return "out";
        if (value.some(x => eq(x, v))) return "dup";
        return "valid";
    }, [draft, parsedDraft, domain, value]);

    const addValue = useCallback((v: number) => {
        if (!domain || !domainContains(domain, v)) return;
        if (value.some(x => eq(x, v))) return;
        const next = [...value, v].sort((a, b) => a - b);
        onChange(next);
    }, [domain, onChange, value]);

    const removeAt = useCallback((idx: number) => {
        const next = value.slice();
        next.splice(idx, 1);
        onChange(next);
    }, [onChange, value]);

    const clearAll = useCallback(() => onChange([]), [onChange]);

    const handleAddDraft = useCallback(() => {
        if (draftStatus !== "valid") return;
        addValue(parsedDraft);
        setDraft("");
    }, [draftStatus, addValue, parsedDraft]);

    const handleConfirm = useCallback(() => {
        // уже в onChange массив консистентен
        setCollapsed(true);
    }, []);

    const handleEdit = useCallback(() => {
        setCollapsed(false);
    }, []);

    if (!series || !domain) {
        return (
            <div className="rounded-xl2 border border-border bg-panel p-3 text-sm text-white shadow-panel">
                <div className="text-xs text-textDim">Выбор нескольких x. Сначала выберите ряд.</div>
            </div>
        );
    }

    if (collapsed) {
        const preview = value.slice(0, 5).map(v => (Number.isInteger(v) ? v.toString() : v.toPrecision(8)));
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
        <span className="text-textDim">
          <Formula inline latex={"x \\in"} />
        </span>
                <span className="font-mono truncate">
          {value.length === 0 ? "—"
              : value.length <= 5 ? `[${preview.join(", ")}]`
                  : `[${preview.join(", ")}, …] (${value.length})`}
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
                <Formula inline latex={"\\text{Допустимые } x\\ \\text{и выбранные значения}"} />
            </div>

            {/* Интервалы домена */}
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

            {/* Характерные точки */}
            {quick.length > 0 && (
                <div className="mb-2 flex flex-wrap items-center gap-2">
                    <span className="text-[11px] text-textDim">Характерные точки:</span>
                    {quick.map((p, i) => (
                        <button
                            key={i}
                            type="button"
                            onClick={() => addValue(p)}
                            disabled={disabled}
                            className="rounded border border-border px-2 py-0.5 text-[11px]"
                            title="Добавить точку"
                        >
                            {Number.isInteger(p) ? p.toString() : p.toPrecision(6)}
                        </button>
                    ))}
                </div>
            )}

            {/* Ввод и слайдер */}
            <div className="flex flex-col gap-2">
                <label htmlFor={`${id}-xs`} className="flex items-center gap-2 text-[11px] text-textDim">
                    <span>Добавьте</span>
                    <Formula inline latex={"x"} />
                    <span>числом или LaTeX (\(\pi/2\), \(3/2\), \(2\\pi\))</span>
                </label>

                <div className="flex gap-2">
                    <input
                        id={`${id}-xs`}
                        type="text"
                        inputMode="decimal"
                        placeholder="например: 0.5 или \pi/2"
                        value={draft}
                        onChange={(e) => setDraft(e.target.value)}
                        onKeyDown={(e) => {
                            if (e.key === "Enter") { e.preventDefault(); handleAddDraft(); }
                            if (e.key === "Escape") { setDraft(""); }
                        }}
                        disabled={disabled}
                        className={[
                            "w-full rounded-md border px-2 py-1 text-sm outline-none bg-surface",
                            draftStatus === "valid"
                                ? "border-emerald-500 focus:ring-1 focus:ring-emerald-500/60"
                                : draftStatus === "out" || draftStatus === "bad"
                                    ? "border-red-500 focus:ring-1 focus:ring-red-500/60"
                                    : "border-border focus:ring-1 focus:ring-primary/50"
                        ].join(" ")}
                        aria-invalid={draftStatus === "out" || draftStatus === "bad"}
                    />
                    <button
                        type="button"
                        onClick={handleAddDraft}
                        disabled={disabled || draftStatus !== "valid"}
                        className="rounded-md border border-border px-3 py-1 text-xs disabled:opacity-50"
                        title="Добавить x"
                    >
                        Добавить
                    </button>
                </div>

                {active?.bounded && (
                    <input
                        type="range"
                        min={0}
                        max={1000}
                        step={1}
                        aria-label="Ползунок выбора x"
                        disabled={disabled}
                        value={(() => {
                            const v = parsedDraft;
                            if (!Number.isFinite(v)) return 500;
                            const clamped = Math.min(Math.max(v, active.a), active.b);
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

            {/* Текущий набор x */}
            <div className="mt-3">
                <div className="mb-1 text-[11px] text-textDim">Выбрано:</div>
                {value.length === 0 ? (
                    <div className="text-[11px] text-textDim">—</div>
                ) : (
                    <div className="flex flex-wrap gap-2">
                        {value.map((v, idx) => (
                            <span
                                key={`${v}-${idx}`}
                                className="inline-flex items-center gap-1 rounded-md border border-border bg-surface px-2 py-0.5 text-xs"
                                title={`${v}`}
                            >
                <span className="font-mono">
                  {Number.isInteger(v) ? v.toString() : v.toPrecision(8)}
                </span>
                <button
                    type="button"
                    onClick={() => removeAt(idx)}
                    className="rounded border border-border px-1 text-[10px] hover:border-primary"
                    aria-label={`Удалить ${v}`}
                >
                  ×
                </button>
              </span>
                        ))}
                    </div>
                )}
            </div>

            {/* Статус + подтверждение/очистка */}
            <div className="mt-3 flex items-center gap-2 text-[11px]">
                {draftStatus === "valid" && <span className="text-emerald-500">Готово к добавлению.</span>}
                {draftStatus === "out" && <span className="text-red-500">Вне области домена.</span>}
                {draftStatus === "dup" && <span className="text-textDim">Такое значение уже добавлено.</span>}
                {draftStatus === "bad" && <span className="text-red-500">Не удалось распарсить.</span>}
                <button
                    type="button"
                    onClick={clearAll}
                    disabled={disabled || value.length === 0}
                    className="ml-auto rounded-md border border-border px-3 py-1 text-xs disabled:opacity-50"
                >
                    Очистить
                </button>
                <button
                    type="button"
                    onClick={handleConfirm}
                    disabled={disabled}
                    className="rounded-md border border-border px-3 py-1 text-xs"
                >
                    Подтвердить
                </button>
            </div>
        </div>
    );
}
