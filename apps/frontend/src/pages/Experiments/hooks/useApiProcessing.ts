import { useCallback, useState } from "react";
import { API_BASE } from "../constants";
import { type ApiJsonResult } from "../types";
import { normalizeNetworkError } from "../utils/errors";

export function useApiProcessing() {
    const [isSending, setIsSending] = useState(false);
    const [jsonResult, setJsonResult] = useState<ApiJsonResult | null>(null);
    const [jsonError, setJsonError] = useState<string | null>(null);

    const resetResults = useCallback(() => {
        setJsonResult(null);
        setJsonError(null);
    }, []);

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

    // --- helpers ---

    // Защита строковых литералов: временно вырезаем их, чтобы не портить содержимое при замене Infinity/NaN.
    function _extractStrings(s: string): { stripped: string; bag: string[] } {
        const bag: string[] = [];
        const stripped = s.replace(/"(?:\\.|[^"\\])*"/g, (m) => {
            bag.push(m);
            return `__STR_${bag.length - 1}__`;
        });
        return { stripped, bag };
    }
    function _restoreStrings(s: string, bag: string[]): string {
        return s.replace(/__STR_(\d+)__/g, (_, i) => bag[+i]);
    }
    function _sanitizeInvalidJsonNumbers(raw: string): string {
        const { stripped, bag } = _extractStrings(raw);
        // Заменяем невалидные числовые токены только в местах значений (после :, [, { или пробела) и перед разделителем/закрывающей скобкой.
        const patched = stripped
            .replace(/([:\[,{\s])(-?Infinity)(?=[\s,\]}])/g, "$1null")
            .replace(/([:\[,{\s])NaN(?=[\s,\]}])/g, "$1null");
        return _restoreStrings(patched, bag);
    }

    async function _decompressGzipToText(blob: Blob): Promise<string> {
        // native
        if (typeof DecompressionStream !== "undefined") {
            const ds = new DecompressionStream("gzip");
            const stream = blob.stream().pipeThrough(ds);
            const ab = await new Response(stream).arrayBuffer();
            return new TextDecoder("utf-8").decode(ab);
        }
        // fallback: pako
        try {
            const { ungzip } = await import("pako");
            const ab = await blob.arrayBuffer();
            const out = ungzip(new Uint8Array(ab));
            return new TextDecoder("utf-8").decode(out);
        } catch {
            throw new Error("Gzip распаковка недоступна: ни DecompressionStream, ни pako.");
        }
    }

    // Универсальный парсер: сначала читаем текст с клона, пытаемся парсить; при падении — санируем Infinity/NaN;
    // если это был «сырое» .gz без Content-Encoding — читаем оригинал как blob и распаковываем с той же санацией.
    async function _jsonFromResponse<T>(r: Response): Promise<T> {
        // Быстрый путь: браузер уже разжал по Content-Encoding → text() даёт валидный текст
        const txt0 = await r.clone().text();
        try {
            return JSON.parse(txt0) as T;
        } catch {
            // Попытка с санацией невалидных чисел
            try {
                const fixed = _sanitizeInvalidJsonNumbers(txt0);
                return JSON.parse(fixed) as T;
            } catch {
                // Падаем в путь с ручной распаковкой (на случай results.json.gz без CE)
            }
        }

        // Ручная распаковка
        const blob = await r.blob();
        try {
            const ungz = await _decompressGzipToText(blob);
            try {
                return JSON.parse(ungz) as T;
            } catch {
                const fixed = _sanitizeInvalidJsonNumbers(ungz);
                return JSON.parse(fixed) as T;
            }
        } catch {
            // Последняя попытка: трактуем blob как текст без распаковки (если сервер ошибся заголовками)
            const asText = await new Response(blob).text();
            const fixed = _sanitizeInvalidJsonNumbers(asText);
            return JSON.parse(fixed) as T;
        }
    }

    async function _getDocJson<T>(uuid: string, name: string): Promise<T> {
        const r = await fetch(`${API_BASE}/jobs/${uuid}/docs/${encodeURIComponent(name)}`);
        if (!r.ok) {
            const text = await r.text().catch(() => "");
            throw new Error(`Ошибка /jobs/${uuid}/docs/${name} (${r.status}): ${text}`);
        }
        return _jsonFromResponse<T>(r);
    }

    const _pollReady = async (uuid: string): Promise<"ready" | "failed" | "timeout"> => {
        const intervalMs = 2000;
        const hardLimitMs = 30 * 60 * 1000;
        const start = Date.now();
        // eslint-disable-next-line no-constant-condition
        while (true) {
            const st = await _getStatus(uuid);
            if (st.status === "ready") return "ready";
            if (st.status === "failed") return "failed";
            if (Date.now() - start > hardLimitMs) return "timeout";
            await new Promise((res) => setTimeout(res, intervalMs));
        }
    };

    const _fetchResultsJson = async (uuid: string): Promise<boolean> => {
        let ok = false;
        await _getDocJson<ApiJsonResult>(uuid, "results.json")
            .then((data) => { setJsonResult(data); ok = true; })
            .catch((e) => setJsonError(normalizeNetworkError(e)));
        return ok;
    };

    const sendForJson = useCallback(
        async (payload: unknown): Promise<{ jsonOk: boolean }> => {
            resetResults();
            setIsSending(true);
            try {
                const uuid = await _startJob(payload);
                const st = await _pollReady(uuid);
                if (st === "failed") {
                    setJsonError("Задача завершилась со статусом failed");
                    setIsSending(false);
                    return { jsonOk: false };
                }
                if (st === "timeout") {
                    setJsonError("Таймаут ожидания результата");
                    setIsSending(false);
                    return { jsonOk: false };
                }
                const ok = await _fetchResultsJson(uuid);
                setIsSending(false);
                return { jsonOk: ok };
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
            resetResults();
            setIsSending(true);
            try {
                const st = await _pollReady(uuid);
                if (st === "failed") {
                    setJsonError("Задача завершилась со статусом failed");
                    setIsSending(false);
                    return { jsonOk: false };
                }
                if (st === "timeout") {
                    setJsonError("Таймаут ожидания результата");
                    setIsSending(false);
                    return { jsonOk: false };
                }
                const ok = await _fetchResultsJson(uuid);
                setIsSending(false);
                return { jsonOk: ok };
            } catch (e) {
                setJsonError(normalizeNetworkError(e));
                setIsSending(false);
                return { jsonOk: false };
            }
        },
        [resetResults]
    );

    return {
        state: { isSending, jsonResult, jsonError },
        actions: { sendForJson, loadByUuid, resetResults },
    };
}
