// src/pages/Experiments.tsx

import { useState } from "react";
import type { Experiment } from "@/types/experiment";
import { ExperimentSourceSelector } from "@/widgets/ExperimentSourceSelector";
import { AlgorithmSeriesErrorMatrix } from "@/widgets/AlgorithmSeriesErrorMatrix.tsx";

export default function Experiments() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto">
            <ExperimentSourceSelector onExperimentChange={setExperiment} />

            <br />

            <AlgorithmSeriesErrorMatrix experiment={experiment} maxSeries={40} />
        </div>
    );
}
