import React from "react";
import type { Accel, Experiment, Series } from "@/entities/experiment/model/experiment";
import { buildExperimentSessionStateKey } from "@/shared/lib/inMemorySessionState";
import {
    MatrixAlgoSeriesFilter,
    type MatrixAlgoSeriesFilterState,
} from "./MatrixAlgoSeriesFilter";

function buildFilterStateKey(state: MatrixAlgoSeriesFilterState): string {
    return [
        state.accel.query,
        state.accel.variantMode,
        state.accel.groupMode,
        Array.from(state.accel.selectedGroupKeys).sort().join(","),
        state.accel.mMinText,
        state.accel.mMaxText,
        state.accel.argsOp,
        state.accel.argClauses.map((c) => `${c.key}=${c.value}`).join("|"),
        state.series.query,
        state.series.groupMode,
        Array.from(state.series.selectedGroupKeys).sort().join(","),
        state.series.precisionMode,
        Array.from(state.series.selectedPrecisions).sort().join(","),
        state.series.argsOp,
        state.series.argClauses.map((c) => `${c.key}=${c.value}`).join("|"),
    ].join("::");
}

export interface ExperimentMatrixFilterScopeRenderArgs {
    experimentFiltered: Experiment | null;
    filteredAccels: Accel[];
    filteredSeries: Series[];
    filterState: MatrixAlgoSeriesFilterState | null;
    filterStateKey: string;
}

export interface ExperimentMatrixFilterScopeProps {
    experiment: Experiment | null;
    resetKey?: string | number;
    sessionKey?: string;
    children: (args: ExperimentMatrixFilterScopeRenderArgs) => React.ReactNode;
}

export function ExperimentMatrixFilterScope(props: ExperimentMatrixFilterScopeProps) {
    const { experiment, resetKey, sessionKey, children } = props;

    if (!experiment) {
        return (
            <>
                {children({
                    experimentFiltered: null,
                    filteredAccels: [],
                    filteredSeries: [],
                    filterState: null,
                    filterStateKey: "no-filter",
                })}
            </>
        );
    }

    const filterSessionKey =
        sessionKey ?? buildExperimentSessionStateKey(experiment.id, "matrix-filters");

    return (
        <MatrixAlgoSeriesFilter
            accelList={experiment.accelList ?? []}
            seriesList={experiment.seriesList ?? []}
            resetKey={resetKey ?? experiment.id}
            sessionKey={filterSessionKey}
        >
            {({ filteredAccels, filteredSeries, state }) => {
                const accelIds = new Set(filteredAccels.map((a) => a.id));
                const seriesIds = new Set(filteredSeries.map((s) => s.id));

                const filteredSeriesAccel = (experiment.seriesAccelList ?? []).filter(
                    (sa) => accelIds.has(sa.accel_id) && seriesIds.has(sa.series_id)
                );

                const experimentFiltered: Experiment = {
                    ...experiment,
                    accelList: filteredAccels,
                    seriesList: filteredSeries,
                    seriesAccelList: filteredSeriesAccel,
                };

                return children({
                    experimentFiltered,
                    filteredAccels,
                    filteredSeries,
                    filterState: state,
                    filterStateKey: buildFilterStateKey(state),
                });
            }}
        </MatrixAlgoSeriesFilter>
    );
}
