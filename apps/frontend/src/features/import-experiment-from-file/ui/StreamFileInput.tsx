import { useEffect } from "react";
import { useStreamLoadRecords } from "../model/useStreamLoadRecords";
import type { Item } from "@/types/item.ts";
import { normalizeRecords } from "@/utils/normalizeRecords.ts";

interface StreamFileInputProps {
    onItemsChange?: (items: Item[]) => void;
}

export function StreamFileInput({ onItemsChange }: StreamFileInputProps) {
    const { state, load, recordsRef } = useStreamLoadRecords();

    useEffect(() => {
        if (state.status === "success" && onItemsChange) {
            const items = normalizeRecords(recordsRef.current);
            onItemsChange(items);
        }
    }, [state.status]);

    return (
        <div className="space-y-2 text-sm">
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
                    Ошибка<br />
                    {state.issues.map((x, i) => (
                        <div key={i}>{x.path}: {x.message}</div>
                    ))}
                </div>
            )}
        </div>
    );
}
