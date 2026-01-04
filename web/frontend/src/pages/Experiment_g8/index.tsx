import { useState } from "react";
import { ErrorMatrixTable } from "@/widgets/ErrorMatrixTable.tsx";
import type { Experiment } from "@/entities/experiment/model/experiment";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";

export default function Experiment_g6() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto">

            <ParquetFolderInput onExperimentChange={setExperiment}/>
            <br/>

            <ErrorMatrixTable experiment={experiment}/>
        </div>
    );
}
