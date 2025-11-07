import React, { useMemo, useState } from "react";
import { SERIES, type SeriesNode } from "../../data/series";
import { SelectSeries } from "../../components/SelectSeries.tsx";
import { API_BASE } from "../Experiments/constants.ts";
import { SubmitAndTrackJob } from "../../components/SubmitAndTrackJob.tsx";
import type { ApiJsonResult, ErrorFilterMode } from "../Experiments/types.ts";
import { CopyToClipboard } from "../../components/CopyToClipboard.tsx";
import { Check } from "lucide-react";
import { normalizeFromJson } from "../../utils/responseToItem.ts";
import { AccelerationGainChartByN } from "../../charts/AccelerationGainChartByN.tsx";
import { DeltaToLimitPartialSumChart } from "../../charts/DeltaToLimitPartialSumChart.tsx";
import { LogPsDevChartByN } from "../../charts/LogPsDevChartByN.tsx";
import { PartialSumChartByN } from "../../charts/PartialSumChartByN.tsx";
import { AccelValueChartByN } from "../../charts/AccelValueChartByN.tsx";
import { LogAccelDevChartByN } from "../../charts/LogAccelDevChartByN.tsx";
import { EOCPartialSumChartByN } from "../../charts/EOCPartialSumChartByN.tsx";
import { SeriesTermChartByN } from "../../charts/SeriesTermChartByN.tsx";
import { EOCAccelChartByN } from "../../charts/EOCAccelChartByN.tsx";
import { SelectX } from "../../components/SelectX.tsx";
import { type AlgorithmNode, ALGORITHMS } from "../../data/algorithms.ts";
import { GenerateExpFromDataButton } from "../../components/GenerateExpFromDataButton.tsx";
import { SelectAlgorithms } from "../../components/SelectAlgorithms.tsx";
import { SelectMs } from "../../components/SelectMs.tsx";
import { useItemsSelection } from "../../analysis/useItemsSelection.ts";
import { ItemsSelectionMatrix } from "../../analysis/ItemsSelectionMatrix.tsx";
import { StepsToToleranceExplorer } from "../../charts/StepsToToleranceExplorer.tsx";
import { buildJsonTable, filterByError } from "../Experiments/utils/tables.ts";
import { DataTable } from "../../components/ui/DataTable.tsx";

