import React, { useMemo, useState } from "react";
import { useJsonInput } from "./hooks/useJsonInput";
import { useApiProcessing } from "./hooks/useApiProcessing";
import { type ErrorFilterMode } from "./types";
import { buildJsonTable, filterByError } from "./utils/tables";
import { buildChartGroups } from "./utils/chartUtils";
import { parseCsv } from "../../utils/csv";
import { DataTable } from "../../components/ui/DataTable";

const Experiments: React.FC = () => {
    // входные данные
    const {
        state: {rawText, fileName, jsonPayload, prettyPayload},
        actions: {setRawText, onFileChange, onDrop, applyPasted, reset},
        refs: {fileInputRef},
    } = useJsonInput();

    // результаты API
    const {
        state: {isSending, jsonResult, jsonError, csvBlobUrl, csvText, csvError},
        actions: {sendForJsonAndCsv, resetResults},
    } = useApiProcessing();

    const [generalError, setGeneralError] = useState<string | null>(null);
    const [activeTab, setActiveTab] = useState<"jsonTable" | "csvTable" | "rawJson" | "rawCsv">("jsonTable");
    const [errorFilterMode, setErrorFilterMode] = useState<ErrorFilterMode>("all");

    const jsonTable = useMemo(() => buildJsonTable(jsonResult), [jsonResult]);
    const csvTable = useMemo(() => (csvText ? parseCsv(csvText) : {
        headers: [] as string[],
        rows: [] as string[][]
    }), [csvText]);
    const chartGroups = useMemo(() => buildChartGroups(jsonResult), [jsonResult]);

    const onUsePasted = () => {
        applyPasted((msg) => setGeneralError(msg || null));
        resetResults();
    };

    const onResetAll = () => {
        reset();
        setGeneralError(null);
        resetResults();
    };

    const onSend = async () => {
        if (!jsonPayload) {
            setGeneralError("Сначала загрузите или вставьте корректный JSON.");
            return;
        }
        setGeneralError(null);
        const {jsonOk} = await sendForJsonAndCsv(jsonPayload);
        if (jsonOk) setActiveTab("jsonTable");
    };

    return (
        <div className="mx-auto max-w-6xl px-4 py-6">
            <h1 className="mb-4 text-2xl font-bold">Эксперименты</h1>
            <p className="mb-6 text-textDim">
                Загрузите JSON с параметрами рядов и алгоритмов. Мы отправим его во внешнее API и получим результаты в
                форматах JSON и CSV.
            </p>

            {/* Входные данные */}
            <div className="mb-6 rounded-2xl border border-border/60 bg-panel/70 p-4">
                <h2 className="mb-3 text-lg font-semibold">Входные данные</h2>

                <div
                    onDrop={onDrop}
                    onDragOver={(e) => e.preventDefault()}
                    className="mb-3 flex cursor-pointer items-center justify-center rounded-xl border border-dashed border-border/70 p-6 text-center text-sm text-textDim hover:bg-panel/60"
                    onClick={() => fileInputRef.current?.click()}
                    title="Перетащите файл JSON сюда или нажмите для выбора"
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
                <input ref={fileInputRef} type="file" accept="application/json,.json" onChange={onFileChange}
                       className="hidden"/>

                <label className="mb-1 block text-sm font-medium">Или вставьте JSON вручную</label>
                <textarea
                    className="h-48 w-full resize-y rounded-xl border border-border/60 bg-panel p-2 font-mono text-sm text-white"
                    placeholder={`{
  "series": [ ... ],
  "methods": [ ... ]
}`}
                    value={rawText}
                    onChange={(e) => setRawText(e.target.value)}
                />

                <div className="mt-3 flex gap-3">
                    <button
                        className="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black hover:opacity-90"
                        onClick={onUsePasted}
                    >
                        Использовать вставленный JSON
                    </button>
                    <button className="rounded-xl border border-border px-4 py-2 text-sm" onClick={onResetAll}>
                        Сбросить
                    </button>
                    <button
                        className="ml-auto rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black hover:opacity-90 disabled:opacity-60"
                        onClick={onSend}
                        disabled={isSending || !jsonPayload}
                    >
                        {isSending ? "Отправка… (JSON/CSV)" : "Отправить (JSON/CSV)"}
                    </button>
                </div>

                {prettyPayload && (
                    <div className="mt-4">
                        <h3 className="mb-1 text-sm font-semibold text-secondary">Проверка распознанного JSON</h3>
                        <pre
                            className="max-h-64 whitespace-pre-wrap overflow-auto rounded-xl border border-border/60 bg-panel p-3 text-xs leading-relaxed text-white">
                            {prettyPayload}
                        </pre>
                    </div>
                )}
            </div>

            {/* Результаты */}
            <div className="rounded-2xl border border-border/60 bg-panel/70 p-4">
                <h2 className="mb-3 text-lg font-semibold">Результаты</h2>

                {generalError && (
                    <div
                        className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{generalError}</div>
                )}

                <div className="mb-3 flex flex-wrap gap-2">
                    {[
                        {id: "jsonTable", label: "Таблица JSON"},
                        {id: "csvTable", label: "Таблица CSV"},
                        {id: "rawJson", label: "Raw JSON"},
                        {id: "rawCsv", label: "Raw CSV"},
                    ].map((t) => (
                        <button
                            key={t.id}
                            onClick={() => setActiveTab(t.id as any)}
                            className={`rounded-lg px-3 py-1 text-sm ${
                                activeTab === (t.id as any) ? "bg-primary text-black" : "border border-border hover:bg-panel/60"
                            }`}
                        >
                            {t.label}
                        </button>
                    ))}

                    <div className="ml-auto flex gap-2">
                        {jsonResult && (
                            <a
                                className="rounded-lg border border-border px-3 py-1 text-sm hover:bg-panel/60"
                                href={URL.createObjectURL(new Blob([JSON.stringify(jsonResult, null, 2)], {type: "application/json"}))}
                                download="results.json"
                            >
                                Скачать JSON
                            </a>
                        )}
                        {csvBlobUrl && (
                            <a className="rounded-lg border border-border px-3 py-1 text-sm hover:bg-panel/60"
                               href={csvBlobUrl} download="results.csv">
                                Скачать CSV
                            </a>
                        )}
                    </div>
                </div>

                {activeTab === "jsonTable" && (
                    <>
                        {jsonError && (
                            <div
                                className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{jsonError}</div>
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
                                    const t = filterByError(jsonTable, errorFilterMode);
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

                                {/* Графики */}
                                {(() => {
                                    const {ChartsSection} = require("./components/ChartsSection");
                                    return <ChartsSection groups={chartGroups}/>;
                                })()}
                            </>
                        ) : (
                            <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных.
                                Отправьте запрос.</div>
                        )}
                    </>
                )}

                {activeTab === "csvTable" && (
                    <>
                        {csvError && (
                            <div
                                className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{csvError}</div>
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
                                    const t = filterByError(csvTable, errorFilterMode);
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
                            <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных.
                                Отправьте запрос.</div>
                        )}
                    </>
                )}

                {activeTab === "rawJson" && (
                    <>
                        {jsonError && <div
                            className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{jsonError}</div>}
                        {jsonResult ? (
                            <pre
                                className="max-h-[28rem] overflow-auto rounded-xl border border-border/60 bg-surface p-3 text-xs leading-relaxed text-white whitespace-pre">
                                {JSON.stringify(jsonResult, null, 2)}
                            </pre>
                        ) : (
                            <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных.
                                Отправьте запрос.</div>
                        )}
                    </>
                )}

                {activeTab === "rawCsv" && (
                    <>
                        {csvError && <div
                            className="mb-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">{csvError}</div>}
                        {csvText ? (
                            <pre
                                className="max-h-[28rem] overflow-auto rounded-xl border border-border/60 bg-surface p-3 text-xs leading-relaxed text-white whitespace-pre">
                                {csvText}
                            </pre>
                        ) : (
                            <div className="rounded-xl border border-border/60 p-4 text-sm text-textDim">Нет данных.
                                Отправьте запрос.</div>
                        )}
                    </>
                )}
            </div>
        </div>
    );
};

export default Experiments;
