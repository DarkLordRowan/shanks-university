import { useCallback, useState } from "react";
import { API_BASE } from "../constants";
import { type ApiJsonResult } from "../types";
import { normalizeNetworkError } from "../utils/errors";

/**
 * Определение (интерфейс хука):
 * state: { isSending, jsonResult, jsonError, csvBlobUrl, csvText, csvError }
 * actions: { sendForJsonAndCsv(payload), resetResults }
 *
 * Совместимость: сигнатура и имена такие же, как в старой версии.
 * Изменение: внутри реализован протокол очереди задач (/process/json → polling /jobs/{uuid}/status → /jobs/{uuid}/docs/*).
 */
export function useApiProcessing() {
    const [isSending, setIsSending] = useState(false);
    const [jsonResult, setJsonResult] = useState<ApiJsonResult | null>(null);
    const [jsonError, setJsonError] = useState<string | null>(null);
    const [csvBlobUrl, setCsvBlobUrl] = useState<string | null>(null);
    const [csvText, setCsvText] = useState<string | null>(null);
    const [csvError, setCsvError] = useState<string | null>(null);

    const resetResults = useCallback(() => {
        setJsonResult(null);
        setJsonError(null);
        if (csvBlobUrl) URL.revokeObjectURL(csvBlobUrl);
        setCsvBlobUrl(null);
        setCsvText(null);
        setCsvError(null);
    }, [csvBlobUrl]);

    async function _startJob(payload: unknown): Promise<string> {
        const r = await fetch(`${API_BASE}/process/json`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload),
        });
        if (!r.ok) {
            const text = await r.text().catch(() => "");
            throw new Error(`Ошибка /process/json (${r.status}): ${text}`);
        }
        const data = await r.json() as { uuid: string };
        if (!data?.uuid) throw new Error("Сервис вернул пустой uuid");
        return data.uuid;
    }

    type JobStatus = "queued" | "processing" | "ready" | "failed";
    type StatusDoc = {
        uuid: string;
        status: JobStatus;
        created_at: string;
        completed_at: string | null;
        error: string | null;
    };

    async function _getStatus(uuid: string): Promise<StatusDoc> {
        const r = await fetch(`${API_BASE}/jobs/${uuid}/status`);
        if (!r.ok) {
            const text = await r.text().catch(() => "");
            throw new Error(`Ошибка /jobs/${uuid}/status (${r.status}): ${text}`);
        }
        return r.json() as Promise<StatusDoc>;
    }

    async function _getDocJson<T>(uuid: string, name: string): Promise<T> {
        const r = await fetch(`${API_BASE}/jobs/${uuid}/docs/${encodeURIComponent(name)}`);
        if (!r.ok) {
            const text = await r.text().catch(() => "");
            throw new Error(`Ошибка /jobs/${uuid}/docs/${name} (${r.status}): ${text}`);
        }
        return r.json() as Promise<T>;
    }

    async function _getDocCsv(uuid: string, name: string): Promise<{ url: string; text: string | null }> {
        const r = await fetch(`${API_BASE}/jobs/${uuid}/docs/${encodeURIComponent(name)}`);
        if (!r.ok) {
            const text = await r.text().catch(() => "");
            throw new Error(`Ошибка /jobs/${uuid}/docs/${name} (${r.status}): ${text}`);
        }
        const blob = await r.blob();
        const url = URL.createObjectURL(blob);
        let text: string | null = null;
        try {
            text = await blob.text();
        } catch {
            text = null;
        }
        return { url, text };
    }

    const sendForJsonAndCsv = useCallback(
        async (payload: unknown): Promise<{ jsonOk: boolean }> => {
            resetResults();
            setIsSending(true);

            try {
                const uuid = await _startJob(payload);

                // Polling циклами, без агрессивного тайминга
                // Параметры: 2 сек интервал, до 30 минут (1800 сек) по умолчанию.
                const intervalMs = 2000;
                const hardLimitMs = 30 * 60 * 1000;
                const start = Date.now();

                // eslint-disable-next-line no-constant-condition
                while (true) {
                    const st = await _getStatus(uuid);
                    if (st.status === "ready") break;
                    if (st.status === "failed") {
                        setJsonError(st.error ?? "Задача завершилась со статусом failed");
                        setIsSending(false);
                        return { jsonOk: false };
                    }
                    if (Date.now() - start > hardLimitMs) {
                        setJsonError("Таймаут ожидания результата");
                        setIsSending(false);
                        return { jsonOk: false };
                    }
                    await new Promise((res) => setTimeout(res, intervalMs));
                }

                // Документы готовы → забираем
                let nextJson: ApiJsonResult | null = null;

                const jsonReq = _getDocJson<ApiJsonResult>(uuid, "results.json")
                    .then((data) => { nextJson = data; setJsonResult(data); })
                    .catch((e) => setJsonError(normalizeNetworkError(e)));

                const csvReq = _getDocCsv(uuid, "results.csv")
                    .then(({ url, text }) => {
                        setCsvBlobUrl(url);
                        if (typeof text === "string") setCsvText(text);
                    })
                    .catch((e) => setCsvError(normalizeNetworkError(e)));

                await Promise.allSettled([jsonReq, csvReq]);
                setIsSending(false);
                return { jsonOk: !!nextJson };
            } catch (e) {
                setJsonError(normalizeNetworkError(e));
                setIsSending(false);
                return { jsonOk: false };
            }
        },
        [resetResults]
    );

    const loadByUuid = useCallback(
        async (uuid: string): Promise<{ jsonOk: boolean }> => {
            // сброс предыдущих результатов
            resetResults();
            setIsSending(true);
            try {
                const intervalMs = 2000;
                const hardLimitMs = 30 * 60 * 1000;
                const start = Date.now();

                // eslint-disable-next-line no-constant-condition
                while (true) {
                    const st = await _getStatus(uuid);
                    if (st.status === "ready") break;
                    if (st.status === "failed") {
                        setJsonError(st.error ?? "Задача завершилась со статусом failed");
                        setIsSending(false);
                        return { jsonOk: false };
                    }
                    if (Date.now() - start > hardLimitMs) {
                        setJsonError("Таймаут ожидания результата");
                        setIsSending(false);
                        return { jsonOk: false };
                    }
                    await new Promise((res) => setTimeout(res, intervalMs));
                }

                let nextJson: ApiJsonResult | null = null;

                const jsonReq = _getDocJson<ApiJsonResult>(uuid, "results.json")
                    .then((data) => { nextJson = data; setJsonResult(data); })
                    .catch((e) => setJsonError(normalizeNetworkError(e)));

                const csvReq = _getDocCsv(uuid, "results.csv")
                    .then(({ url, text }) => {
                        setCsvBlobUrl(url);
                        if (typeof text === "string") setCsvText(text);
                    })
                    .catch((e) => setCsvError(normalizeNetworkError(e)));

                await Promise.allSettled([jsonReq, csvReq]);
                setIsSending(false);
                return { jsonOk: !!nextJson };
            } catch (e) {
                setJsonError(normalizeNetworkError(e));
                setIsSending(false);
                return { jsonOk: false };
            }
        },
        [resetResults]
    );

    return {
        state: { isSending, jsonResult, jsonError, csvBlobUrl, csvText, csvError },
        actions: { sendForJsonAndCsv, loadByUuid, resetResults },
    };
}
