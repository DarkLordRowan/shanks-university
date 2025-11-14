import { useState, useCallback } from "react";
import type { ValidationIssue } from "@/shared/lib/json/validationTypes";
import { streamParseResponseRecords } from "@/shared/lib/json/streamingParseResponseRecords";

type LoadState =
    | { status: "idle" }
    | { status: "loading"; count: number }
    | { status: "error"; issues: ValidationIssue[] }
    | { status: "success"; count: number };

export function useStreamLoadRecords() {
    const [state, setState] = useState<LoadState>({ status: "idle" });

    const load = useCallback(async (file: File) => {
        setState({ status: "loading", count: 0 });

        let count = 0;
        let hadError = false;

        await streamParseResponseRecords(
            file,
            (record) => {
                if (hadError) return;
                count += 1;
                console.log("parsed record", count, record); // хотя бы первые 1–2 раза посмотреть
                setState({ status: "loading", count });
            },
            (issues) => {
                hadError = true;
                console.error("validation issues", issues);
                setState({ status: "error", issues });
            },
        );

        if (!hadError) {
            setState({ status: "success", count });
        }
    }, []);

    return { state, load };
}
