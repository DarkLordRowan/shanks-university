import type { Accel } from "@/entities/experiment/model/experiment";
import { MatrixAxisFilter, type FilterMode, type Group } from "./MatrixAxisFilter";

export interface MatrixAccelsFilterProps {
    query: string;
    onQuery: (v: string) => void;

    groups: Group<Accel>[];

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

export function MatrixAccelsFilter(props: MatrixAccelsFilterProps) {
    return <MatrixAxisFilter<Accel> title="Алгоритмы" hint="группировка по name" {...props} />;
}
