// model/useStreamLoadDiffCells.ts
import { useCallback, useState } from "react";
import type { ValidationIssue } from "@/shared/lib/json/validationTypes";
import { streamParseResponseRecords } from "@/shared/lib/json/streamingParseResponseRecords";
import type { DiffCell } from "@/types/diffCell";
import { applyFromPrev, applyFromNext, buildCellKey, buildSeriesKey } from "@/lib/diffUtils";
import type { Item } from "@/types/item";
import { normalizeRecord } from "@/utils/normalizeRecords.ts";

type LoadState =
    | { status: "idle" }
    | { status: "loading"; count: number }
    | { status: "error"; issues: ValidationIssue[] }
    | { status: "success"; count: number };

type Side = "prev" | "next";

export function useStreamLoadDiffCells(
    side: Side,
    diffMapRef: React.MutableRefObject<Map<string, DiffCell>>,
) {
    const [state, setState] = useState<LoadState>({ status: "idle" });

    const load = useCallback(async (file: File) => {
        setState({ status: "loading", count: 0 });

        let count = 0;
        let i = 0;
        let hadError = false;

        await streamParseResponseRecords(
            file,
            (record) => {
                if (hadError) return;

                // ResponseRecord -> Item
                const item = normalizeRecord(record, i++);

                const seriesKey = buildSeriesKey(
                    item.series.seriesName,
                    item.series.x,
                );
                const algoKey = item.algorithm.algorithmId;
                const cellKey = buildCellKey(algoKey, seriesKey);

                const map = diffMapRef.current;
                const existing = map.get(cellKey) ?? null;

                const updated =
                    side === "prev"
                        ? applyFromPrev(item, existing)
                        : applyFromNext(item, existing);

                map.set(cellKey, updated);

                count += 1;
                if (count <= 3) {
                    console.log(`[${side}] parsed record`, count, record);
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
    }, [side, diffMapRef]);

    return { state, load };
}
