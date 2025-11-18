// analysis/buildErrorMatrix.ts
import type { Item } from "../types/item";

export type AlgoKey = string;

export interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: Item["algorithm"]["algorithmArgs"] | null;
}

export interface AlgoStats {
    total: number;   // всего Item для алгоритма
    success: number; // без error
    error: number;   // с error (независимо от n)
}

export interface ErrorMatrix {
    /** Ось X: шаги n, на которых были ошибки (error.data.n != null) */
    nList: number[];
    /** Ось Y: алгоритмы (уникальность по algorithmId) */
    algoList: AlgoInfo[];
    /**
     * Карта ячеек (ошибки по (algo, n)):
     *   key = `${algoKey}||${n}`
     *   value = количество ошибок
     */
    cellMap: Map<string, number>;
    /** Агрегаты по алгоритмам */
    algoStats: Record<AlgoKey, AlgoStats>;
    /** Общее число элементов Item с ошибкой, участвующих в матрице (error.data.n != null) */
    totalErrorItems: number;
}

/** Краткая сводка аргументов алгоритма */
function buildArgsSummary(args: Item["algorithm"]["algorithmArgs"] | null): string {
    if (!args || Object.keys(args).length === 0) return "";
    const entries = Object.entries(args).sort(([a, b]) => a.localeCompare(b));
    return entries.map(([k, v]) => `${k}=${v}`).join(", ");
}

/**
 * Строит матрицу:
 *
 *  - ось X: n (шаг, где возникла ошибка)
 *  - ось Y: алгоритмы (уникальность по algorithmId)
 *  - cellMap[algoKey, n] = количество ошибок
 *  - algoStats[algoKey] = { total, success, error }
 */
export function buildErrorMatrix(items: Item[]): ErrorMatrix {
    const nSet = new Set<number>();
    const algoMap = new Map<AlgoKey, AlgoInfo>();
    const cellMap = new Map<string, number>();
    const statsMap = new Map<AlgoKey, AlgoStats>();

    let totalErrWithN = 0;

    for (const it of items) {
        const algoKey: AlgoKey = it.algorithm.algorithmId;

        // регистрация алгоритма
        if (!algoMap.has(algoKey)) {
            const args = it.algorithm.algorithmArgs ?? null;
            algoMap.set(algoKey, {
                key: algoKey,
                algorithmName: it.algorithm.algorithmName,
                m: it.algorithm.m,
                argsSummary: buildArgsSummary(args),
                algorithmArgs: args,
            });
        }

        // агрегаты по алгоритму (total / success / error)
        const currentStats =
            statsMap.get(algoKey) ?? { total: 0, success: 0, error: 0 };

        currentStats.total += 1;
        if (it.error) {
            currentStats.error += 1;
        } else {
            currentStats.success += 1;
        }
        statsMap.set(algoKey, currentStats);

        // матрица по n: учитываем только ошибки с конкретным n
        const err = it.error;
        const n = err?.data?.n;

        if (err && n != null) {
            nSet.add(n);
            const cellKey = `${algoKey}||${n}`;
            const prev = cellMap.get(cellKey) ?? 0;
            cellMap.set(cellKey, prev + 1);
            totalErrWithN += 1;
        }
    }

    const nList = Array.from(nSet).sort((a, b) => a - b);

    const algoList = Array.from(algoMap.values()).sort(
        (a, b) =>
            a.algorithmName.localeCompare(b.algorithmName) ||
            (a.m ?? 0) - (b.m ?? 0),
    );

    const algoStats: Record<AlgoKey, AlgoStats> = {};
    for (const [key, st] of statsMap.entries()) {
        algoStats[key] = st;
    }

    return {
        nList,
        algoList,
        cellMap,
        algoStats,
        totalErrorItems: totalErrWithN,
    };
}
