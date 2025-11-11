import { SERIES, type SeriesNode } from "./series";
import { ALGORITHMS, type AlgorithmNode } from "./algorithms";
import { randomFromDomain } from "../pages/Experiments_v2/utils/domain";

export interface NConfig {
    start: number;
    stop: number;
    step: number;
}

export interface ExperimentConfig {
    series: { name: string; args: { x: number[] } }[];
    methods: {
        name: string;
        n: NConfig;
        m: number[];
        args: Record<string, unknown>;
    }[];
}

function toPascalCase(id: string): string {
    return id
        .split("_")
        .filter(Boolean)
        .map((part) => part.charAt(0).toUpperCase() + part.slice(1))
        .join("");
}

/**
 * Формирует объект полной конфигурации эксперимента.
 * @param nConfig диапозон n
 * @param m массив m
 * @param seriesList явный список рядов; если не задан, используется глобальный SERIES
 */
export function buildExperimentConfig(
    nConfig: NConfig = { start: 1, stop: 51, step: 1 },
    m: number[] = [10],
    seriesList?: SeriesNode[]
): ExperimentConfig {
    const source = seriesList ?? SERIES;

    const seriesPart = source
        .slice()
        .sort((a, b) => a.num - b.num)
        .filter((s) => s.num !== 0)
        .map((s: SeriesNode) => ({
            name: toPascalCase(s.id),
            args: { x: [randomFromDomain(s)] },
        }));

    const methodsPart = ALGORITHMS.map((a: AlgorithmNode) => {
        const name = a.python_id;
        const base = {
            name,
            n: nConfig,
            m,
            args: {} as Record<string, unknown>,
        };

        return base;
    });

    return { series: seriesPart, methods: methodsPart };
}
