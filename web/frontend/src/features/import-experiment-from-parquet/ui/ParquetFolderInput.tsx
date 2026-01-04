// src/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx

import { type RefObject, useEffect, useRef } from "react";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { useLoadParquetExperiment } from "../model/useLoadParquetExperiment.ts";

type LoadState = ReturnType<typeof useLoadParquetExperiment>["state"];

function getSummaryText(state: LoadState): string {
    switch (state.status) {
        case "success":
            return "Готово";
        case "loading":
            return "Загрузка…";
        case "error":
            return "Ошибка";
        default:
            return "Файлы не выбраны";
    }
}

interface StatusDetailsProps {
    state: LoadState;
}

function StatusDetails({ state }: StatusDetailsProps) {
    if (state.status === "loading") {
        return <div className="text-xs text-textDim">{state.message}</div>;
    }

    if (state.status === "success") {
        return (
            <div className="text-xs text-textDim">
                Parquet обработан. Всего связок series-accel: {state.count}
            </div>
        );
    }

    if (state.status === "error") {
        return <div className="text-xs text-red-500">Ошибка: {state.message}</div>;
    }

    return null;
}

interface FolderSelectLabelProps {
    state: LoadState;
    isLoading: boolean;
    inputRef: RefObject<HTMLInputElement | null>;
    onFiles: (files: FileList) => void;
}

function FolderSelectLabel(props: FolderSelectLabelProps) {
    const { state, isLoading, inputRef, onFiles } = props;

    return (
        <label className="inline-flex items-center gap-3 cursor-pointer">
            <span
                className={
                    "inline-flex items-center rounded-md border px-3 py-1.5 " +
                    "bg-panel border-border text-textDim shadow-sm " +
                    (isLoading
                        ? "opacity-60 cursor-not-allowed"
                        : "hover:border-primary hover:text-primary")
                }
                aria-busy={isLoading}
            >
                Выбрать папку с parquet
            </span>

            <span className="text-xs text-textDim">{getSummaryText(state)}</span>

            <input
                ref={inputRef}
                type="file"
                className="hidden"
                multiple
                accept=".parquet"
                disabled={isLoading}
                onChange={(e) => {
                    const files = e.target.files;
                    if (files && files.length > 0) onFiles(files);
                }}
            />
        </label>
    );
}

interface ParquetFolderInputProps {
    onExperimentChange?: (experiment: Experiment | null) => void;
}

export function ParquetFolderInput({ onExperimentChange }: ParquetFolderInputProps) {
    const { state, load, experimentRef } = useLoadParquetExperiment();
    const inputRef = useRef<HTMLInputElement | null>(null);

    useEffect(() => {
        if (state.status === "success" && onExperimentChange) {
            onExperimentChange(experimentRef.current);
        }
    }, [state.status, onExperimentChange, experimentRef]);

    type DirInput = HTMLInputElement & { webkitdirectory?: boolean };

    useEffect(() => {
        if (inputRef.current) {
            (inputRef.current as DirInput).webkitdirectory = true;
        }
    }, []);

    const isLoading = state.status === "loading";

    return (
        <div className="space-y-2 text-sm">
            <FolderSelectLabel
                state={state}
                isLoading={isLoading}
                inputRef={inputRef}
                onFiles={load}
            />

            <StatusDetails state={state} />
        </div>
    );
}
