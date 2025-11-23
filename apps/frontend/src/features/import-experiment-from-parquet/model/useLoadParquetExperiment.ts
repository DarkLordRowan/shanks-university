// src/features/import-experiment-from-parquet/model/useLoadParquetExpirement.ts

import { useCallback, useRef, useState } from "react";
import type { Experiment } from "@/entities/experiment/model/experiment";
import type { ParquetAccelRow, ParquetSeriesRow } from "@/shared/parquet/types";
import { readParquetFile } from "@/shared/parquet/readParquetFile";
import { buildExperimentFromParquet } from "@/shared/parquet/buildExperimentFromParquet";

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

type LoadingSetter = (phase: Phase, message: string, done: number, total: number) => void;

function splitFilesByKind(allFiles: File[]) {
    const seriesFiles = allFiles.filter((f) => f.webkitRelativePath.includes("/series/"));
    const accelFiles = allFiles.filter((f) => f.webkitRelativePath.includes("/accelerations/"));
    return { seriesFiles, accelFiles };
}

async function readSeriesFiles(
    files: File[],
    totalFiles: number,
    offset: number,
    updateLoading: LoadingSetter
): Promise<{ rows: ParquetSeriesRow[]; filesDone: number }> {
    const seriesRows: ParquetSeriesRow[] = [];
    let filesDone = offset;

    for (const f of files) {
        const parts = parsePartitions(f.webkitRelativePath);
        const precisionFromPath = parts["precision"];
        const seriesNameFromPath = parts["series_name"];

        const rows = await readParquetFile<ParquetSeriesRow>(f);

        for (const r of rows) {
            const sid = toNumberOrNull((r as unknown as { series_id?: unknown }).series_id);

            const row: ParquetSeriesRow = {
                ...r,
                series_id: sid ?? -1,
                precision: (r as any).precision ?? precisionFromPath ?? "",
                series_name: (r as any).series_name ?? seriesNameFromPath ?? "",
            };

            seriesRows.push(row);
        }

        filesDone += 1;
        updateLoading(
            "reading",
            `Чтение series (${filesDone}/${totalFiles})`,
            filesDone,
            totalFiles
        );
    }

    return { rows: seriesRows, filesDone };
}

async function readAccelFiles(
    files: File[],
    totalFiles: number,
    offset: number,
    updateLoading: LoadingSetter
): Promise<{ rows: ParquetAccelRow[]; filesDone: number }> {
    const accelRows: ParquetAccelRow[] = [];
    let filesDone = offset;

    for (const f of files) {
        const parts = parsePartitions(f.webkitRelativePath);
        const seriesIdFromPath = toNumberOrNull(parts["series_id"]);

        const rows = await readParquetFile<ParquetAccelRow>(f);

        for (const r of rows) {
            const sid =
                toNumberOrNull((r as unknown as { series_id?: unknown }).series_id) ??
                seriesIdFromPath;

            const row: ParquetAccelRow = {
                ...r,
                series_id: sid ?? -1,
            };

            accelRows.push(row);
        }

        filesDone += 1;
        updateLoading(
            "reading",
            `Чтение accelerations (${filesDone}/${totalFiles})`,
            filesDone,
            totalFiles
        );
    }

    return { rows: accelRows, filesDone };
}

export function useLoadParquetExperiment() {
    const [state, setState] = useState<LoadParquetState>({ status: "idle" });
    const experimentRef = useRef<Experiment | null>(null);

    const runIdRef = useRef(0);

    const setLoadingWithRun = useCallback(
        (runId: number, phase: Phase, message: string, done: number, total: number) => {
            if (runIdRef.current !== runId) return;
            setState({
                status: "loading",
                phase,
                message,
                done,
                total,
            });
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
        async (files: FileList | null) => {
            const runId = ++runIdRef.current;

            if (!files || files.length === 0) {
                setErrorWithRun(runId, "Нет файлов");
                return;
            }

            const allFiles = Array.from(files);
            const { seriesFiles, accelFiles } = splitFilesByKind(allFiles);

            if (seriesFiles.length === 0 || accelFiles.length === 0) {
                setErrorWithRun(runId, "Не найдены каталоги 'series' или 'accelerations'");
                return;
            }

            const filesTotal = seriesFiles.length + accelFiles.length;

            const updateLoading: LoadingSetter = (phase, message, done, total) =>
                setLoadingWithRun(runId, phase, message, done, total);

            updateLoading("reading", "Чтение Parquet", 0, filesTotal);

            try {
                const { rows: seriesRows, filesDone: afterSeries } = await readSeriesFiles(
                    seriesFiles,
                    filesTotal,
                    0,
                    updateLoading
                );

                const { rows: accelRows } = await readAccelFiles(
                    accelFiles,
                    filesTotal,
                    afterSeries,
                    updateLoading
                );

                updateLoading("building", "Построение эксперимента", 0, accelRows.length || 1);

                const experiment = await buildExperimentFromParquet(
                    seriesRows,
                    accelRows,
                    (processed, total) => {
                        updateLoading(
                            "building",
                            `Построение эксперимента (${processed}/${total})`,
                            processed,
                            total
                        );
                    }
                );

                if (runIdRef.current !== runId) return;

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
