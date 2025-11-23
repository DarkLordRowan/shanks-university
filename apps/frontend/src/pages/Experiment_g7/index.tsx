import type { Experiment } from "@/entities/experiment/model/experiment";
import { useState } from "react";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import { AlgorithmSeriesDiffHeatmap } from "@/widgets/AlgorithmSeriesDiffHeatmap.tsx";

export default function Experiment_g7() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto">

            <ParquetFolderInput onExperimentChange={setExperiment}/>
            <br/>

            <AlgorithmSeriesDiffHeatmap
                experiment={experiment}
                maxSeries={40}
            />

        </div>
    );
}
