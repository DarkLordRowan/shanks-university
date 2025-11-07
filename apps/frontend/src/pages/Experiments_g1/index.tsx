import React, { useState } from "react";
import { SERIES, type SeriesNode } from "../../data/series";
import { API_BASE } from "../Experiments/constants.ts";
import { SubmitAndTrackJob } from "../../components/SubmitAndTrackJob.tsx";
import type { ApiJsonResult } from "../Experiments/types.ts";
import { CopyToClipboard } from "../../components/CopyToClipboard.tsx";
import { Check } from "lucide-react";
import type { AlgorithmNode } from "../../data/algorithms.ts";
import { GenerateExpFromDataButton } from "../../components/GenerateExpFromDataButton.tsx";
import { SelectM } from "../../components/SelectM.tsx";
import { SelectAlgorithm } from "../../components/SelectAlgorithm.tsx";
import { SelectSerieses } from "../../components/SelectSerieses.tsx";
import { StepsToToleranceBar } from "../../charts/StepsToToleranceBar.tsx";
import { normalizeFromJson } from "../../utils/responseToItem.ts";
import { PartialSumByN_VarySeries } from "../../charts/vary_series/PartialSumByN_VarySeries.tsx";
import { LogPsDevByN_VarySeries } from "../../charts/vary_series/LogPsDevByN_VarySeries.tsx";
import { LogAccelDevByN_VarySeries } from "../../charts/vary_series/LogAccelDevByN_VarySeries.tsx";
import { SeriesTermByN_VarySeries } from "../../charts/vary_series/SeriesTermByN_VarySeries.tsx";
import {
    DeltaToLimitPartialSumByN_VarySeries
} from "../../charts/vary_series/DeltaToLimitPartialSumByN_VarySeries.tsx";
import { AccelValueByN_VarySeries } from "../../charts/vary_series/AccelValueByN_VarySeries.tsx";

const Experiments_v3: React.FC = () => {

    const [serieses, setSerieses] = useState<SeriesNode[]>([]);
    const [m, setM] = useState<number | null>(null);
    const [algorithm, setAlgorithm] = useState<AlgorithmNode | null>(null);

    const [requestJson, setRequestJson] = useState<string | null>(null);
    const [responseJson, setResponseJson] = useState<ApiJsonResult | null>(null);
    const [requestUUID, setRequestUUID] = useState<string | null>(null);
    const [errorMsg, setErrorMsg] = useState<string | null>(null);

    return (
        <div className="mx-auto max-w-6xl px-4 py-6">

            <div>
                1 алгоритм, 1 m. Разные ряды
            </div>
            <br/>

            <SelectSerieses value={serieses} onChange={setSerieses} items={SERIES} />
            <SelectM value={m} onChange={setM}/>
            <SelectAlgorithm value={algorithm} onChange={setAlgorithm} />

            <div className="mt-3 flex gap-3">
                <GenerateExpFromDataButton
                    mode="vary-series"
                    series={serieses}
                    algorithm={algorithm}
                    m={m}
                    onSuccess={setRequestJson}
                />

                <SubmitAndTrackJob
                    requestJson={requestJson}
                    apiBase={API_BASE}
                    onUuid={(uuid) => setRequestUUID(uuid)}
                    onResponse={(resp) => setResponseJson(resp)}
                    onError={(m) => setErrorMsg(m)}
                />

                {requestUUID && <UuidBadge uuid={requestUUID}/>}

                <ResponseChip json={responseJson}/>

                {errorMsg && (
                    <div className="mt-3 rounded-xl border border-red-400/60 bg-red-500/10 p-3 text-sm text-red-300">
                        {errorMsg}
                    </div>
                )}
            </div>



            <div>
                {responseJson && (
                    <div className="mt-4 w-full space-y-10">
                        {/* 0. Число шагов до точности ε */}
                        <StepsToToleranceBar
                            items={normalizeFromJson(responseJson)}
                            eps={1e-4}
                        />

                        {/* 1. Частичные суммы Sₙ */}
                        <PartialSumByN_VarySeries items={normalizeFromJson(responseJson)} />

                        {/* 2. Ошибка частичных сумм |Sₙ − L| */}
                        <DeltaToLimitPartialSumByN_VarySeries items={normalizeFromJson(responseJson)} />

                        {/* 3. log10(|Sₙ − L|) */}
                        <LogPsDevByN_VarySeries items={normalizeFromJson(responseJson)} />

                        {/* 4. Ускоренные значения Aₙ */}
                        <AccelValueByN_VarySeries items={normalizeFromJson(responseJson)} />

                        {/* 5. log10(|Aₙ − L|) */}
                        <LogAccelDevByN_VarySeries items={normalizeFromJson(responseJson)} />

                        {/* 6. Значения членов ряда aₙ */}
                        <SeriesTermByN_VarySeries items={normalizeFromJson(responseJson)} />


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
        if (!data) {
            setHref(null);
            return;
        }
        const blob = new Blob([JSON.stringify(data, null, 2)], {type: "application/json"});
        const url = URL.createObjectURL(blob);
        setHref(url);
        return () => URL.revokeObjectURL(url);
    }, [data]);
    return href;
}

type UuidBadgeProps = { uuid: string };
const UuidBadge: React.FC<UuidBadgeProps> = React.memo(({uuid}) => (
    <div className="flex items-center gap-2 rounded-xl border border-border/60 bg-panel/70 px-3 py-2 text-sm">
        <span className="text-textDim">UUID:</span>
        <span className="font-mono text-white">{uuid}</span>
        <CopyToClipboard value={uuid}/>
    </div>
));

type ResponseChipProps = { json: ApiJsonResult | null };
const ResponseChip: React.FC<ResponseChipProps> = React.memo(({json}) => {
    const href = useDownloadUrl(json);
    if (!json) return null;
    return (
        <div
            className="flex items-center gap-3 rounded-xl border border-green-500/40 bg-green-500/10 px-3 py-2 text-sm text-green-300">
            <Check size={16} className="text-green-400"/>
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
