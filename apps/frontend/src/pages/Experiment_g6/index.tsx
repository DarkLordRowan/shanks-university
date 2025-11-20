import { useState } from "react";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import type { Experiment } from "@/types/experiment.ts";

export default function Experiment_g6() {

    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto">

            <ParquetFolderInput onExperimentChange={setExperiment}/>
            <br/>

            {/*<AlgorithmSeriesErrorMatrix*/}
            {/*    items={items}*/}
            {/*    maxSeries={40}*/}
            {/*/>*/}

        </div>
    );
}
