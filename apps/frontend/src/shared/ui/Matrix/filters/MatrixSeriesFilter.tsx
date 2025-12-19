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

    idMode: FilterMode;
    onIdMode: (m: FilterMode) => void;

    selectedIds: Set<string>;
    onClearIds: () => void;
}

export function MatrixSeriesFilter(props: MatrixSeriesFilterProps) {
    return <MatrixAxisFilter<Series> title="Ряды" hint="группировка по name" {...props} />;
}
