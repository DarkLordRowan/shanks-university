// src/features/import-experiment-from-parquet/model/useLoadParquetExpirement.ts

import { useCallback, useRef, useState } from "react";

import type { Experiment } from "@/entities/experiment/model/experiment";
import type { ParquetAccelRow, ParquetSeriesRow } from "@/shared/parquet/types";

import { readParquetFile } from "@/shared/parquet/readParquetFile";
import {
    buildSeriesEntityFromParquetRow,
    buildAccelAndSeriesAccelEntityVariantsFromParquetRow,
} from "@/shared/parquet/buildExperimentFromParquet";

type Series = Experiment["seriesList"][number];
type Accel = Experiment["accelList"][number];
type SeriesAccel = Experiment["seriesAccelList"][number];

function parsePartitions(path: string): Record<string, string> {
    const res: Record<string, string> = {};
    const parts = path.split(/[\\/]/);

    for (const seg of parts) {
        const idx = seg.indexOf("=");
        if (idx <= 0) continue;
        const key = seg.slice(0, idx);
        const val = decodeURIComponent(seg.slice(idx + 1));
        res[key] = val;
    }

    return res;
}

function toNumberOrNull(v: unknown): number | null {
    if (typeof v === "number") return Number.isNaN(v) ? null : v;
    if (typeof v === "bigint") return Number(v);
    if (typeof v === "string" && v.trim() !== "") {
        const n = Number(v);
        return Number.isNaN(n) ? null : n;
    }
    return null;
}

function yieldToMainThread(): Promise<void> {
    return new Promise((resolve) => requestAnimationFrame(() => resolve()));
}

type Phase = "reading" | "building";

export type LoadParquetState =
    | { status: "idle" }
    | {
    status: "loading";
    phase: Phase;
    message: string;
    done: number;
    total: number;
}
    | { status: "error"; message: string }
    | { status: "success"; count: number };

function splitFilesByKind(allFiles: File[]) {
    const seriesFiles = allFiles.filter((f) => f.webkitRelativePath.includes("/series/"));
    const accelFiles = allFiles.filter((f) => f.webkitRelativePath.includes("/accelerations/"));
    return { seriesFiles, accelFiles };
}

// Для series можно использовать проекцию (плоские колонки).
const SERIES_COLUMNS = [
    "series_id",
    "series_name",
    "precision",
    "arguments",
    "series_limit",
    "computed",
] as const;

// Для accelerations НЕ используем проекцию по умолчанию: nested (computed/errors/events/filtered) может «обнуляться».

