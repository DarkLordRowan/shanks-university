import React, { useMemo, useRef, useState } from "react";
import { parseCsv } from "../utils/csv";
import { DataTable } from "../components/ui/DataTable";

const API_BASE = "https://shanks-worker.mirea-kmbo.ru/api";

type ApiJsonResult = any;

const Experiments: React.FC = () => {
  const [rawText, setRawText] = useState("");
  const [fileName, setFileName] = useState("");
  const [jsonPayload, setJsonPayload] = useState<any | null>(null);

  const [isSending, setIsSending] = useState(false);
  const [jsonResult, setJsonResult] = useState<ApiJsonResult | null>(null);
  const [jsonError, setJsonError] = useState<string | null>(null);
  const [csvBlobUrl, setCsvBlobUrl] = useState<string | null>(null);
  const [csvText, setCsvText] = useState<string | null>(null);
  const [csvError, setCsvError] = useState<string | null>(null);
  const [generalError, setGeneralError] = useState<string | null>(null);
  const [activeTab, setActiveTab] = useState<"jsonTable" | "csvTable" | "rawJson" | "rawCsv">("jsonTable");

  const fileInputRef = useRef<HTMLInputElement | null>(null);

  const prettyPayload = useMemo(() => (jsonPayload ? JSON.stringify(jsonPayload, null, 2) : ""), [jsonPayload]);

  const normalizeNetworkError = (e: unknown) =>
      e instanceof TypeError && e.message === "Failed to fetch"
          ? "Не удалось выполнить запрос (возможна CORS/сетевая ошибка). Проверьте доступность API и CORS."
          : e instanceof Error && e.message
              ? e.message
              : "Неизвестная ошибка сети.";

  const handleFile = async (file: File) => {
    try {
      const text = await file.text();
      setRawText(text);
      setFileName(file.name);
      setJsonPayload(JSON.parse(text));
      setGeneralError(null); setJsonError(null); setCsvError(null);
    } catch {
      setGeneralError("Не удалось прочитать/распарсить JSON-файл.");
      setJsonPayload(null);
    }
  };

  const onFileChange: React.ChangeEventHandler<HTMLInputElement> = async (e) => {
    const f = e.target.files?.[0]; if (f) await handleFile(f);
  };
  const onDrop: React.DragEventHandler<HTMLDivElement> = async (e) => {
    e.preventDefault(); if (e.dataTransfer.files?.length) await handleFile(e.dataTransfer.files[0]);
  };

  const onPasteClick = () => {
    try {
      setJsonPayload(JSON.parse(rawText));
      setGeneralError(null); setJsonError(null); setCsvError(null);
      setFileName(""); if (fileInputRef.current) fileInputRef.current.value = "";
    } catch { setGeneralError("Вставленный текст не является корректным JSON."); setJsonPayload(null); }
  };

  const reset = () => {
    setRawText(""); setFileName(""); setJsonPayload(null);
    setIsSending(false); setJsonResult(null); setJsonError(null);
    setCsvBlobUrl(null); setCsvText(null); setCsvError(null); setGeneralError(null);
    if (fileInputRef.current) fileInputRef.current.value = "";
  };

  const sendForJsonAndCsv = async () => {
    if (!jsonPayload) { setGeneralError("Сначала загрузите или вставьте корректный JSON."); return; }
    setIsSending(true); setGeneralError(null);
    setJsonResult(null); setJsonError(null); setCsvBlobUrl(null); setCsvText(null); setCsvError(null);

    const jsonReq = fetch(`${API_BASE}/process/json`, {
      method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(jsonPayload),
    })
        .then(async (r) => { if (!r.ok) throw new Error(`Ошибка /process/json (${r.status}): ${await r.text()}`); return r.json(); })
        .then((data) => setJsonResult(data))
        .catch((e) => setJsonError(normalizeNetworkError(e)));

    const csvReq = fetch(`${API_BASE}/process/csv`, {
      method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(jsonPayload),
    })
        .then(async (r) => {
          if (!r.ok) throw new Error(`Ошибка /process/csv (${r.status}): ${await r.text()}`);
          const blob = await r.blob(); setCsvBlobUrl(URL.createObjectURL(blob)); setCsvText(await blob.text());
        })
        .catch((e) => setCsvError(normalizeNetworkError(e)));

    await Promise.allSettled([jsonReq, csvReq]);
    setIsSending(false);
    if (jsonResult) setActiveTab("jsonTable");
  };

  const jsonTable = useMemo(() => {
    if (!Array.isArray(jsonResult)) return { headers: [], rows: [] as (string | number)[][] };
    const headers = ["series_name","series_x","series_lim","accel_name","m","n","series_value","partial_sum","partial_sum_dev","accel_value","accel_value_dev","error"];
    const rows: (string | number)[][] = [];
    for (const block of jsonResult) {
      const s = block?.series ?? {}; const a = block?.accel ?? {};
      const err = block?.error?.description ? String(block.error.description) : "";
      const seriesName = s?.name ?? "";
      const seriesX = s?.arguments && typeof s.arguments === "object" && "x" in s.arguments ? (s.arguments.x as any) : "";
      const seriesLim = typeof s?.lim === "number" ? s.lim : "";
      const accelName = a?.name ?? ""; const m = a?.m_value ?? a?.m ?? "";
      const items: any[] = Array.isArray(block?.computed) ? block.computed : [];
      for (const it of items) {
        rows.push([seriesName, seriesX, seriesLim, accelName, m, it?.n ?? "", it?.series_value ?? "", it?.partial_sum ?? "",
          it?.partial_sum_deviation ?? "", it?.accel_value ?? "", it?.accel_value_deviation ?? "", err]);
      }
    }
    return { headers, rows };
  }, [jsonResult]);

  const csvTable = useMemo(() => (csvText ? parseCsv(csvText) : { headers: [], rows: [] as string[][] }), [csvText]);



  type ErrorFilterMode = "all" | "onlyGood" | "onlyBad";
  const [errorFilterMode, setErrorFilterMode] = useState<ErrorFilterMode>("all");

  const applyErrorFilter = (table: { headers: string[]; rows: any[][] }) => {
    const i = table.headers.indexOf("error_description");
    const j = i === -1 ? table.headers.indexOf("error") : i;
    if (j === -1 || errorFilterMode === "all") return table;

    const hasErr = (r: any[]) => String(r[j] ?? "").trim().length > 0;
    const rows =
        errorFilterMode === "onlyBad"
            ? table.rows.filter(hasErr)
            : table.rows.filter(r => !hasErr(r));

    return { headers: table.headers, rows };
  };

  return (
      <div className="mx-auto max-w-6xl px-4 py-6">
        <h1 className="mb-4 text-2xl font-bold">Эксперименты</h1>
        <p className="mb-6 text-textDim">Загрузите JSON с параметрами серий и методов. Мы отправим его во внешнее API и получим результаты в форматах JSON и CSV.</p>

        {/* Входные данные */}
        <div className="mb-6 rounded-2xl border border-border/60 bg-panel/70 p-4">
          <h2 className="mb-3 text-lg font-semibold">Входные данные</h2>

          <div
              onDrop={onDrop} onDragOver={(e) => e.preventDefault()}
              className="mb-3 flex cursor-pointer items-center justify-center rounded-xl border border-dashed border-border/70 p-6 text-center text-sm text-textDim hover:bg-panel/60"
              onClick={() => fileInputRef.current?.click()} title="Перетащите файл JSON сюда или нажмите для выбора"
          >
            {fileName ? (
                <div>
                  <div className="font-medium text-white">Файл: {fileName}</div>
                  <div className="text-xs text-textDim">Нажмите, чтобы выбрать другой файл</div>
                </div>
            ) : (
                <div>
                  <div className="font-medium text-white">Перетащите JSON-файл сюда</div>
                  <div className="text-xs text-textDim">или нажмите для выбора файла</div>
                </div>
            )}
          </div>
          <input ref={fileInputRef} type="file" accept="application/json,.json" onChange={onFileChange} className="hidden" />

          <label className="mb-1 block text-sm font-medium">Или вставьте JSON вручную</label>
          <textarea
              className="h-48 w-full resize-y rounded-xl border border-border/60 bg-panel p-2 font-mono text-sm text-white"
              placeholder='{
  "series": [ ... ],
  "methods": [ ... ]
}'
              value={rawText} onChange={(e) => setRawText(e.target.value)}
          />

          <div className="mt-3 flex gap-3">
            <button className="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black hover:opacity-90" onClick={onPasteClick}>
              Использовать вставленный JSON
            </button>
            <button className="rounded-xl border border-border px-4 py-2 text-sm" onClick={reset}>Сбросить</button>
            <button
                className="ml-auto rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black hover:opacity-90 disabled:opacity-60"
                onClick={sendForJsonAndCsv} disabled={isSending || !jsonPayload}
            >
              {isSending ? "Отправка… (JSON)" : "Отправить (JSON)"}
            </button>
          </div>

          {prettyPayload && (
              <div className="mt-4">
                <h3 className="mb-1 text-sm font-semibold text-secondary">Проверка распознанного JSON</h3>
                <pre className="max-h-64 overflow-auto rounded-xl border border-border/60 bg-panel p-3 text-xs leading-relaxed text-white whitespace-pre-wrap">
{prettyPayload}
            </pre>
              </div>
          )}
        </div>

        {/* Результаты */}
        <div className="rounded-2xl border border-border/60 bg-panel/70 p-4">
          <h2 className="mb-3 text-lg font-semibold">Результаты</h2>

          {generalError && <div className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{generalError}</div>}

          <div className="mb-3 flex flex-wrap gap-2">
            {[
              { id: "jsonTable", label: "Таблица JSON" },
              { id: "csvTable", label: "Таблица CSV" },
              { id: "rawJson", label: "Raw JSON" },
              { id: "rawCsv", label: "Raw CSV" },
            ].map((t) => (
                <button
                    key={t.id}
                    onClick={() => setActiveTab(t.id as any)}
                    className={`rounded-lg px-3 py-1 text-sm ${activeTab === (t.id as any) ? "bg-primary text-black" : "border border-border hover:bg-panel/60"}`}
                >
                  {t.label}
                </button>
            ))}

            <div className="ml-auto flex gap-2">
              {jsonResult && (
                  <a
                      className="rounded-lg border border-border px-3 py-1 text-sm hover:bg-panel/60"
                      href={URL.createObjectURL(new Blob([JSON.stringify(jsonResult, null, 2)], { type: "application/json" }))}
                      download="results.json"
                  >
                    Скачать JSON
                  </a>
              )}
              {csvBlobUrl && (
                  <a className="rounded-lg border border-border px-3 py-1 text-sm hover:bg-panel/60" href={csvBlobUrl} download="results.csv">
                    Скачать CSV
                  </a>
              )}
            </div>
          </div>

          {activeTab === "jsonTable" && (
              <>
                {jsonError && (
                    <div className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{jsonError}</div>
                )}
                {jsonTable.headers.length ? (
                    <>
                      <div className="mb-2 flex items-center gap-2 text-sm">
                        <span className="text-textDim">Ошибки:</span>
                        <select
                            className="rounded-md border border-border bg-background/60 px-2 py-1 text-sm"
                            value={errorFilterMode}
                            onChange={(e) => setErrorFilterMode(e.target.value as ErrorFilterMode)}
                        >
                          <option value="all">показывать все</option>
                          <option value="onlyGood">только без ошибок</option>
                          <option value="onlyBad">только с ошибками</option>
                        </select>
                      </div>
                      {(() => {
                        const t = applyErrorFilter(jsonTable);
                        return (
                            <DataTable
                                headers={t.headers}
                                rows={t.rows}
                                enableSorting
                                enableColumnFilters
                                storageKey="exp_json_table"
                            />
                        );
                      })()}
                    </>
                ) : (
                    <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных. Отправьте запрос.</div>
                )}
              </>
          )}

          {activeTab === "csvTable" && (
              <>
                {csvError && (
                    <div className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{csvError}</div>
                )}
                {csvTable.headers.length ? (
                    <>
                      <div className="mb-2 flex items-center gap-2 text-sm">
                        <span className="text-textDim">Ошибки:</span>
                        <select
                            className="rounded-md border border-border bg-background/60 px-2 py-1 text-sm"
                            value={errorFilterMode}
                            onChange={(e) => setErrorFilterMode(e.target.value as ErrorFilterMode)}
                        >
                          <option value="all">показывать все</option>
                          <option value="onlyGood">только без ошибок</option>
                          <option value="onlyBad">только с ошибками</option>
                        </select>
                      </div>
                      {(() => {
                        const t = applyErrorFilter(csvTable);
                        return (
                            <DataTable
                                headers={t.headers}
                                rows={t.rows}
                                compact
                                enableSorting
                                enableColumnFilters
                                storageKey="exp_csv_table"
                            />
                        );
                      })()}
                    </>
                ) : (
                    <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных. Отправьте запрос.</div>
                )}
              </>
          )}

          {activeTab === "rawJson" && (
              <>
                {jsonError && <div className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{jsonError}</div>}
                {jsonResult ? (
                    <pre className="max-h-[28rem] overflow-auto rounded-xl border border-border/60 bg-surface p-3 text-xs leading-relaxed text-white whitespace-pre">
{JSON.stringify(jsonResult, null, 2)}
              </pre>
                ) : (
                    <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных. Отправьте запрос.</div>
                )}
              </>
          )}

          {activeTab === "rawCsv" && (
              <>
                {csvError && <div className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{csvError}</div>}
                {csvText ? (
                    <pre className="max-h-[28rem] overflow-auto rounded-xl border border-border/60 bg-surface p-3 text-xs leading-relaxed text-white whitespace-pre">
{csvText}
              </pre>
                ) : (
                    <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных. Отправьте запрос.</div>
                )}
              </>
          )}
        </div>
      </div>
  );
};

export default Experiments;
