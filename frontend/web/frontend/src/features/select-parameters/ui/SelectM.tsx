import { useCallback, useEffect, useId, useMemo, useState } from "react";
import { getAllowedMValues, isValidMValue } from "@/features/select-parameters/model/mValues";

type Props = {
    value: number | null;
    onChange: (m: number | null) => void;
    disabled?: boolean;
};

function clamp01(x: number) { return Math.min(1, Math.max(0, x)); }

export function SelectM({ value, onChange, disabled }: Props) {
    const id = useId();

    // свёрнуто, если m уже выбран
    const [collapsed, setCollapsed] = useState<boolean>(value != null);
    useEffect(() => { setCollapsed(value != null); }, [value]);

    // черновик строкой (для свободного ввода)
    const [draft, setDraft] = useState<string>(value == null ? "" : String(value));

    // множество допустимых значений
    const options = useMemo(() => getAllowedMValues(), []);

    // валидность
    const validity: "in" | "out" | "empty" = useMemo(() => {
        const s = draft.trim();
        if (s === "") return "empty";
        const n = Number(s);
        if (!Number.isFinite(n)) return "out";
        return isValidMValue(n) ? "in" : "out";
    }, [draft]);

    // быстрый выбор
    const setFromQuick = useCallback((m: number) => {
        onChange(m);
        setDraft(String(m));
    }, [onChange]);

    // подтверждение
    const handleConfirm = useCallback(() => {
        if (validity !== "in") return;
        const n = Number(draft.trim());
        onChange(n);
        setCollapsed(true);
    }, [draft, validity, onChange]);

    // редактирование
    const handleEdit = useCallback(() => {
        setCollapsed(false);
        setDraft(value == null ? "" : String(value));
    }, [value]);

    // слайдер (0..100), привязка к ближайшему шагу 2
    const sliderValue = useMemo(() => {
        const s = draft.trim();
        const n = Number(s);
        if (!Number.isFinite(n)) return value == null ? 0 : value;
        return Math.round(n / 2) * 2;
    }, [draft, value]);

    if (collapsed) {
        return (
            <div className="flex items-center gap-2 rounded-xl2 border border-border bg-panel p-2 text-sm shadow-panel">
                <span className="text-textDim">m =</span>
                <span className="font-mono">{value == null ? "—" : value}</span>
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
            <div className="mb-2 text-xs font-semibold text-textDim">Выбор m ∈ {`{0,2,…,100}`}</div>

            {/* Быстрые значения (скроллимые чипы) */}
            <div className="mb-2 flex w-full gap-2 overflow-x-auto pb-1">
                {options.map((m) => (
                    <button
                        key={m}
                        type="button"
                        onClick={() => setFromQuick(m)}
                        disabled={disabled}
                        className="rounded-md border border-border px-2 py-0.5 text-xs hover:border-primary"
                    >
                        {m}
                    </button>
                ))}
            </div>

            {/* Поле ввода */}
            <div className="flex flex-col gap-2">
                <label htmlFor={`${id}-m`} className="text-[11px] text-textDim">
                    Введите m (чётное число от 0 до 100, шаг 2)
                </label>
                <input
                    id={`${id}-m`}
                    type="number"
                    inputMode="numeric"
                    min={0}
                    max={100}
                    step={2}
                    placeholder="например, 6"
                    value={draft}
                    onChange={(e) => setDraft(e.target.value)}
                    className={[
                        "w-full rounded-md border px-2 py-1 text-sm outline-none bg-surface",
                        validity === "in"
                            ? "border-emerald-500 focus:ring-1 focus:ring-emerald-500/60"
                            : validity === "out"
                                ? "border-red-500 focus:ring-1 focus:ring-red-500/60"
                                : "border-border focus:ring-1 focus:ring-primary/50"
                    ].join(" ")}
                    aria-invalid={validity === "out"}
                />

                {/* Слайдер (0..100), дискретизация по 2 */}
                <input
                    type="range"
                    min={0}
                    max={100}
                    step={2}
                    value={sliderValue}
                    onChange={(e) => {
                        const raw = Number(e.target.value);
                        const snapped = Math.round(clamp01(raw / 100) * 50) * 2;
                        setDraft(String(snapped));
                    }}
                    className="accent-primary"
                    aria-label="Ползунок выбора m"
                />
            </div>

            {/* Статус + подтверждение */}
            <div className="mt-3 flex items-center gap-3 text-[11px]">
                {validity === "in" && <span className="text-emerald-500">m допустим.</span>}
                {validity === "out" && <span className="text-red-500">Недопустимое значение.</span>}
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
