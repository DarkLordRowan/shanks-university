// src/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx

import { useEffect } from "react";
import type { Experiment } from "@/types/experiment";
import { useLoadParquetExpirement } from "../model/useLoadParquetExpirement";

interface ParquetFolderInputProps {
    onExperimentChange?: (experiment: Experiment | null) => void;
}

export function ParquetFolderInput({ onExperimentChange }: ParquetFolderInputProps) {
    const { state, load, experimentRef } = useLoadParquetExpirement();

    useEffect(() => {
        if (state.status === "success" && onExperimentChange) {
            onExperimentChange(experimentRef.current);
        }
    }, [state.status, onExperimentChange]);

    return (
        <div className="space-y-2 text-sm">
            <input
                type="file"
                // важно для выбора папки
                // eslint-disable-next-line @typescript-eslint/ban-ts-comment
                // @ts-ignore
                webkitdirectory="true"
                multiple
                accept=".parquet"
                onChange={(e) => {
                    load(e.target.files);
                }}
            />

            {state.status === "loading" && (
                <div>
                    {state.message} ({state.filesDone}/{state.filesTotal})
                </div>
            )}

            {state.status === "success" && (
                <div>
                    Parquet обработан. Всего связок series-accel: {state.count}
                </div>
            )}

            {state.status === "error" && (
                <div className="text-red-600">
                    Ошибка: {state.message}
                </div>
            )}
        </div>
    );
}
