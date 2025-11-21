// src/pages/Experiments.tsx

import { useState } from "react";
import type { Experiment } from "@/types/experiment";
import { ExperimentViewSwitcher } from "@/widgets/ExperimentViewSwitcher";
import { ExperimentSourceWrapper } from "@/widgets/ExperimentSourceWrapper.tsx";

export default function Experiments() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    return (
        <div className="mx-auto mt-6 flex max-w-6xl flex-col gap-6 px-4">
            <div className="flex justify-center">
                <ExperimentSourceWrapper onExperimentChange={setExperiment} />
            </div>

            {experiment && <ExperimentViewSwitcher experiment={experiment} />}
        </div>
    );
}
