// lib/buildExpConfigLocal.ts
import type { SeriesNode } from "./series.ts";
import type { AlgorithmNode } from "./algorithms.ts";
import { randomFromDomain } from "../pages/Experiments_v2/utils/domain";

export interface NConfig { start: number; stop: number; step: number; }

export interface ExperimentConfig {
    series: { name: string; args: { x: number[] } }[];
    methods: { name: string; n: NConfig; m: number[]; args: Record<string, unknown> }[];
}

export type Mode = "vary-series" | "vary-x" | "vary-algo" | "vary-m";

export function toPascalCase(id: string): string {
    return id.split("_").filter(Boolean).map(p => p[0].toUpperCase() + p.slice(1)).join("");
}

const asArray = <T>(v?: T | T[] | null): T[] =>
    v == null ? [] : Array.isArray(v) ? v : [v];

/**
 * Универсальный генератор без глобальных списков.
 * Условия:
 * - vary-series: series[] обязателен (>=1); algorithm обязателен; m обязателен (array|one).
 *   x для каждого ряда генерируется случайно, переданный x игнорируется.
 * - vary-x: series одиночный; x[] обязателен (>=1); algorithm обязателен; m обязателен.
 * - vary-algo: series одиночный; algorithms[] обязателен (>=1); x одиночный; m обязателен.
 * - vary-m: series одиночный; m[] обязателен (>=1); algorithm обязателен; x одиночный.
 */
export function buildExpConfig(
    mode: Mode,
    nConfig: NConfig,
    opts: {
        series?: SeriesNode | SeriesNode[] | null;
        x?: number | number[] | null;
        m?: number | number[] | null;
        algorithm?: AlgorithmNode | AlgorithmNode[] | null;
    }
): ExperimentConfig {
    const S = asArray(opts.series);
    const X = asArray(opts.x);
    const M = asArray(opts.m);
    const A = asArray(opts.algorithm);

    switch (mode) {
        case "vary-series": {
            if (S.length === 0) throw new Error("vary-series: требуется хотя бы один ряд.");
            if (A.length === 0) throw new Error("vary-series: требуется algorithm.");
            if (M.length === 0) throw new Error("vary-series: требуется список m или одиночное m.");
            const seriesPart = S.map(s => ({
                name: toPascalCase(s.id),
                args: { x: [randomFromDomain(s)] }, // правило: x генерируется для каждого ряда
            }));
            const methodsPart = [{
                name: A[0].python_id,
                n: nConfig,
                m: M,
                args: {},
            }];
            return { series: seriesPart, methods: methodsPart };
        }

        case "vary-x": {
            if (S.length !== 1) throw new Error("vary-x: должен быть ровно один ряд.");
            if (X.length === 0) throw new Error("vary-x: требуется массив x.");
            if (A.length === 0) throw new Error("vary-x: требуется algorithm.");
            if (M.length === 0) throw new Error("vary-x: требуется список m или одиночное m.");
            const seriesPart = [{ name: toPascalCase(S[0].id), args: { x: X } }];
            const methodsPart = [{
                name: A[0].python_id,
                n: nConfig,
                m: M,
                args: {},
            }];
            return { series: seriesPart, methods: methodsPart };
        }

        case "vary-algo": {
            if (S.length !== 1) throw new Error("vary-algo: должен быть ровно один ряд.");
            if (A.length === 0) throw new Error("vary-algo: требуется массив algorithms.");
            if (M.length === 0) throw new Error("vary-algo: требуется список m или одиночное m.");
            const x = X.length ? X[0] : randomFromDomain(S[0]);
            const seriesPart = [{ name: toPascalCase(S[0].id), args: { x: [x] } }];
            const methodsPart = A.map(a => ({
                name: a.python_id,
                n: nConfig,
                m: M,
                args: {},
            }));
            return { series: seriesPart, methods: methodsPart };
        }

        case "vary-m": {
            if (S.length !== 1) throw new Error("vary-m: должен быть ровно один ряд.");
            if (M.length === 0) throw new Error("vary-m: требуется массив m.");
            if (A.length === 0) throw new Error("vary-m: требуется algorithm.");
            const x = X.length ? X[0] : randomFromDomain(S[0]);
            const seriesPart = [{ name: toPascalCase(S[0].id), args: { x: [x] } }];
            const methodsPart = [{
                name: A[0].python_id,
                n: nConfig,
                m: M,
                args: {},
            }];
            return { series: seriesPart, methods: methodsPart };
        }
    }
}
