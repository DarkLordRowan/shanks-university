// src/pages/Experiments.tsx

import { useState } from "react";
import type { Experiment } from "@/types/experiment";
import { ExperimentSourceSelector } from "@/widgets/ExperimentSourceSelector";

export default function Experiments() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="flex justify-center mt-6">
            <ExperimentSourceSelector onExperimentChange={setExperiment} />
        </div>
    );
}
