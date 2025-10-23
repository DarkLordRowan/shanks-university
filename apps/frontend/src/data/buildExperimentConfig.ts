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

/**
 * Преобразует snake_case идентификатор в PascalCase.
 * Например: "arcsin_x_minus_x_series" → "ArcsinXMinusXSeries"
 */
function toPascalCase(id: string): string {
    return id
        .split("_")
        .filter(Boolean)
        .map((part) => part.charAt(0).toUpperCase() + part.slice(1))
        .join("");
}

/**
 * Формирует объект полной конфигурации эксперимента.
 */
export function buildExperimentConfig(
    nConfig: NConfig = { start: 1, stop: 51, step: 1 },
    m: number[] = [10]
): ExperimentConfig {
    // Формируем раздел series
    const seriesPart = SERIES
        .sort((a, b) => a.num - b.num)
        .filter((s) => s.num !== 0)
        .map((s: SeriesNode) => ({
            name: toPascalCase(s.id), // уже включает "Series"
            args: { x: [randomFromDomain(s)] },
        }));

    // Формируем раздел methods (алгоритмы)
    const methodsPart = ALGORITHMS.map((a: AlgorithmNode) => {
        const name = toPascalCase(a.id); // уже включает "Algorithm"
        const base = {
            name,
            n: nConfig,
            m,
            args: {} as Record<string, unknown>,
        };

        switch (name) {
            case "DrummondDAlgorithm":
                base.args = {
                    remainder: ["v_variant", "u_variant", "t_wave_variant"],
                    useRecFormulas: [true, false],
                };
                break;
            case "LevinAlgorithm":
                base.args = {
                    remainder: ["v_variant", "u_variant", "t_wave_variant"],
                    useRecFormulas: [true, false],
                    beta: [52],
                };
                break;
            case "LevinSidiMAlgorithm":
                base.args = {
                    remainder: ["v_variant", "u_variant", "t_wave_variant"],
                    gamma: [52],
                };
                break;
            case "LevinSidiSAlgorithm":
                base.args = {
                    remainder: ["v_variant", "u_variant", "t_wave_variant"],
                    useRecFormulas: [true, false],
                    parameter: [52],
                };
                break;
            case "WhynnRhoAlgorithm":
                base.args = {
                    numerator: ["rho_variant", "generalized_variant", "gamma_rho_variant"],
                    gamma: [52],
                    RHO: [2],
                };
                break;
            case "WynnEpsilon3Algorithm":
                base.args = {
                    epsilon_threshold: [1e-6, 1e-8, 1e-10],
                };
                break;
            default:
                base.args = {};
        }

        return base;
    });

    return { series: seriesPart, methods: methodsPart };
}