export function useLoadParquetExperiment() {
    const [state, setState] = useState<LoadParquetState>({ status: "idle" });
    const experimentRef = useRef<Experiment | null>(null);

    const runIdRef = useRef(0);

    const setLoadingWithRun = useCallback(
        (runId: number, phase: Phase, message: string, done: number, total: number) => {
            if (runIdRef.current !== runId) return;
            setState({ status: "loading", phase, message, done, total });
        },
        []
    );

    const setErrorWithRun = useCallback((runId: number, message: string) => {
        if (runIdRef.current !== runId) return;
        setState({ status: "error", message });
    }, []);

    const setSuccessWithRun = useCallback((runId: number, count: number) => {
        if (runIdRef.current !== runId) return;
        setState({ status: "success", count });
    }, []);

    const load = useCallback(
        async (files: FileList | File[]) => {
            const fileArr = Array.isArray(files) ? files : Array.from(files);

            const runId = ++runIdRef.current;

            if (!fileArr || fileArr.length === 0) {
                setErrorWithRun(runId, "Нет файлов");
                return;
            }

            const allFiles = Array.from(fileArr);
            const { seriesFiles, accelFiles } = splitFilesByKind(allFiles);

            const totalFiles = seriesFiles.length + accelFiles.length;
            if (totalFiles === 0) {
                setErrorWithRun(
                    runId,
                    "Не найдены файлы /series/ и /accelerations/ в webkitRelativePath"
                );
                return;
            }

            try {
                setLoadingWithRun(
                    runId,
                    "reading",
                    "Подготовка (0/" + totalFiles + ")",
                    0,
                    totalFiles
                );

                const seriesByNumericId = new Map<number, Series>();
                const accelById = new Map<string, Accel>();
                const seriesAccelList: SeriesAccel[] = [];

                let filesDone = 0;

                // 1) series/*
                for (const f of seriesFiles) {
                    if (runIdRef.current !== runId) return;

                    const parts = parsePartitions(f.webkitRelativePath);
                    const precisionFromPath = parts["precision"];
                    const seriesNameFromPath = parts["series_name"];

                    const rows = await readParquetFile<ParquetSeriesRow>(f, {
                        columns: [...SERIES_COLUMNS],
                    });

                    for (const r of rows) {
                        const sid = toNumberOrNull(
                            (r as unknown as { series_id?: unknown }).series_id
                        );

                        const source = r as Partial<Pick<ParquetSeriesRow, "series_id" | "precision" | "series_name">>;
                        const patched: ParquetSeriesRow = {
                            ...r,
                            series_id: sid ?? -1,
                            precision: source.precision ?? precisionFromPath ?? "",
                            series_name: source.series_name ?? seriesNameFromPath ?? "",
                        };

                        const series = buildSeriesEntityFromParquetRow(patched);
                        if (!series) continue;

                        const key = toNumberOrNull(series.id);
                        if (key == null) continue;

                        seriesByNumericId.set(key, series);
                    }

                    filesDone += 1;
                    setLoadingWithRun(
                        runId,
                        "reading",
                        "Чтение series (" + filesDone + "/" + totalFiles + ")",
                        filesDone,
                        totalFiles
                    );
                    await yieldToMainThread();
                }

                // 2) accelerations/*
                for (const f of accelFiles) {
                    if (runIdRef.current !== runId) return;

                    const parts = parsePartitions(f.webkitRelativePath);
                    const seriesIdFromPath = toNumberOrNull(parts["series_id"]);

                    // БАЗОВО: читаем без columns, чтобы не потерять nested.
                    const rows = await readParquetFile<ParquetAccelRow>(f);

                    // Опционально можно попытаться ускорить: сначала с columns, затем fallback.
                    // Оставлено выключенным, чтобы не ловить «пропавшие computed/filtered».
                    // let rows = await readParquetFile<ParquetAccelRow>(f, { columns: [...ACCEL_COLUMNS] });
                    // const nestedLooksMissing =
                    //     rows.length > 0 &&
                    //     rows.every((r) => (r as any).computed == null && (r as any).errors == null && (r as any).events == null);
                    // if (nestedLooksMissing) {
                    //     rows = await readParquetFile<ParquetAccelRow>(f);
                    // }

                    for (const r of rows) {
                        const sid =
                            toNumberOrNull((r as unknown as { series_id?: unknown }).series_id) ??
                            seriesIdFromPath;

                        if (sid == null) continue;

                        const series = seriesByNumericId.get(sid);
                        if (!series) continue;

                        const patched: ParquetAccelRow = {
                            ...r,
                            series_id: sid,
                        };

                        const variants = buildAccelAndSeriesAccelEntityVariantsFromParquetRow({
                            row: patched,
                            series,
                        });

                        for (const { accelId, accel, seriesAccel } of variants) {
                            if (!accelById.has(accelId)) {
                                accelById.set(accelId, accel);
                            }
                            seriesAccelList.push(seriesAccel);
                        }
                    }

                    filesDone += 1;
                    setLoadingWithRun(
                        runId,
                        "reading",
                        "Чтение accelerations (" + filesDone + "/" + totalFiles + ")",
                        filesDone,
                        totalFiles
                    );
                    await yieldToMainThread();
                }

                if (runIdRef.current !== runId) return;

                setLoadingWithRun(runId, "building", "Финализация", totalFiles, totalFiles);

                const seriesList = [...seriesByNumericId.values()].sort(
                    (a, b) => Number(a.id) - Number(b.id)
                );
                const accelList = [...accelById.values()];

                const experiment: Experiment = {
                    id: "0",
                    seriesList,
                    accelList,
                    seriesAccelList,
                };

                experimentRef.current = experiment;

                const count =
                    experiment.seriesAccelList?.length ?? experiment.seriesList?.length ?? 0;
                setSuccessWithRun(runId, count);
            } catch (e) {
                console.error("[useLoadParquetExperiment] error", e);
                setErrorWithRun(runId, e instanceof Error ? e.message : String(e));
            }
        },
        [setErrorWithRun, setLoadingWithRun, setSuccessWithRun]
    );

    return { state, load, experimentRef };
}
