import React, { useMemo, useRef, useState } from "react";

/**
 * Эксперименты: загрузка JSON, отправка во внешнее API и получение JSON+CSV.
 * API: https://shanks-worker.mirea-kmbo.ru/api
 *
 * Мы отправляем ОДИН JSON-пейлоад и параллельно получаем:
 *  - JSON  через POST /process/json
 *  - CSV   через POST /process/csv
 */
const API_BASE = "https://shanks-worker.mirea-kmbo.ru/api";

type ApiJsonResult = any;

const Experiments: React.FC = () => {
  const [rawText, setRawText] = useState<string>("");
  const [fileName, setFileName] = useState<string>("");
  const [jsonPayload, setJsonPayload] = useState<any | null>(null);

  const [isSending, setIsSending] = useState(false);

  const [jsonResult, setJsonResult] = useState<ApiJsonResult | null>(null);
  const [jsonError, setJsonError] = useState<string | null>(null);

  const [csvBlobUrl, setCsvBlobUrl] = useState<string | null>(null);
  const [csvPreview, setCsvPreview] = useState<string | null>(null);
  const [csvError, setCsvError] = useState<string | null>(null);

  const [generalError, setGeneralError] = useState<string | null>(null);

  const fileInputRef = useRef<HTMLInputElement | null>(null);

  const prettyPayload = useMemo(() => {
    if (!jsonPayload) return "";
    try {
      return JSON.stringify(jsonPayload, null, 2);
    } catch {
      return "";
    }
  }, [jsonPayload]);

  // ---------- helpers ----------
  const normalizeNetworkError = (e: unknown) => {
    if (e instanceof TypeError && e.message === "Failed to fetch") {
      return "Не удалось выполнить запрос (возможна CORS/сетевая ошибка). Проверьте доступность API и CORS.";
    }
    if (e instanceof Error && e.message) return e.message;
    return "Неизвестная ошибка сети.";
  };

  const handleFile = async (file: File) => {
    try {
      const text = await file.text();
      setRawText(text);
      setFileName(file.name);
      const parsed = JSON.parse(text);
      setJsonPayload(parsed);

      setGeneralError(null);
      setJsonError(null);
      setCsvError(null);
    } catch {
      setGeneralError("Не удалось прочитать/распарсить JSON-файл.");
      setJsonPayload(null);
    }
  };

  const onFileChange: React.ChangeEventHandler<HTMLInputElement> = async (e) => {
    const f = e.target.files?.[0];
    if (f) await handleFile(f);
  };

  const onDrop: React.DragEventHandler<HTMLDivElement> = async (e) => {
    e.preventDefault();
    if (e.dataTransfer.files?.length) {
      await handleFile(e.dataTransfer.files[0]);
    }
  };

  const onPasteClick = () => {
    try {
      const parsed = JSON.parse(rawText);
      setJsonPayload(parsed);
      setGeneralError(null);
      setJsonError(null);
      setCsvError(null);
      setFileName("");
      if (fileInputRef.current) fileInputRef.current.value = "";
    } catch {
      setGeneralError("Вставленный текст не является корректным JSON.");
      setJsonPayload(null);
    }
  };

  const reset = () => {
    setRawText("");
    setFileName("");
    setJsonPayload(null);

    setIsSending(false);

    setJsonResult(null);
    setJsonError(null);

    setCsvBlobUrl(null);
    setCsvPreview(null);
    setCsvError(null);

    setGeneralError(null);
    if (fileInputRef.current) fileInputRef.current.value = "";
  };

  // ---------- main action: send once, get JSON + CSV ----------
  const sendForJsonAndCsv = async () => {
    if (!jsonPayload) {
      setGeneralError("Сначала загрузите или вставьте корректный JSON.");
      return;
    }

    setIsSending(true);
    setGeneralError(null);

    // сбрасываем прошлые результаты
    setJsonResult(null);
    setJsonError(null);
    setCsvBlobUrl(null);
    setCsvPreview(null);
    setCsvError(null);

    const jsonReq = fetch(`${API_BASE}/process/json`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(jsonPayload),
    })
        .then(async (r) => {
          if (!r.ok) throw new Error(`Ошибка /process/json (${r.status}): ${await r.text()}`);
          return r.json();
        })
        .then((data) => setJsonResult(data))
        .catch((e) => setJsonError(normalizeNetworkError(e)));

    const csvReq = fetch(`${API_BASE}/process/csv`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(jsonPayload),
    })
        .then(async (r) => {
          if (!r.ok) throw new Error(`Ошибка /process/csv (${r.status}): ${await r.text()}`);
          const blob = await r.blob();
          const url = URL.createObjectURL(blob);
          setCsvBlobUrl(url);
          // превью первых ~50 строк
          const text = await blob.text();
          const lines = text.split(/\r?\n/).slice(0, 50).join("\n");
          setCsvPreview(lines);
        })
        .catch((e) => setCsvError(normalizeNetworkError(e)));

    await Promise.allSettled([jsonReq, csvReq]);
    setIsSending(false);
  };

  return (
      <div className="mx-auto max-w-5xl px-4 py-6">
        <h1 className="mb-4 text-2xl font-bold">Эксперименты</h1>
        <p className="mb-6 text-textDim">
          Загрузите JSON с параметрами серий и методов. Мы отправим его во внешнее API и получим результаты в форматах JSON и CSV.
        </p>

        <div className="grid gap-6 md:grid-cols-2">
          {/* Ввод */}
          <div className="rounded-2xl border border-border/60 bg-panel/70 p-4">
            <h2 className="mb-3 text-lg font-semibold">Входные данные</h2>

            {/* Drag-n-drop зона */}
            <div
                onDrop={onDrop}
                onDragOver={(e) => e.preventDefault()}
                className="mb-3 flex cursor-pointer items-center justify-center rounded-xl border border-dashed border-border/70 p-6 text-center text-sm text-textDim hover:bg-panel/60"
                onClick={() => fileInputRef.current?.click()}
                title="Перетащите файл JSON сюда или нажмите для выбора"
            >
              {fileName ? (
                  <div>
                    <div className="font-medium text-text">Файл: {fileName}</div>
                    <div className="text-xs text-textDim">Нажмите, чтобы выбрать другой файл</div>
                  </div>
              ) : (
                  <div>
                    <div className="font-medium text-text">Перетащите JSON-файл сюда</div>
                    <div className="text-xs text-textDim">или нажмите для выбора файла</div>
                  </div>
              )}
            </div>
            <input
                ref={fileInputRef}
                type="file"
                accept="application/json,.json"
                onChange={onFileChange}
                className="hidden"
            />

            <label className="mb-1 block text-sm font-medium">Или вставьте JSON вручную</label>
            <textarea
                className="h-48 w-full resize-y rounded-xl border border-border/60 bg-panel/80 p-2 font-mono text-sm text-text"
                placeholder='{
  "series": [ ... ],
  "methods": [ ... ]
}'
                value={rawText}
                onChange={(e) => setRawText(e.target.value)}
            />

            <div className="mt-3 flex gap-3">
              <button
                  className="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-primary-foreground hover:opacity-90"
                  onClick={onPasteClick}
              >
                Использовать вставленный JSON
              </button>
              <button
                  className="rounded-xl border border-border px-4 py-2 text-sm"
                  onClick={reset}
              >
                Сбросить
              </button>
            </div>

            {prettyPayload && (
                <div className="mt-4">
                  <h3 className="mb-1 text-sm font-semibold text-secondary">Проверка распознанного JSON</h3>
                  <pre className="max-h-64 overflow-auto rounded-xl border border-border/60 bg-panel/80 p-3 text-xs leading-relaxed text-text whitespace-pre-wrap">
{prettyPayload}
              </pre>
                </div>
            )}
          </div>

          {/* Отправка и результаты */}
          <div className="rounded-2xl border border-border/60 bg-panel/70 p-4">
            <h2 className="mb-3 text-lg font-semibold">Запрос к API</h2>

            <div className="flex flex-wrap gap-3">
              <button
                  className="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-primary-foreground hover:opacity-90 disabled:opacity-60"
                  onClick={sendForJsonAndCsv}
                  disabled={isSending || !jsonPayload}
              >
                {isSending ? "Отправка… (JSON + CSV)" : "Отправить (JSON + CSV)"}
              </button>
            </div>

            {generalError && (
                <div className="mt-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">
                  {generalError}
                </div>
            )}

            {/* JSON результат / ошибка */}
            <div className="mt-6">
              <div className="mb-2 flex items-center justify-between">
                <h3 className="text-base font-semibold">Результат JSON</h3>
                {jsonResult && (
                    <a
                        className="rounded-lg border border-border px-3 py-1 text-sm hover:bg-panel/60"
                        href={URL.createObjectURL(
                            new Blob([JSON.stringify(jsonResult, null, 2)], { type: "application/json" })
                        )}
                        download="results.json"
                    >
                      Скачать JSON
                    </a>
                )}
              </div>
              {jsonError && (
                  <div className="rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">
                    {jsonError}
                  </div>
              )}
              {jsonResult && (
                  <pre className="mt-2 max-h-80 overflow-auto rounded-xl border border-border/60 bg-background/60 p-3 text-xs leading-relaxed text-text whitespace-pre">
{JSON.stringify(jsonResult, null, 2)}
              </pre>
              )}
            </div>

            {/* CSV результат / ошибка */}
            <div className="mt-6">
              <div className="mb-2 flex items-center justify-between">
                <h3 className="text-base font-semibold">Результат CSV</h3>
                {csvBlobUrl && (
                    <a
                        className="rounded-lg border border-border px-3 py-1 text-sm hover:bg-panel/60"
                        href={csvBlobUrl}
                        download="results.csv"
                    >
                      Скачать CSV
                    </a>
                )}
              </div>
              {csvError && (
                  <div className="rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">
                    {csvError}
                  </div>
              )}
              {csvPreview && (
                  <>
                    <div className="mb-2 text-xs text-textDim">Предпросмотр (первые строки):</div>
                    <pre className="max-h-64 overflow-auto rounded-xl border border-border/60 bg-background/60 p-3 text-xs leading-relaxed text-text whitespace-pre">
{csvPreview}
                </pre>
                  </>
              )}
            </div>
          </div>
        </div>

        <div className="mt-8 text-xs text-textDim">
          Эндпоинты: <code className="rounded bg-background/60 px-1">POST /process/json</code> и{" "}
          <code className="rounded bg-background/60 px-1">POST /process/csv</code>. Оба получают один и тот же JSON-пейлоад.
        </div>
      </div>
  );
};

export default Experiments;
