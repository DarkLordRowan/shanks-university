import type { Series } from "@/entities/experiment/model/experiment";
import { MatrixAxisFilter, type FilterMode, type Group } from "./MatrixAxisFilter";

export interface MatrixSeriesFilterProps {
    query: string;
    onQuery: (v: string) => void;

    groups: Group<Series>[];

    groupMode: FilterMode;
    onGroupMode: (m: FilterMode) => void;

    selectedGroupKeys: Set<string>;
    onToggleGroup: (key: string) => void;
    onSelectAllGroups: () => void;
    onClearGroups: () => void;
}

export function MatrixSeriesFilter(props: MatrixSeriesFilterProps) {
    return <MatrixAxisFilter<Series> title="Ряды" hint="группировка по name" {...props} />;
}
