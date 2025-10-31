import React from "react";
import type { SeriesNode } from "../data/series";
import { buildExperimentConfig, type NConfig } from "../data/buildExperimentConfig";

type Props = {
    series: SeriesNode[];
    nConfig?: NConfig;
    m?: number[];
    label?: string;
    disabled?: boolean;
    onSuccess: (json: string) => void;
    onError?: (msg: string) => void;
};

export const GenerateFromDataButton: React.FC<Props> = (
    {
        series,
        nConfig = {start: 1, stop: 51, step: 1},
        m = [10],
        label = "Сгенерировать из данных проекта",
        disabled,
        onSuccess,
        onError,
    }) => {
    const handleClick = () => {
        try {
            const cfg = buildExperimentConfig(nConfig, m, series);
            const json = JSON.stringify(cfg, null, 2);
            onSuccess(json);
        } catch (e) {
            console.error(e);
            onError?.("Ошибка при генерации JSON из данных проекта.");
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
