import React, { useCallback } from "react";
import { useLoadResponseRecordsFromFile } from "../model/useLoadResponseRecordsFromFile";

type Props = {
    onLoaded?: (dataCount: number) => void;
};

export function ResponseRecordsFileInput({ onLoaded }: Props) {
    const { state, loadFromFile, reset, isLoading, isError, isSuccess } =
        useLoadResponseRecordsFromFile();

    const handleChange = useCallback(
        async (event: React.ChangeEvent<HTMLInputElement>) => {
            const file = event.target.files?.[0];
            if (!file) return;

            await loadFromFile(file);

            if (onLoaded && state.status === "success") {
                onLoaded(state.data.length);
            }
        },
        [loadFromFile, onLoaded, state.status, state],
    );

    return (
        <div className="space-y-2 text-sm">
            <div className="flex items-center gap-2">
                <input
                    type="file"
                    accept="application/json"
                    onChange={handleChange}
                    disabled={isLoading}
                />
                {isLoading && <span>Загрузка и валидация…</span>}
                {isSuccess && (
                    <button
                        type="button"
                        onClick={reset}
                        className="rounded border px-2 py-1 text-xs"
                    >
                        Сбросить
                    </button>
                )}
            </div>

            {isSuccess && state.status === "success" && (
                <div className="text-green-700">
                    Успешно: элементов в массиве: {state.data.length}
                </div>
            )}

            {isError && state.status === "error" && (
                <div className="space-y-1">
                    <div className="font-semibold text-red-700">
                        Ошибки валидации:
                    </div>
                    <ul className="max-h-40 overflow-auto rounded border p-2 text-xs">
                        {state.issues.map((issue, idx) => (
                            <li key={idx} className="mb-1">
                                <div className="font-mono">{issue.path}</div>
                                <div>{issue.message}</div>
                                <div className="text-[10px] text-gray-500">
                                    code: {issue.code}
                                </div>
                            </li>
                        ))}
                    </ul>
                </div>
            )}
        </div>
    );
}
