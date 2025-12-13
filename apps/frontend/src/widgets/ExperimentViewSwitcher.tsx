// src/widgets/ExperimentViewSwitcher.tsx

import { type ReactNode, useState } from "react";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { ErrorMatrixTable } from "@/widgets/ErrorMatrixTable";
import { AlgorithmSeriesDiffHeatmap } from "@/widgets/AlgorithmSeriesDiffHeatmap";
import { AlgorithmSeriesErrorMatrix } from "@/widgets/AlgorithmSeriesErrorMatrix";
import { AlgorithmSeriesConvergenceTable } from "@/widgets/AlgorithmSeriesConvergenceTable";
import { AlgoRankingTable } from "@/widgets/AlgoRankingTable.tsx";
import { AlgorithmSeriesErrorStatsTable } from "@/widgets/AlgorithmSeriesErrorStatsTable";

type ViewKey =
    | "error-matrix"
    | "series-diff"
    | "series-error"
    | "series-convergence"
    | "series-error-stats"
    | "algo-ranking";

interface ViewButtonProps {
    active: boolean;
    onClick: () => void;
    children: ReactNode;
}

function ViewButton(props: ViewButtonProps) {
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

interface ViewToggleProps {
    value: ViewKey;
    onChange: (value: ViewKey) => void;
}

function ViewToggle(props: ViewToggleProps) {
    const { value, onChange } = props;

    return (
        <div className="flex flex-wrap gap-2 mb-3">
            <ViewButton active={value === "error-matrix"} onClick={() => onChange("error-matrix")}>
                Таблица ошибок по шагам
            </ViewButton>

            <ViewButton active={value === "series-diff"} onClick={() => onChange("series-diff")}>
                Хитмап разницы алгоритмов
            </ViewButton>

            <ViewButton active={value === "series-error"} onClick={() => onChange("series-error")}>
                Матрица ошибок по рядам
            </ViewButton>

            <ViewButton
                active={value === "series-convergence"}
                onClick={() => onChange("series-convergence")}
            >
                Монотонность и направление
            </ViewButton>

            <ViewButton
                active={value === "series-error-stats"}
                onClick={() => onChange("series-error-stats")}
            >
                Ошибки
            </ViewButton>

            <ViewButton active={value === "algo-ranking"} onClick={() => onChange("algo-ranking")}>
                Ранги алгоритмов
            </ViewButton>
        </div>
    );
}

interface ExperimentViewSwitcherProps {
    experiment: Experiment | null;
    className?: string;
}

export function ExperimentViewSwitcher(props: ExperimentViewSwitcherProps) {
    const { experiment, className } = props;

    const [view, setView] = useState<ViewKey>("series-convergence");

    return (
        <div className={`w-full max-w-6xl ${className ?? ""}`}>
            <ViewToggle value={view} onChange={setView} />

            {experiment && (
                <div className="mt-2">
                    {view === "error-matrix" && <ErrorMatrixTable experiment={experiment} />}

                    {view === "series-diff" && (
                        <AlgorithmSeriesDiffHeatmap experiment={experiment} />
                    )}

                    {view === "series-error" && (
                        <AlgorithmSeriesErrorMatrix experiment={experiment} />
                    )}

                    {view === "series-convergence" && (
                        <AlgorithmSeriesConvergenceTable experiment={experiment} maxSeries={15} />
                    )}

                    {view === "series-error-stats" && (
                        <AlgorithmSeriesErrorStatsTable experiment={experiment} maxSeries={15} />
                    )}

                    {view === "algo-ranking" && <AlgoRankingTable experiment={experiment} />}
                </div>
            )}
        </div>
    );
}
