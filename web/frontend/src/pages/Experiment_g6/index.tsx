import { useState } from "react";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { AlgorithmSeriesErrorMatrix } from "@/widgets/AlgorithmSeriesErrorMatrix";

export default function Experiment_g6() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto">
            <ParquetFolderInput onExperimentChange={setExperiment} />
            <br />

            <AlgorithmSeriesErrorMatrix experiment={experiment} maxSeries={40} />
        </div>
    );
}
