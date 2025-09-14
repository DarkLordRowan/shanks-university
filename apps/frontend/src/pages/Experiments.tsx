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
    const headers = ["id","series_name","series_x","series_lim","accel_name","m","n","series_value","partial_sum","partial_sum_deviation","accel_value","accel_value_deviation","error"];
    const rows: (string | number)[][] = [];
    for (const block of jsonResult) {
      const s = block?.series ?? {}; const a = block?.accel ?? {};
      const blkId = typeof block?.id === "string" ? block.id : "";
      const err = block?.error?.description ? String(block.error.description) : "";
      const seriesName = s?.name ?? "";
      const seriesX = s?.arguments && typeof s.arguments === "object" && "x" in s.arguments ? (s.arguments.x as any) : "";
      const seriesLim = typeof s?.lim === "number" ? s.lim : "";
      const accelName = a?.name ?? ""; const m = a?.m_value ?? a?.m ?? "";
      const items: any[] = Array.isArray(block?.computed) ? block.computed : [];
      for (const it of items) {
        rows.push([blkId, seriesName, seriesX, seriesLim, accelName, m, it?.n ?? "", it?.series_value ?? "", it?.partial_sum ?? "",
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



// === charts utils ===
  type XY = { x: number; y: number };

  const toNum = (v: any): number | null => {
    if (typeof v === "number" && Number.isFinite(v)) return v;
    if (v == null) return null;
    const n = Number(String(v).replace(",", "."));
    return Number.isFinite(n) ? n : null;
  };

  const niceTicks = (min: number, max: number, count = 5) => {
    if (min === max) return [min];
    const span = max - min;
    const step = Math.pow(10, Math.floor(Math.log10(span / count)));
    const err = span / (count * step);
    const mult = err >= 7.5 ? 10 : err >= 3 ? 5 : err >= 1.5 ? 2 : 1;
    const niceStep = step * mult;
    const t0 = Math.ceil(min / niceStep) * niceStep;
    const ticks: number[] = [];
    for (let t = t0; t <= max + 1e-12; t += niceStep) ticks.push(+t.toFixed(12));
    return ticks;
  };

  const niceLogTicks = (minPos: number, maxPos: number) => {
    const a = Math.floor(Math.log10(minPos));
    const b = Math.ceil(Math.log10(maxPos));
    const ticks: number[] = [];
    for (let p = a; p <= b; p++) ticks.push(Math.pow(10, p));
    return ticks;
  };

  const linePath = (pts: XY[], xScale: (n: number) => number, yScale: (n: number) => number) =>
      pts.map((p, i) => `${i ? "L" : "M"}${xScale(p.x)},${yScale(p.y)}`).join(" ");

  const fmt = (v: number) => (v >= 1e4 || v < 1e-3 ? v.toExponential(2) : v.toFixed(2));

// === небольшой универсальный SVG-чарт ===
  const SimpleLineChart: React.FC<{
    width?: number;
    height?: number;
    title: string;
    xLabel?: string;
    yLabel?: string;
    series: { label: string; data: XY[]; color: string; dashed?: boolean }[];
    horizontalLines?: { y: number; label?: string; color?: string; dashed?: boolean }[];
    logY?: boolean;
    annotateMaxText?: string | ((v: number) => string);
  }> = ({
          width = 420,
          height = 220,
          title,
          xLabel,
          yLabel,
          series,
          horizontalLines = [],
          logY = false,
          annotateMaxText,
        }) => {
    const PADL = 40, PADR = 8, PADT = 24, PADB = 30;
    const W = width, H = height;
    const all = series.flatMap(s => s.data);
    if (!all.length) return null;

    const minX = Math.min(...all.map(p => p.x));
    const maxX = Math.max(...all.map(p => p.x));
    const xScale = (x: number) => {
      if (maxX === minX) return PADL + (W - PADL - PADR) / 2;
      return PADL + ((x - minX) / (maxX - minX)) * (W - PADL - PADR);
    };

    const allY = series.flatMap(s => s.data.map(d => d.y)).concat(horizontalLines.map(l => l.y));
    let minY = Math.min(...allY), maxY = Math.max(...allY);
    if (!logY) {
      const pad = (maxY - minY) * 0.05 || 1;
      minY -= pad; maxY += pad;
    } else {
      // лог шкала => только положительные
      const pos = allY.filter(v => v > 0);
      const lo = Math.min(...pos), hi = Math.max(...pos);
      minY = lo; maxY = hi;
    }

    const yLinear = (y: number) => {
      if (maxY === minY) return H - PADB - (H - PADT - PADB)/2;
      return H - PADB - ((y - minY) / (maxY - minY)) * (H - PADT - PADB);
    };
    const yLog = (y: number) => {
      const ly = Math.log10(y), lmin = Math.log10(minY), lmax = Math.log10(maxY);
      if (lmax === lmin) return H - PADB - (H - PADT - PADB)/2;
      return H - PADB - ((ly - lmin) / (lmax - lmin)) * (H - PADT - PADB);
    };
    const yScale = logY ? yLog : yLinear;

    const xticks = niceTicks(minX, maxX, 6);
    const yticks = logY ? niceLogTicks(minY, maxY) : niceTicks(minY, maxY, 5);

    // легенда
    const Legend: React.FC = () => (
        <div className="mt-1 flex flex-wrap items-center gap-3 text-[11px] text-textDim">
          {series.map(s => (
              <span key={s.label} className="inline-flex items-center gap-1">
          <span style={{background:s.color}} className="inline-block h-2 w-3 rounded-sm" />
                {s.label}
        </span>
          ))}
        </div>
    );

    // аннотация максимума (для коэффициента)
    let maxPoint: XY | null = null;
    if (annotateMaxText) {
      const allPts = series.flatMap(s => s.data);
      if (allPts.length) {
        maxPoint = allPts.reduce((a,b) => (a.y > b.y ? a : b));
      }
    }

    return (
        <div className="rounded-lg border border-border/60 bg-surface p-2">
          <div className="mb-1 text-sm">{title}</div>
          <div className="relative w-full" style={{ aspectRatio: `${W}/${H}` }}>
            <svg
                viewBox={`0 0 ${W} ${H}`}
                preserveAspectRatio="xMidYMid meet"
                className="absolute inset-0 h-full w-full block"
            >
            {/* оси */}
            <path d={`M${PADL},${H-PADB} L${W-PADR},${H-PADB}`} stroke="currentColor" strokeOpacity="0.4" />
            <path d={`M${PADL},${PADT} L${PADL},${H-PADB}`} stroke="currentColor" strokeOpacity="0.4" />
            {/* сетка */}
            {xticks.map((t, i) => (
                <g key={`x${i}`}>
                  <line x1={xScale(t)} y1={PADT} x2={xScale(t)} y2={H-PADB} stroke="currentColor" strokeOpacity="0.08" />
                  <text x={xScale(t)} y={H-PADB+14} textAnchor="middle" className="fill-current text-[10px]">{t}</text>
                </g>
            ))}
            {yticks.map((t, i) => (
                <g key={`y${i}`}>
                  <line x1={PADL} y1={yScale(t)} x2={W-PADR} y2={yScale(t)} stroke="currentColor" strokeOpacity="0.08" />
                  <text x={PADL-6} y={yScale(t)+3} textAnchor="end" className="fill-current text-[10px]">
                    {logY ? `1e${Math.log10(t).toFixed(0)}` : t}
                  </text>
                </g>
            ))}

            {/* горизонтальные линии (предел) */}
            {horizontalLines.map((l, i) => (
                <g key={`h${i}`}>
                  <line x1={PADL} x2={W-PADR} y1={yScale(l.y)} y2={yScale(l.y)}
                        stroke={l.color ?? "#ef4444"} strokeDasharray={l.dashed ? "6 4" : undefined} strokeWidth={1.2}/>
                  {l.label && (
                      <text x={W-PADR-4} y={yScale(l.y)-4} textAnchor="end" className="fill-current text-[10px] opacity-80">
                        {l.label}
                      </text>
                  )}
                </g>
            ))}

            {/* линии серий */}
            {series.map((s, idx) => (
                <g key={idx}>
                  <path d={linePath(s.data, xScale, yScale)} fill="none"
                        stroke={s.color} strokeWidth={1.8} strokeDasharray={s.dashed ? "6 4" : undefined}/>
                  {s.data.map((p, i) => (
                      <circle key={i} cx={xScale(p.x)} cy={yScale(p.y)} r={2.2} fill={s.color}/>
                  ))}
                </g>
            ))}

            {/* подписи осей */}
            {xLabel && <text x={(PADL+W-PADR)/2} y={H-6} textAnchor="middle" className="fill-current text-[10px]">{xLabel}</text>}
            {yLabel && <text x={12} y={(PADT+H-PADB)/2} transform={`rotate(-90 12 ${(PADT+H-PADB)/2})`} textAnchor="middle" className="fill-current text-[10px]">{yLabel}</text>}

            {/* аннотация максимума */}
              {annotateMaxText && maxPoint && (() => {
                const txt = typeof annotateMaxText === "function" ? annotateMaxText(maxPoint.y) : annotateMaxText;
                const bx = Math.min(W - PADR - 76, Math.max(PADL, xScale(maxPoint.x) + 6));
                const by = Math.max(PADT + 14, yScale(maxPoint.y) - 6);
                return (
                    <>
                      <rect x={bx} y={by - 12} width="76" height="16" rx="3" className="fill-current" opacity="0.12" />
                      <text x={bx + 4} y={by - 0.5} className="fill-current text-[10px]">{txt}</text>
                    </>
                );
              })()}
            </svg>
          </div>
          <Legend/>
        </div>
    );
  };

// === подготовка данных для трёх панелей ===
  type ChartGroup = {
    id: string;
    caption: string;
    limit?: number | null;
    partial: XY[];
    accel: XY[];
    errPartial: XY[];  // |S_n - L|
    errAccel: XY[];    // |A_n - L|
    ratio: XY[];       // |S_n - L| / |A_n - L|
  };

  const chartGroups = useMemo<ChartGroup[]>(() => {
    if (!Array.isArray(jsonResult)) return [];
    const byKey: Record<string, ChartGroup> = {};

    for (const block of jsonResult) {
      const s = block?.series ?? {};
      const a = block?.accel ?? {};
      const seriesName = s?.name ?? "";
      const xVal = (s?.arguments && typeof s.arguments === "object" && "x" in s.arguments) ? s.arguments.x : "";
      const lim = toNum(s?.lim);
      const accelName = a?.name ?? "";
      const m = a?.m_value ?? a?.m ?? "";
      const blkId = typeof block?.id === "string" ? block.id : "";

      const key = `${seriesName}|x=${xVal}|${accelName}|m=${m}`;
      if (!byKey[key]) {
        byKey[key] = {
          id: key,
          caption: `${seriesName} (x=${xVal}${lim!=null?`, lim=${lim}`:""}) · ${accelName}${m!==""?` (m=${m})`:""}${blkId ? ` · id=${blkId}` : ""}`,
          limit: lim,
          partial: [],
          accel: [],
          errPartial: [],
          errAccel: [],
          ratio: [],
        };
      }
      const items: any[] = Array.isArray(block?.computed) ? block.computed : [];
      for (const it of items) {
        const n = toNum(it?.n);
        if (n == null) continue;

        const ps = toNum(it?.partial_sum);
        if (ps != null) byKey[key].partial.push({ x: n, y: ps });

        const av = toNum(it?.accel_value);
        if (av != null) byKey[key].accel.push({ x: n, y: av });

        if (lim != null && ps != null) byKey[key].errPartial.push({ x: n, y: Math.abs(ps - lim) });
        if (lim != null && av != null) byKey[key].errAccel.push({ x: n, y: Math.abs(av - lim) });

        if (lim != null && ps != null && av != null) {
          const eP = Math.abs(ps - lim), eA = Math.abs(av - lim);
          if (eA > 0) byKey[key].ratio.push({ x: n, y: eP / eA });
        }
      }
    }

    const groups = Object.values(byKey);
    for (const g of groups) {
      g.partial.sort((a,b)=>a.x-b.x);
      g.accel.sort((a,b)=>a.x-b.x);
      g.errPartial.sort((a,b)=>a.x-b.x);
      g.errAccel.sort((a,b)=>a.x-b.x);
      g.ratio.sort((a,b)=>a.x-b.x);
    }
    return groups;
  }, [jsonResult]);


  return (
      <div className="mx-auto max-w-6xl px-4 py-6">
        <h1 className="mb-4 text-2xl font-bold">Эксперименты</h1>
        <p className="mb-6 text-textDim">Загрузите JSON с параметрами рядов и алгоритмов. Мы отправим его во внешнее API и получим результаты в форматах JSON и CSV.</p>

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

                      {/* >>> НОВОЕ: графики по сериям (после таблицы) <<< */}
                      {/* === ГРАФИКИ ДЛЯ КАЖДОГО РЯДА (после таблицы) === */}
                      {chartGroups.length > 0 && (
                          <div className="mt-6 space-y-4">
                            <h3 className="text-lg font-semibold">Графики по сериям</h3>
                            <div className="space-y-4">
                              {chartGroups.map((g) => (
                                  <div key={g.id} className="rounded-xl border border-border/60 bg-panel/60 p-3">
                                    <div className="mb-2 text-sm text-textDim">{g.caption}</div>
                                    <div className="grid gap-3 md:grid-cols-2 xl:grid-cols-3">
                                      {/* 1) Значения методов + предел */}
                                      <SimpleLineChart
                                          title="Сравнение значений методов"
                                          xLabel="Порядок (n)"
                                          series={[
                                            { label: "Частичные суммы", data: g.partial, color: "#3b82f6" },
                                            { label: "Ускоренный метод", data: g.accel, color: "#f59e0b" },
                                          ]}
                                          horizontalLines={g.limit != null ? [{ y: g.limit, label: `Предел: ${g.limit}`, color:"#ef4444", dashed:true }] : []}
                                      />

                                      {/* 2) Абсолютное отклонение (лог-шкала) */}
                                      <SimpleLineChart
                                          title="Абсолютное отклонение (log scale)"
                                          xLabel="Порядок (n)"
                                          yLabel="|значение − предел|"
                                          logY
                                          series={[
                                            { label: "Частичные суммы", data: g.errPartial, color: "#3b82f6" },
                                            { label: "Ускоренный метод", data: g.errAccel, color: "#f59e0b" },
                                          ]}
                                      />

                                      {/* 3) Коэффициент ускорения: |Sₙ−L| / |Aₙ−L| */}
                                      <SimpleLineChart
                                          title="Коэффициент ускорения (Частичная / Ускоренная)"
                                          xLabel="Порядок (n)"
                                          series={[
                                            { label: "Коэффициент ускорения", data: g.ratio, color: "#10b981" },
                                          ]}
                                          logY
                                          annotateMaxText={(v) => `Max: ${fmt(v)}x`}
                                      />

                                    </div>
                                  </div>
                              ))}
                            </div>
                          </div>
                      )}

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
