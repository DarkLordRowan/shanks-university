// ui/StreamDiffFileInput.tsx
import { useEffect } from "react";
import type { DiffCell } from "@/types/diffCell";
import { useStreamLoadDiffCells } from "../model/useStreamLoadDiffCells";

type Side = "prev" | "next";

interface StreamDiffFileInputProps {
    side: Side;
    diffMapRef: React.MutableRefObject<Map<string, DiffCell>>;
    onDiffCellsChange?: (cells: DiffCell[]) => void;
    label?: string;
}

export function StreamDiffFileInput({
                                        side,
                                        diffMapRef,
                                        onDiffCellsChange,
                                        label,
                                    }: StreamDiffFileInputProps) {
    const { state, load } = useStreamLoadDiffCells(side, diffMapRef);

    useEffect(() => {
        if (state.status === "success" && onDiffCellsChange) {
            const cells = Array.from(diffMapRef.current.values());
            onDiffCellsChange(cells);
        }
    }, [state.status, diffMapRef, onDiffCellsChange]);

    return (
        <div className="space-y-2 text-sm">
            {label && <div className="font-medium">{label}</div>}
            <input
                type="file"
                accept="application/json"
                onChange={(e) => {
                    const f = e.target.files?.[0];
                    if (f) load(f);
                }}
            />

            {state.status === "loading" && (
                <div>Загружено объектов: {state.count}</div>
            )}

            {state.status === "success" && (
                <div>Файл обработан. Всего объектов: {state.count}</div>
            )}

            {state.status === "error" && (
                <div className="text-red-600">
                    Ошибка
                    <br />
                    {state.issues.map((x, i) => (
                        <div key={i}>
                            {x.path}: {x.message}
                        </div>
                    ))}
                </div>
            )}
        </div>
    );
}
