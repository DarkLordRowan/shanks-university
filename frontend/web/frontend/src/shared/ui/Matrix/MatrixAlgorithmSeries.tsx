import type { Accel, Series } from "@/entities/experiment/model/experiment";
import {
    MatrixAlgorithmSeriesView,
    type MatrixAlgorithmSeriesViewProps,
} from "./MatrixAlgorithmSeriesView";
import { MatrixAlgoSeriesFilter } from "./filters/MatrixAlgoSeriesFilter";

export interface MatrixAlgorithmSeriesProps extends MatrixAlgorithmSeriesViewProps {
    enableFilters?: boolean;

    filterResetKey?: string | number;
    filterSessionKey?: string;
    groupAccelsBy?: (a: Accel) => { key: string; title?: string };
    groupSeriesBy?: (s: Series) => { key: string; title?: string };
}

export function MatrixAlgorithmSeries(props: MatrixAlgorithmSeriesProps) {
    const {
        enableFilters = true,
        filterResetKey,
        filterSessionKey,
        groupAccelsBy,
        groupSeriesBy,
        accelList,
        seriesList,
        ...viewProps
    } = props;

    if (!enableFilters) {
        return (
            <MatrixAlgorithmSeriesView
                accelList={accelList}
                seriesList={seriesList}
                {...viewProps}
            />
        );
    }

    return (
        <MatrixAlgoSeriesFilter
            accelList={accelList}
            seriesList={seriesList}
            resetKey={filterResetKey}
            sessionKey={filterSessionKey}
            groupAccelsBy={groupAccelsBy}
            groupSeriesBy={groupSeriesBy}
        >
            {({ filteredAccels, filteredSeries }) => (
                <MatrixAlgorithmSeriesView
                    accelList={filteredAccels}
                    seriesList={filteredSeries}
                    {...viewProps}
                />
            )}
        </MatrixAlgoSeriesFilter>
    );
}
