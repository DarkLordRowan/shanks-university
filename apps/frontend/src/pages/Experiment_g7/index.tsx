import { useCallback, useMemo, useRef, useState } from "react";
import type { DiffCell } from "@/types/diffCell";
import { StreamDiffFileInput } from "@/ui/StreamDiffFileInput.tsx";
import { AlgorithmSeriesDiffHeatmap } from "@/components/AlgorithmSeriesDiffHeatmap.tsx";

export default function Experiment_g7() {
    const diffMapRef = useRef<Map<string, DiffCell>>(new Map());

    const [cells, setCells] = useState<DiffCell[]>([]);
    const [hasPrev, setHasPrev] = useState(false);
    const [hasNext, setHasNext] = useState(false);

    const handlePrevCells = useCallback((allCells: DiffCell[]) => {
        setCells(allCells);
        setHasPrev(true);
    }, []);

    const handleNextCells = useCallback((allCells: DiffCell[]) => {
        setCells(allCells);
        setHasNext(true);
    }, []);
    const stats = useMemo(() => {
        let withPrev = 0;
        let withNext = 0;
        for (const c of cells) {
            if (c.scorePrev != null) withPrev++;
            if (c.scoreNext != null) withNext++;
        }
        return {withPrev, withNext, total: cells.length};
    }, [cells]);

    return (
        <div className="mx-auto space-y-4">
            <h1 className="text-lg font-semibold">Experiment_g7</h1>

            <div className="flex flex-col gap-4 md:flex-row">
                <div className="flex-1">
                    <StreamDiffFileInput
                        side="prev"
                        diffMapRef={diffMapRef}
                        onDiffCellsChange={handlePrevCells}
                        label="JSON #1 (prev)"
                    />
                </div>
                <div className="flex-1">
                    <StreamDiffFileInput
                        side="next"
                        diffMapRef={diffMapRef}
                        onDiffCellsChange={handleNextCells}
                        label="JSON #2 (next)"
                    />
                </div>
            </div>

            <div className="text-xs text-textDim">
                Всего ячеек: {stats.total} ·
                {" "}есть prev: {stats.withPrev} ·
                {" "}есть next: {stats.withNext} ·
                {" "}загружен первый: {hasPrev ? "да" : "нет"} ·
                {" "}загружен второй: {hasNext ? "да" : "нет"}
            </div>

            {hasPrev && hasNext && cells.length > 0 && (
                <AlgorithmSeriesDiffHeatmap cells={cells} maxSeries={40} />
            )}
        </div>
    );
}
