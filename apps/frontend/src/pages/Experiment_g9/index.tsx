import { useMemo, useState } from "react";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import { AlgoRankingTable } from "@/widgets/AlgoRankingTable";
import { usePersistedExperiment } from "@/shared/lib/hooks/usePersistedExperiment.ts";

export default function Experiment_g6() {

    const [experiment, setExperiment] = usePersistedExperiment();

    const [epsilonExp, setEpsilonExp] = useState(-6);


    const epsilon = useMemo(
        () => Math.pow(10, epsilonExp),
        [epsilonExp]
    );


    return (
        <div className="mx-auto">

            <ParquetFolderInput onExperimentChange={setExperiment}/>
            <br/>

            <div className="flex items-center gap-4 text-xs text-textDim">
                <div className="flex flex-col gap-1">
                    <label className="font-medium text-text">
                        Порог точности ε
                    </label>
                    <div className="flex items-baseline gap-2 font-mono">
                        <span>
                            ε = 10
                            <sup>{epsilonExp}</sup>
                        </span>
                        <span className="text-textDim/80">
                            ≈ {epsilon.toExponential(2)}
                        </span>
                    </div>
                </div>

                <div className="flex-1">
                    <input
                        type="range"
                        min={-100}
                        max={-1}
                        step={1}
                        value={epsilonExp}
                        onChange={(e) =>
                            setEpsilonExp(parseInt(e.target.value, 10))
                        }
                        className="w-full"
                    />
                    <div className="flex justify-between text-[10px] mt-1">
                        <span>10^-100</span>
                        <span>10^-50</span>
                        <span>10^-1</span>
                    </div>
                </div>
            </div>

            <AlgoRankingTable experiment={experiment} epsilon={epsilon}/>

        </div>
    );
}
