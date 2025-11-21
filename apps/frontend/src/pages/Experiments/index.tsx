// src/pages/Experiments.tsx

import { useState } from "react";
import type { Experiment } from "@/types/experiment";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput";
import { BackendExperimentLoader } from "@/features/load-experiment-from-backend/ui/BackendExperimentLoader";
import { AlgorithmSeriesDiffHeatmap } from "@/widgets/AlgorithmSeriesDiffHeatmap";
import { ErrorMatrixTable } from "@/widgets/ErrorMatrixTable";

type Source = "parquet" | "backend";

export default function Experiments() {
    const [source, setSource] = useState<Source>("parquet");
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto max-w-6xl space-y-6 p-4">
            {/* Переключатель источника */}
            <div className="inline-flex rounded-lg border border-border bg-surface/60 p-1">
                <button
                    type="button"
                    className={
                        "px-3 py-1 text-sm rounded-md transition " +
                        (source === "parquet"
                            ? "bg-primary text-primary-foreground shadow"
                            : "text-textDim hover:bg-surface/80")
                    }
                    onClick={() => setSource("parquet")}
                >
                    Из файлов parquet
                </button>
                <button
                    type="button"
                    className={
                        "px-3 py-1 text-sm rounded-md transition " +
                        (source === "backend"
                            ? "bg-primary text-primary-foreground shadow"
                            : "text-textDim hover:bg-surface/80")
                    }
                    onClick={() => setSource("backend")}
                >
                    Из backend по id
                </button>
            </div>

            {/* Блок загрузки эксперимента */}
            <div className="rounded-xl border border-border bg-surface/80 p-4 space-y-4">
                {source === "parquet" && (
                    <ParquetFolderInput onExperimentChange={setExperiment} />
                )}

                {source === "backend" && (
                    <BackendExperimentLoader onExperimentLoaded={setExperiment} />
                )}
            </div>

            {/* Просмотр эксперимента */}
            {experiment && (
                <div className="space-y-8">
                    <div className="text-sm text-textDim">
                        Текущий эксперимент: <span className="font-mono">{experiment.id ?? "без id"}</span>
                    </div>

                    <AlgorithmSeriesDiffHeatmap
                        experiment={experiment}
                        maxSeries={40}
                    />

                    <ErrorMatrixTable
                        experiment={experiment}
                        maxSteps={40}
                    />
                </div>
            )}

            {!experiment && (
                <div className="text-sm text-textDim">
                    Эксперимент не загружен. Выбери источник и загрузку выше.
                </div>
            )}
        </div>
    );
}
