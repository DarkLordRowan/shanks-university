import { useState, useCallback, useRef } from "react";
import type { ValidationIssue } from "@/shared/lib/json/validationTypes";
import type { ResponseRecord } from "@/shared/api/experiments/dto";
import { streamParseResponseRecords } from "@/shared/lib/json/streamingParseResponseRecords";

type LoadState =
    | { status: "idle" }
    | { status: "loading"; count: number }
    | { status: "error"; issues: ValidationIssue[] }
    | { status: "success"; count: number };

export function useStreamLoadRecords() {
    const [state, setState] = useState<LoadState>({ status: "idle" });

    const recordsRef = useRef<ResponseRecord[]>([]);

    const load = useCallback(async (file: File) => {
        recordsRef.current = [];
        setState({ status: "loading", count: 0 });

        let count = 0;
        let hadError = false;

        await streamParseResponseRecords(
            file,
            (record) => {
                if (hadError) return;

                recordsRef.current.push(record);

                count += 1;
                if (count <= 3) {
                    console.log("parsed record", count, record);
                }
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

    return { state, load, recordsRef };
}
