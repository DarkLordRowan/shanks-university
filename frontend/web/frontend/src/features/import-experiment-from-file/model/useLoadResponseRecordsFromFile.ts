import { useCallback, useState } from "react";
import type { ResponseRecords } from "@/shared/api/experiments/dto";
import { parseResponseRecordsFromFile } from "@/shared/lib/json/parseResponseRecordsFromFile";
import type { ValidationIssue } from "@/shared/lib/json/validationTypes";

type LoadState =
    | { status: "idle" }
    | { status: "loading" }
    | { status: "success"; data: ResponseRecords }
    | { status: "error"; issues: ValidationIssue[] };

export function useLoadResponseRecordsFromFile() {
    const [state, setState] = useState<LoadState>({ status: "idle" });

    const loadFromFile = useCallback(async (file: File) => {
        setState({ status: "loading" });

        const result = await parseResponseRecordsFromFile(file);

        if (result.ok) {
            setState({ status: "success", data: result.data });
            return { ok: true as const, data: result.data };
        }

        setState({ status: "error", issues: result.issues });
        return { ok: false as const, issues: result.issues };
    }, []);

    const reset = useCallback(() => {
        setState({ status: "idle" });
    }, []);

    return {
        state,
        loadFromFile,
        reset,
        // удобные флаги
        isIdle: state.status === "idle",
        isLoading: state.status === "loading",
        isSuccess: state.status === "success",
        isError: state.status === "error",
    };
}
