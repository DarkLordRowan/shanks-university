import { API_BASE } from "../pages/Experiments/constants";

export async function postJson<T = unknown>(path: string, body: unknown): Promise<T> {
    const r = await fetch(`${API_BASE}${path}`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
    });
    if (!r.ok) {
        const text = await r.text().catch(() => "");
        throw new Error(`Ошибка ${path} (${r.status}): ${text}`);
    }
    return r.json() as Promise<T>;
}

export async function postCsvBlob(path: string, body: unknown): Promise<Blob> {
    const r = await fetch(`${API_BASE}${path}`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
    });
    if (!r.ok) {
        const text = await r.text().catch(() => "");
        throw new Error(`Ошибка ${path} (${r.status}): ${text}`);
    }
    return r.blob();
}
