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

            const result = await loadFromFile(file);

            if (onLoaded && result.ok) {
                onLoaded(result.data.length);
            }
        },
        [loadFromFile, onLoaded],
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
                    <ul className="max-h-60 overflow-auto rounded border p-2 text-xs font-mono">
                        {state.issues.map((issue, idx) => (
                            <li key={idx} className="mb-2">
                                <div>{issue.path}</div>
                                {issue.objectIndex != null && (
                                    <div>objectIndex: {issue.objectIndex}</div>
                                )}
                                <div>{issue.message}</div>
                                {issue.snippet && (
                                    <pre className="mt-1 whitespace-pre-wrap rounded bg-black/30 p-1">
                            {issue.snippet}
                        </pre>
                                )}
                            </li>
                        ))}
                    </ul>
                </div>
            )}
        </div>
    );
}
