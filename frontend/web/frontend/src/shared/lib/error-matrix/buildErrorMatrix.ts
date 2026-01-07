import type { AccelArgs, Experiment } from "@/entities/experiment/model/experiment";

export type AlgoKey = string;

export interface AlgoInfo {
    key: AlgoKey;
    algorithmName: string;
    m: number | null;
    argsSummary: string;
    algorithmArgs: Record<string, unknown> | null;
}

export interface AlgoStats {
    total: number; // всего элементов для алгоритма
    success: number; // без error
    error: number; // с error (независимо от n)
}

export interface ErrorMatrix {
    /** Ось X: шаги n, на которых были ошибки */
    nList: number[];
    /** Ось Y: алгоритмы (уникальность по algorithmId / accel_id) */
    algoList: AlgoInfo[];
    /**
     * Карта ячеек (ошибки по (algo, n)):
     *   key = `${algoKey}||${n}`
     *   value = количество ошибок
     */
    cellMap: Map<string, number>;
    /**
     * Уникальные сообщения ошибок по ячейкам:
     *   key = `${algoKey}||${n}`
     *   value = массив уникальных текстов ошибок
     */
    cellMessagesMap: Map<string, string[]>;
    /** Агрегаты по алгоритмам */
    algoStats: Record<AlgoKey, AlgoStats>;
    /** Общее число элементов с ошибкой, участвующих в матрице */
    totalErrorItems: number;
}

/** Краткая сводка аргументов (универсальная) */
function buildArgsSummary(args: Record<string, unknown> | null | undefined): string {
    if (!args) return "";
    const keys = Object.keys(args);
    if (keys.length === 0) return "";
    const entries = keys
        .sort((a, b) => a.localeCompare(b))
        .map((k) => `${k}=${String((args as Record<string, unknown>)[k])}`);
    return entries.join(", ");
}

/**
 * Вариант построения матрицы ошибок из Experiment
 * с фильтром по series.precision.
 *
 * Интерпретация:
 *   - total/ok/err считаются по "элементам" (seriesAccel, шаг n):
 *       * для каждого computed[n]:
 *           total += 1
 *           err   += 1, если есть хотя бы одна ошибка с таким n
 *           иначе success += 1
 *       * для ошибок с n, для которых нет computed[n]:
 *           total += 1, error += 1
 *   - cellMap[algoKey, n] = количество ошибок для данного алгоритма и шага n
 */
export function buildErrorMatrixFromExperiment(
    experiment: Experiment | null,
    precision: string | null
): ErrorMatrix {
    const nSet = new Set<number>();
    const algoMap = new Map<AlgoKey, AlgoInfo>();
    const cellMap = new Map<string, number>();
    const cellMessagesMap = new Map<string, string[]>();
    const statsMap = new Map<AlgoKey, AlgoStats>();

    let totalErrWithN = 0;

    if (!experiment || !experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        return {
            nList: [],
            algoList: [],
            cellMap,
            cellMessagesMap,
            algoStats: {},
            totalErrorItems: 0,
        };
    }

    const seriesById = new Map((experiment.seriesList ?? []).map((s) => [s.id, s]));
    const accelById = new Map((experiment.accelList ?? []).map((a) => [a.id, a]));

    for (const sa of experiment.seriesAccelList) {
        const series = seriesById.get(sa.series_id);
        if (!series) continue;

        // фильтр по точности
        if (precision && series.precision !== precision) continue;

        const accel = accelById.get(sa.accel_id);
        const algoKey: AlgoKey = sa.accel_id;

        // регистрация алгоритма
        if (!algoMap.has(algoKey)) {
            const args = (accel?.args ?? null) as AccelArgs | null;
            algoMap.set(algoKey, {
                key: algoKey,
                algorithmName: accel?.name ?? algoKey,
                m: accel?.m ?? null,
                argsSummary: buildArgsSummary(args as Record<string, unknown> | null),
                algorithmArgs: args as Record<string, unknown> | null,
            });
        }

        const ensureStats = (): AlgoStats => {
            const st = statsMap.get(algoKey);
            if (st) return st;
            const init: AlgoStats = { total: 0, success: 0, error: 0 };
            statsMap.set(algoKey, init);
            return init;
        };

        // агрегируем ошибки по n: считаем и тексты
        const errorInfoByN = new Map<number, { count: number; messages: string[] }>();
        for (const e of sa.errors ?? []) {
            const n = e?.n;
            if (!Number.isFinite(n)) continue;

            const rawMsg = (e as any).message;
            const msg = typeof rawMsg === "string" ? rawMsg.trim() : String(rawMsg ?? "").trim();

            let info = errorInfoByN.get(n);
            if (!info) {
                info = { count: 0, messages: [] };
                errorInfoByN.set(n, info);
            }
            info.count += 1;
            if (msg) {
                info.messages.push(msg);
            }
        }

        const seenNs = new Set<number>();

        // учитываем все computed[n] как элементы
        for (const cp of sa.computed ?? []) {
            const n = cp?.n;
            if (!Number.isFinite(n)) continue;
            seenNs.add(n);

            const st = ensureStats();
            st.total += 1;

            const info = errorInfoByN.get(n);
            const errCount = info?.count ?? 0;

            if (errCount > 0) {
                st.error += 1;

                nSet.add(n);
                const cellKey = `${algoKey}||${n}`;
                const prev = cellMap.get(cellKey) ?? 0;
                cellMap.set(cellKey, prev + errCount);
                totalErrWithN += errCount;

                if (info && info.messages.length > 0) {
                    const existing = new Set(cellMessagesMap.get(cellKey) ?? []);
                    for (const m of info.messages) {
                        if (m) existing.add(m);
                    }
                    cellMessagesMap.set(cellKey, Array.from(existing));
                }
            } else {
                st.success += 1;
            }
        }

        // ошибки по n, для которых нет computed[n]
        for (const [n, info] of errorInfoByN.entries()) {
            if (seenNs.has(n)) continue;

            const st = ensureStats();
            st.total += 1;
            st.error += 1;

            nSet.add(n);
            const cellKey = `${algoKey}||${n}`;
            const prev = cellMap.get(cellKey) ?? 0;
            cellMap.set(cellKey, prev + info.count);
            totalErrWithN += info.count;

            if (info.messages.length > 0) {
                const existing = new Set(cellMessagesMap.get(cellKey) ?? []);
                for (const m of info.messages) {
                    if (m) existing.add(m);
                }
                cellMessagesMap.set(cellKey, Array.from(existing));
            }
        }
    }

    const nList = Array.from(nSet).sort((a, b) => a - b);

    const algoList = Array.from(algoMap.values()).sort(
        (a, b) => a.algorithmName.localeCompare(b.algorithmName) || (a.m ?? 0) - (b.m ?? 0)
    );

    const algoStats: Record<AlgoKey, AlgoStats> = {};
    for (const [key, st] of statsMap.entries()) {
        algoStats[key] = st;
    }

    return {
        nList,
        algoList,
        cellMap,
        cellMessagesMap,
        algoStats,
        totalErrorItems: totalErrWithN,
    };
}
