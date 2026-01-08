// src/features/import-experiment-from-parquet/ui/ParquetZipUrlInput.tsx

import { useEffect, useMemo, useState } from "react";
import JSZip from "jszip";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { useLoadParquetExperiment } from "../model/useLoadParquetExperiment";

const DEFAULT_ZIP_URL = "DEFAULT_ZIP_URL"; // заглушка, подставишь реальную

type LoadState = ReturnType<typeof useLoadParquetExperiment>["state"];
type Phase = "idle" | "downloading" | "unzipping" | "error";

function getSummaryText(state: LoadState, phase: Phase): string {
    if (phase === "downloading") return "Скачивание…";
    if (phase === "unzipping") return "Распаковка…";
    if (phase === "error") return "Ошибка";

    switch (state.status) {
        case "success":
            return "Готово";
        case "loading":
            return "Загрузка…";
        case "error":
            return "Ошибка";
        default:
            return "Ссылка не задана";
    }
}

function StatusDetails(props: { state: LoadState; phase: Phase; phaseError: string | null }) {
    const { state, phase, phaseError } = props;

    if (phase === "downloading") {
        return <div className="text-xs text-textDim">Скачивание zip…</div>;
    }

    if (phase === "unzipping") {
        return <div className="text-xs text-textDim">Распаковка zip…</div>;
    }

    if (phase === "error") {
        return <div className="text-xs text-red-500">Ошибка: {phaseError ?? "unknown"}</div>;
    }

    if (state.status === "loading") {
        return <div className="text-xs text-textDim">{state.message}</div>;
    }

    if (state.status === "success") {
        return (
            <div className="text-xs text-textDim">
                Parquet обработан. Всего связок series-accel: {state.count}
            </div>
        );
    }

    if (state.status === "error") {
        return <div className="text-xs text-red-500">Ошибка: {state.message}</div>;
    }

    return null;
}

function ensureUrl(s: string): URL {
    try {
        return new URL(s);
    } catch {
        throw new Error("Некорректный URL");
    }
}

function attachRelativePath(file: File, relPath: string): File {
    // На случай если парсер завязан на webkitRelativePath (как при выборе папки)
    try {
        Object.defineProperty(file, "webkitRelativePath", {
            value: relPath,
            configurable: true,
        });
    } catch {
        // ignore
    }
    return file;
}

async function unzipParquetFiles(zipBytes: ArrayBuffer): Promise<File[]> {
    const zip = await JSZip.loadAsync(zipBytes);

    const parquetEntries = Object.values(zip.files).filter((f) => {
        if (f.dir) return false;
        return f.name.toLowerCase().endsWith(".parquet");
    });

    if (parquetEntries.length === 0) {
        throw new Error("В архиве не найдено файлов *.parquet");
    }

    const out: File[] = [];
    for (const entry of parquetEntries) {
        const blob = await entry.async("blob");

        // name оставляем с путями (entry.name), чтобы можно было восстановить структуру
        const f = new File([blob], entry.name, { type: "application/octet-stream" });
        out.push(attachRelativePath(f, entry.name));
    }

    out.sort((a, b) => a.name.localeCompare(b.name));
    return out;
}

interface ParquetZipUrlInputProps {
    onExperimentChange?: (experiment: Experiment | null) => void;
}

export function ParquetZipUrlInput({ onExperimentChange }: ParquetZipUrlInputProps) {
    const { state, load, experimentRef } = useLoadParquetExperiment();

    const [url, setUrl] = useState<string>("");
    const [phase, setPhase] = useState<Phase>("idle");
    const [phaseError, setPhaseError] = useState<string | null>(null);

    useEffect(() => {
        if (state.status === "success" && onExperimentChange) {
            onExperimentChange(experimentRef.current);
        }
    }, [state.status, onExperimentChange, experimentRef]);

    const isLoading = phase === "downloading" || phase === "unzipping" || state.status === "loading";
    const urlTrim = url.trim();
    const canLoad = !isLoading && urlTrim.length > 0;

    const summary = useMemo(() => getSummaryText(state, phase), [state, phase]);

    function onFillDefault() {
        setUrl(DEFAULT_ZIP_URL);
    }

    async function onLoadClick() {
        setPhaseError(null);

        let parsedUrl: URL;
        try {
            parsedUrl = ensureUrl(urlTrim);
        } catch (e) {
            setPhase("error");
            setPhaseError(e instanceof Error ? e.message : String(e));
            return;
        }

        try {
            setPhase("downloading");

            const resp = await fetch(parsedUrl.toString(), { mode: "cors" });
            if (!resp.ok) throw new Error(`HTTP ${resp.status} ${resp.statusText}`);

            const buf = await resp.arrayBuffer();

            setPhase("unzipping");
            const files = await unzipParquetFiles(buf);

            setPhase("idle");
            await load(files);
        } catch (e) {
            setPhase("error");
            setPhaseError(e instanceof Error ? e.message : String(e));
        }
    }

    return (
        <div className="space-y-2 text-sm">
            <div className="flex flex-col gap-2">
                <div className="flex gap-2 items-center">
                    <input
                        type="url"
                        value={url}
                        onChange={(e) => setUrl(e.target.value)}
                        placeholder={DEFAULT_ZIP_URL}
                        className={
                            "w-full rounded-md border px-3 py-1.5 bg-panel border-border " +
                            "text-textDim shadow-sm outline-none focus:border-primary"
                        }
                        disabled={isLoading}
                    />

                    <button
                        type="button"
                        onClick={onFillDefault}
                        disabled={isLoading || urlTrim === DEFAULT_ZIP_URL}
                        className={
                            "px-3 py-1.5 text-sm border rounded transition-colors " +
                            "bg-panel border-border text-textDim shadow-sm " +
                            (isLoading || urlTrim === DEFAULT_ZIP_URL
                                ? "opacity-60 cursor-not-allowed"
                                : "hover:border-primary hover:text-primary")
                        }
                    >
                        Пример
                    </button>

                    <button
                        type="button"
                        onClick={onLoadClick}
                        disabled={!canLoad}
                        className={
                            "px-3 py-1.5 text-sm border rounded transition-colors " +
                            "bg-panel border-border text-textDim shadow-sm " +
                            (!canLoad
                                ? "opacity-60 cursor-not-allowed"
                                : "hover:border-primary hover:text-primary")
                        }
                        aria-busy={isLoading}
                    >
                        Загрузить
                    </button>

                    <span className="text-xs text-textDim whitespace-nowrap">{summary}</span>
                </div>

                <div className="text-xs text-textDim">
                    Нужен прямой URL на zip (raw.githubusercontent.com или release asset). Кнопка
                    «Пример» подставляет базовую ссылку.
                </div>
            </div>

            <StatusDetails state={state} phase={phase} phaseError={phaseError} />
        </div>
    );
}
