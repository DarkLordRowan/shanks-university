// src/features/import-experiment-from-parquet/model/useLoadParquetExpirement.ts

import { useCallback, useRef, useState } from "react";
import type { Experiment } from "@/types/experiment";
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

export type LoadParquetState =
    | { status: "idle" }
    | {
          status: "loading";
          phase: "reading" | "building";
          message: string;
          done: number;
          total: number;
      }
    | { status: "error"; message: string }
    | { status: "success"; count: number };

export function useLoadParquetExpirement() {
    const [state, setState] = useState<LoadParquetState>({ status: "idle" });
    const experimentRef = useRef<Experiment | null>(null);

    const load = useCallback(async (files: FileList | null) => {
        if (!files || files.length === 0) {
            setState({ status: "error", message: "Нет файлов" });
            return;
        }

        const allFiles = Array.from(files);

        const seriesFiles = allFiles.filter((f) => f.webkitRelativePath.includes("/series/"));
        const accelFiles = allFiles.filter((f) => f.webkitRelativePath.includes("/accelerations/"));

        if (seriesFiles.length === 0 || accelFiles.length === 0) {
            setState({
                status: "error",
                message: "Не найдены каталоги 'series' или 'accelerations'",
            });
            return;
        }

        const filesTotal = seriesFiles.length + accelFiles.length;
        let filesDone = 0;

        setState({
            status: "loading",
            phase: "reading",
            message: "Чтение Parquet",
            done: filesDone,
            total: filesTotal,
        });

        const seriesRows: ParquetSeriesRow[] = [];
        const accelRows: ParquetAccelRow[] = [];

        try {
            for (const f of seriesFiles) {
                const parts = parsePartitions(f.webkitRelativePath);
                const precisionFromPath = parts["precision"];
                const seriesNameFromPath = parts["series_name"];

                const rows = await readParquetFile<ParquetSeriesRow>(f);

                for (const r of rows) {
                    const sid = toNumberOrNull((r as any).series_id);

                    const row: ParquetSeriesRow = {
                        ...r,
                        series_id: sid ?? -1,
                        precision: (r as any).precision ?? precisionFromPath ?? "",
                        series_name: (r as any).series_name ?? seriesNameFromPath ?? "",
                    };

                    seriesRows.push(row);
                }

                filesDone += 1;
                setState({
                    status: "loading",
                    phase: "reading",
                    message: `Чтение series (${filesDone}/${filesTotal})`,
                    done: filesDone,
                    total: filesTotal,
                });
            }

            for (const f of accelFiles) {
                const parts = parsePartitions(f.webkitRelativePath);
                const seriesIdFromPath = toNumberOrNull(parts["series_id"]);

                const rows = await readParquetFile<ParquetAccelRow>(f);

                for (const r of rows) {
                    const sid = toNumberOrNull((r as any).series_id) ?? seriesIdFromPath;

                    const row: ParquetAccelRow = {
                        ...r,
                        series_id: sid ?? -1,
                    };

                    accelRows.push(row);
                }

                filesDone += 1;
                setState({
                    status: "loading",
                    phase: "reading",
                    message: `Чтение accelerations (${filesDone}/${filesTotal})`,
                    done: filesDone,
                    total: filesTotal,
                });
            }

            setState({
                status: "loading",
                phase: "building",
                message: "Построение эксперимента",
                done: 0,
                total: accelRows.length || 1,
            });

            const experiment = await buildExperimentFromParquet(
                seriesRows,
                accelRows,
                (processed, total) => {
                    setState({
                        status: "loading",
                        phase: "building",
                        message: `Построение эксперимента (${processed}/${total})`,
                        done: processed,
                        total,
                    });
                }
            );

            experimentRef.current = experiment;

            const count = experiment.seriesAccelList?.length ?? experiment.seriesList?.length ?? 0;

            setState({
                status: "success",
                count,
            });
        } catch (e) {
            console.error("[useLoadParquetExpirement] error", e);
            setState({
                status: "error",
                message: e instanceof Error ? e.message : String(e),
            });
        }
    }, []);

    return { state, load, experimentRef };
}
