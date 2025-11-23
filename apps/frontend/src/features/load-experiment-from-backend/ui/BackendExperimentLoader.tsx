import React, { useState } from "react";
import type { Experiment } from "@/entities/experiment/model/experiment";

interface BackendExperimentLoaderProps {
    onExperimentChange: (experiment: Experiment | null) => void;
}


export function BackendExperimentLoader(props: BackendExperimentLoaderProps) {
    const { onExperimentChange } = props;

    const [experimentId, setExperimentId] = useState("");
    const [status, setStatus] = useState<"idle" | "loading" | "error" | "success">("idle");
    const [message, setMessage] = useState<string | null>(null);

    async function handleSubmit(e: React.FormEvent) {
        e.preventDefault();
        if (!experimentId.trim()) {
            setStatus("error");
            setMessage("Укажи id эксперимента");
            onExperimentChange(null);
            return;
        }

        setStatus("loading");
        setMessage(null);

        try {
            // TODO: заменить на реальный запрос к backend
            // пример:
            // const resp = await fetch(`/api/experiments/${encodeURIComponent(experimentId)}`);
            // if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
            // const exp: Experiment = await resp.json();
            // onExperimentLoaded(exp);

            // Временная заглушка
            await new Promise((r) => setTimeout(r, 300));
            setStatus("error");
            setMessage("Заглушка: backend ещё не подключен");
            onExperimentChange(null);
        } catch (err) {
            setStatus("error");
            setMessage(err instanceof Error ? err.message : "Ошибка загрузки");
            onExperimentChange(null);
        }
    }

    return (
        <form onSubmit={handleSubmit} className="space-y-3">
            <div className="space-y-1">
                <label className="block text-sm font-medium text-text">
                    ID эксперимента
                </label>
                <input
                    type="text"
                    value={experimentId}
                    onChange={(e) => setExperimentId(e.target.value)}
                    className="w-full rounded-md border border-border bg-surface px-3 py-1.5 text-sm text-text outline-none focus:border-primary focus:ring-1 focus:ring-primary"
                    placeholder="например, 123e4567-e89b-12d3-a456-426614174000"
                />
            </div>

            <div className="flex items-center gap-3">
                <button
                    type="submit"
                    className="rounded-md bg-primary px-3 py-1.5 text-sm font-medium text-primary-foreground hover:bg-primary/90 disabled:opacity-60"
                    disabled={status === "loading"}
                >
                    {status === "loading" ? "Загрузка..." : "Загрузить из backend"}
                </button>

                {status === "error" && message && (
                    <span className="text-xs text-red-500">{message}</span>
                )}
                {status === "success" && (
                    <span className="text-xs text-green-500">
                        Эксперимент загружен
                    </span>
                )}
            </div>
        </form>
    );
}
