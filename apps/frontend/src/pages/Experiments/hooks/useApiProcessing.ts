import { useCallback, useState } from "react";
import { postCsvBlob, postJson } from "../../../services/api";
import { type ApiJsonResult } from "../types";
import { normalizeNetworkError } from "../utils/errors";

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
        setCsvBlobUrl(null);
        setCsvText(null);
        setCsvError(null);
    }, []);

    const sendForJsonAndCsv = useCallback(
        async (payload: any) => {
            setIsSending(true);
            resetResults();

            let nextJson: ApiJsonResult | null = null;

            const jsonReq = postJson<ApiJsonResult>("/process/json", payload)
                .then((data) => {
                    nextJson = data;
                    setJsonResult(data);
                })
                .catch((e) => setJsonError(normalizeNetworkError(e)));

            const csvReq = postCsvBlob("/process/csv", payload)
                .then(async (blob) => {
                    const url = URL.createObjectURL(blob);
                    setCsvBlobUrl(url);
                    const text = await blob.text();
                    setCsvText(text);
                })
                .catch((e) => setCsvError(normalizeNetworkError(e)));

            await Promise.allSettled([jsonReq, csvReq]);
            setIsSending(false);

            return {jsonOk: !!nextJson};
        },
        [resetResults]
    );

    return {
        state: {isSending, jsonResult, jsonError, csvBlobUrl, csvText, csvError},
        actions: {sendForJsonAndCsv, resetResults},
    };
}
