// src/widgets/ExperimentSourceWrapper.tsx

import { type ReactNode, useState } from "react";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { ExperimentSourceSelector } from "@/widgets/ExperimentSourceSelector";

interface ExperimentSourceWrapperProps {
    onExperimentChange?: (exp: Experiment | null) => void;
    className?: string;
}

interface ContainerProps {
    className?: string;
    children: ReactNode;
}

function Container(props: ContainerProps) {
    const { className, children } = props;

    return (
        <div
            className={`w-full max-w-3xl rounded-lg border border-border bg-surface p-4 shadow-sm ${className ?? ""}`}
        >
            {children}
        </div>
    );
}

interface CurrentExperimentCardProps {
    experiment: Experiment;
    onReset: () => void;
}

function CurrentExperimentCard(props: CurrentExperimentCardProps) {
    const { experiment, onReset } = props;

    const seriesCount = experiment.seriesList?.length ?? 0;
    const accelCount = experiment.accelList?.length ?? 0;

    return (
        <Container>
            <div className="flex flex-col gap-3 sm:flex-row sm:items-center sm:justify-between">
                <div>
                    <div className="text-xs uppercase tracking-wide text-textDim">
                        Текущий эксперимент
                    </div>

                    <div className="mt-1 text-lg font-semibold text-text">
                        ID: <span className="font-mono">{experiment.id}</span>
                    </div>

                    <div className="mt-1 text-sm text-textDim">
                        Рядов: <span className="font-mono">{seriesCount}</span>
                        {" • "}
                        Алгоритмов: <span className="font-mono">{accelCount}</span>
                    </div>
                </div>

                <button
                    type="button"
                    onClick={onReset}
                    className="mt-2 inline-flex items-center justify-center rounded-md border border-border bg-surfaceHover px-3 py-1.5 text-sm font-medium hover:bg-surfaceStrong"
                >
                    Заменить эксперимент
                </button>
            </div>
        </Container>
    );
}

export function ExperimentSourceWrapper(props: ExperimentSourceWrapperProps) {
    const { onExperimentChange, className } = props;

    const [experiment, setExperiment] = useState<Experiment | null>(null);

    const handleChange = (exp: Experiment | null) => {
        setExperiment(exp);
        onExperimentChange?.(exp);
    };

    const handleReset = () => {
        setExperiment(null);
        onExperimentChange?.(null);
    };

    if (experiment) {
        return <CurrentExperimentCard experiment={experiment} onReset={handleReset} />;
    }

    return (
        <Container className={className}>
            <ExperimentSourceSelector onExperimentChange={handleChange} />
        </Container>
    );
}
