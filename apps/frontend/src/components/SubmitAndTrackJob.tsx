import React, { useCallback, useEffect, useRef, useState } from "react";
import type { ApiJsonResult } from "../pages/Experiments/types.ts";

type Props = {
    requestJson: string | null;
    label?: string;
    disabled?: boolean;
    apiBase?: string; // опционально, если нужно переопределять
    onUuid?: (uuid: string) => void;
    onResponse?: (responseJson: ApiJsonResult) => void;
    onError?: (message: string) => void;
};

type JobStatus = "queued" | "processing" | "ready" | "failed";
type StatusDoc = {
    uuid: string;
    status: JobStatus;
    created_at: string;
    completed_at: string | null;
    error: string | null;
};

export const SubmitAndTrackJob: React.FC<Props> = ({
                                                       requestJson,
                                                       label = "Отправить",
                                                       disabled,
                                                       apiBase = "/api", // подставьте ваш API_BASE или прокиньте из пропсов
                                                       onUuid,
                                                       onResponse,
                                                       onError,
                                                   }) => {
    const [isSending, setIsSending] = useState(false);
    const abortRef = useRef<AbortController | null>(null);

    // — helpers (минимально необходимые) —
    const sanitizeInvalidJsonNumbers = (raw: string) =>
        raw.replace(/(-?Infinity|NaN)/g, "null");

    const readJsonSmart = async <T,>(r: Response): Promise<T> => {
        try {
            return (await r.clone().json()) as T;
        } catch {
            const txt = await r.text();
            const fixed = sanitizeInvalidJsonNumbers(txt);
            return JSON.parse(fixed) as T;
        }
    };

    const startJob = useCallback(
        async (payload: unknown): Promise<string> => {
            const r = await fetch(`${apiBase}/process/json`, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(payload),
                signal: abortRef.current?.signal,
            });
            if (!r.ok) {
                const t = await r.text().catch(() => "");
                throw new Error(`Ошибка /process/json (${r.status}): ${t}`);
            }
            const data = (await r.json()) as { uuid: string };
            if (!data?.uuid) throw new Error("Сервис вернул пустой uuid");
            return data.uuid;
        },
        [apiBase]
    );

    const getStatus = useCallback(
        async (uuid: string): Promise<StatusDoc> => {
            const r = await fetch(`${apiBase}/jobs/${uuid}/status`, {
                signal: abortRef.current?.signal,
            });
            if (!r.ok) {
                const t = await r.text().catch(() => "");
                throw new Error(`Ошибка /jobs/${uuid}/status (${r.status}): ${t}`);
            }
            return (await r.json()) as StatusDoc;
        },
        [apiBase]
    );

    const getDocJson = useCallback(
        async <T,>(uuid: string, name: string): Promise<T> => {
            const r = await fetch(`${apiBase}/jobs/${uuid}/docs/${encodeURIComponent(name)}`, {
                signal: abortRef.current?.signal,
            });
            if (!r.ok) {
                const t = await r.text().catch(() => "");
                throw new Error(`Ошибка /jobs/${uuid}/docs/${name} (${r.status}): ${t}`);
            }
            return readJsonSmart<T>(r);
        },
        [apiBase]
    );

    const pollReady = useCallback(
        async (uuid: string): Promise<"ready" | "failed" | "timeout"> => {
            const intervalMs = 2000;
            const hardLimitMs = 30 * 60 * 1000;
            const start = Date.now();

            // eslint-disable-next-line no-constant-condition
            while (true) {
                const st = await getStatus(uuid);
                if (st.status === "ready") return "ready";
                if (st.status === "failed") return "failed";
                if (Date.now() - start > hardLimitMs) return "timeout";
                await new Promise((res) => setTimeout(res, intervalMs));
            }
        },
        [getStatus]
    );

    const handleClick = async () => {
        if (!requestJson || isSending) return;

        let payload: unknown;
        try {
            payload = JSON.parse(requestJson);
        } catch {
            onError?.("Некорректный JSON запроса");
            return;
        }

        abortRef.current?.abort();
        abortRef.current = new AbortController();

        setIsSending(true);
        try {
            const uuid = await startJob(payload);
            onUuid?.(uuid);

            const st = await pollReady(uuid);
            if (st === "failed") throw new Error("Задача завершилась со статусом failed");
            if (st === "timeout") throw new Error("Таймаут ожидания результата");

            const resp = await getDocJson<ApiJsonResult>(uuid, "results.json");
            onResponse?.(resp);
        } catch (e: any) {
            onError?.(e?.message ?? "Неизвестная ошибка");
        } finally {
            setIsSending(false);
        }
    };

    useEffect(() => {
        return () => {
            abortRef.current?.abort();
        };
    }, []);

    return (
        <button
            type="button"
            className="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black hover:opacity-90 disabled:opacity-60"
            disabled={disabled || isSending || !requestJson}
            onClick={handleClick}
        >
            {isSending ? "Отправка…" : label}
        </button>
    );
};
