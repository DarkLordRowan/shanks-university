// src/widgets/ExperimentExplorer.tsx

import { type ReactNode, useState } from "react";
import type { Experiment } from "@/types/experiment";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput";
import { BackendExperimentLoader } from "@/features/load-experiment-from-backend/ui/BackendExperimentLoader";

type Source = "parquet" | "backend";

interface SourceButtonProps {
    active: boolean;
    onClick: () => void;
    children: ReactNode;
}

function SourceButton(props: SourceButtonProps) {
    const { active, onClick, children } = props;

    const base = "px-2 py-1 text-sm border rounded transition-colors";
    const activeCls = "bg-primary text-primary-foreground border-primary shadow-sm";
    const inactiveCls = "border-border text-textDim hover:bg-surface/60";

    return (
        <button
            type="button"
            onClick={onClick}
            aria-pressed={active}
            className={base + " " + (active ? activeCls : inactiveCls)}
        >
            {children}
        </button>
    );
}

interface SourceToggleProps {
    value: Source;
    onChange: (value: Source) => void;
}

function SourceToggle(props: SourceToggleProps) {
    const { value, onChange } = props;

    return (
        <div className="flex gap-2 mb-2">
            <SourceButton
                active={value === "parquet"}
                onClick={() => onChange("parquet")}
            >
                Из файлов parquet
            </SourceButton>

            <SourceButton
                active={value === "backend"}
                onClick={() => onChange("backend")}
            >
                Из backend по id
            </SourceButton>
        </div>
    );
}

interface ExperimentSourceSelectorProps {
    onExperimentChange: (experiment: Experiment | null) => void;
}

export function ExperimentSourceSelector(props: ExperimentSourceSelectorProps) {
    const { onExperimentChange } = props;
    const [source, setSource] = useState<Source>("parquet");

    return (
        <div className="space-y-3">
            <SourceToggle value={source} onChange={setSource} />

            {source === "parquet" && (
                <ParquetFolderInput onExperimentChange={onExperimentChange} />
            )}

            {source === "backend" && (
                <BackendExperimentLoader onExperimentChange={onExperimentChange} />
            )}
        </div>
    );
}