const Experiments_v3: React.FC = () => {

    const [series, setSeries] = useState<SeriesNode | null>(null);
    const [x, setX] = useState<number | null>(null);
    const [ms, setMs] = useState<number[]>([2, 4, 6, 8, 10]);
    const [algorithms, setAlgorithms] = useState<AlgorithmNode[]>([]);

    const [requestJson, setRequestJson] = useState<string | null>(null);
    const [responseJson, setResponseJson] = useState<ApiJsonResult | null>(null);
    const [requestUUID, setRequestUUID] = useState<string | null>(null);
    const [errorMsg, setErrorMsg] = useState<string | null>(null);

    const items = useMemo(
        () => (responseJson ? normalizeFromJson(responseJson) : []),
        [responseJson]
    );

    const sel = useItemsSelection(items);
    const filtered = sel.filteredItems;

    const jsonTable = useMemo(() => buildJsonTable(responseJson), [responseJson]);
    const [errorFilterMode, setErrorFilterMode] = useState<ErrorFilterMode>("onlyBad");

    return (
        <div className="mx-auto max-w-6xl px-4 py-6">

            <div>
                1 ряд, 1 x, 1 m. Разные алгоритмы
            </div>
            <br/>

            <SelectSeries items={SERIES} value={series} onChange={setSeries}/>
            <SelectX series={series} value={x} onChange={setX}/>
            <SelectAlgorithms value={algorithms} onChange={setAlgorithms} options={ALGORITHMS}/>
            <SelectMs value={ms} onChange={setMs}/>


            <div className="mt-3 flex gap-3">
                <GenerateExpFromDataButton
                    mode="vary-algo"
                    series={series}
                    x={x}
                    m={ms}
                    algorithm={algorithms}
                    onSuccess={setRequestJson}
                />

                <SubmitAndTrackJob
                    requestJson={requestJson}
                    apiBase={API_BASE}
                    onUuid={(uuid) => setRequestUUID(uuid)}
                    onResponse={(resp) => setResponseJson(resp)}
                    onError={(m) => setErrorMsg(m)}
                />

                {requestUUID && <UuidBadge uuid={requestUUID} />}

                <ResponseChip json={responseJson} />

                {errorMsg && (
                    <div className="mt-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">
                        {errorMsg}
                    </div>
                )}
            </div>

            <br/>

            <div>
                {responseJson && (
                    <div className="mt-4 w-full space-y-6">

                        {jsonTable.headers.length && (
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
                            </>
                        )}

                        <ItemsSelectionMatrix
                            rows={sel.rows}
                            argCols={sel.argCols}
                            onToggle={sel.toggle}
                            onSetAll={sel.setAll}
                            onSort={sel.cycleSort}
                        />

                        <div className="space-y-10">
                            {/* 0. Число шагов до точности ε */}
                            <StepsToToleranceExplorer items={filtered} defaultEps={1e-4} />

                            {/* 1. Частичные суммы S_n */}
                            <PartialSumChartByN items={filtered} />

                            {/* 2. Ошибка частичных сумм |S_n - L| */}
                            <DeltaToLimitPartialSumChart items={filtered} />

                            {/* 3. log10(|S_n - L|) */}
                            <LogPsDevChartByN items={filtered} />

                            {/* 4. Ускоренные значения A_n */}
                            <AccelValueChartByN items={filtered} />

                            {/* 5. log10(|A_n - L|) */}
                            <LogAccelDevChartByN items={filtered} />

                            {/* 6. Коэффициент усиления G(n) = log10(|S_n-L| / |A_n-L|) */}
                            <AccelerationGainChartByN items={filtered} />

                            {/* 7. Порядок сходимости частичных сумм */}
                            <EOCPartialSumChartByN items={filtered} />

                            {/* 8. Порядок сходимости ускоренных */}
                            <EOCAccelChartByN items={filtered} />

                            {/* 9. Значения членов ряда a_n */}
                            <SeriesTermChartByN items={filtered} />
                        </div>
                    </div>
                )}

            </div>

        </div>
    );
};

export default Experiments_v3;


function useDownloadUrl(data: unknown | null) {
    const [href, setHref] = React.useState<string | null>(null);
    React.useEffect(() => {
        if (!data) { setHref(null); return; }
        const blob = new Blob([JSON.stringify(data, null, 2)], { type: "application/json" });
        const url = URL.createObjectURL(blob);
        setHref(url);
        return () => URL.revokeObjectURL(url);
    }, [data]);
    return href;
}

type UuidBadgeProps = { uuid: string };
const UuidBadge: React.FC<UuidBadgeProps> = React.memo(({ uuid }) => (
    <div className="flex items-center gap-2 rounded-xl border border-border/60 bg-panel/70 px-3 py-2 text-sm">
        <span className="text-textDim">UUID:</span>
        <span className="font-mono text-white">{uuid}</span>
        <CopyToClipboard value={uuid} />
    </div>
));

type ResponseChipProps = { json: ApiJsonResult | null };
const ResponseChip: React.FC<ResponseChipProps> = React.memo(({ json }) => {
    const href = useDownloadUrl(json);
    if (!json) return null;
    return (
        <div className="flex items-center gap-3 rounded-xl border border-green-500/40 bg-green-500/10 px-3 py-2 text-sm text-green-300">
            <Check size={16} className="text-green-400" />
            <span className="font-medium">Ответ получен</span>
            {href && (
                <a
                    className="ml-2 rounded-lg border border-green-500/40 bg-green-500/10 px-3 py-1 text-xs font-medium hover:bg-green-500/20"
                    href={href}
                    download="results.json"
                >
                    Скачать JSON
                </a>
            )}
        </div>
    );
});
