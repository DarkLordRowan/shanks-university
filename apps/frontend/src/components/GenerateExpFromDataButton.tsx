import { buildExpConfig, type Mode, type NConfig } from "../data/buildExpConfig";
import type { SeriesNode } from "../data/series";
import type { AlgorithmNode } from "../data/algorithms.ts";

type Props = {
    mode: Mode;
    nConfig?: NConfig;
    series?: SeriesNode | SeriesNode[] | null;
    x?: number | number[] | null;
    m?: number | number[] | null;
    algorithm?: AlgorithmNode | AlgorithmNode[] | null;
    label?: string;
    disabled?: boolean;
    onSuccess: (json: string) => void;
    onError?: (msg: string) => void;
};

export const GenerateExpFromDataButton: React.FC<Props> = (
    {
        mode,
        nConfig = { start: 1, stop: 51, step: 1 },
        series = null,
        x = null,
        m = [10],
        algorithm = null,
        label = "Сгенерировать конфигурацию",
        disabled,
        onSuccess,
        onError,
    }) => {
    const handleClick = () => {
        try {
            const cfg = buildExpConfig(mode, nConfig, {
                series,
                x,
                m,
                algorithm,
            });
            onSuccess(JSON.stringify(cfg, null, 2));
        } catch (e: any) {
            console.error(e);
            onError?.(e?.message ?? "Ошибка генерации конфигурации.");
        }
    };

    return (
        <button
            className="rounded-xl bg-primary/80 px-4 py-2 text-sm font-medium text-black hover:opacity-90 disabled:opacity-60"
            onClick={handleClick}
            disabled={disabled}
            type="button"
        >
            {label}
        </button>
    );
};
