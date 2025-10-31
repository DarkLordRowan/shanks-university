import React, { useState } from "react";
import { SERIES, type SeriesNode } from "../../data/series";
import { SelectSeries } from "../../components/SelectSeries.tsx";
import { GenerateFromDataButton } from "../../components/GenerateFromDataButton.tsx";
import { API_BASE } from "../Experiments/constants.ts";
import { SubmitAndTrackJob } from "../../components/SubmitAndTrackJob.tsx";
import type { ApiJsonResult } from "../Experiments/types.ts";
import { CopyToClipboard } from "../../components/CopyToClipboard.tsx";
import { Check } from "lucide-react";

const Experiments_v3: React.FC = () => {

    const [series, setSeries] = useState<SeriesNode | null>(null);

    const [requestJson, setRequestJson] = useState<string | null>(null);
    const [responseJson, setResponseJson] = useState<ApiJsonResult | null>(null);
    const [requestUUID, setRequestUUID] = useState<string | null>(null);
    const [errorMsg, setErrorMsg] = useState<string | null>(null);

    return (
        <div className="mx-auto max-w-6xl px-4 py-6">
            <SelectSeries items={SERIES} value={series} onChange={setSeries}/>


            <div className="mt-3 flex gap-3">
                <GenerateFromDataButton
                    series={series ? [series] : []}
                    nConfig={{start: 1, stop: 51, step: 1}}
                    m={[10]}
                    disabled={!series}
                    onSuccess={(json) => setRequestJson(json)}
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

            <div>

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
