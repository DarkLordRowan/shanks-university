import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import { AlgorithmSeriesDiffHeatmap } from "@/widgets/AlgorithmSeriesDiffHeatmap.tsx";
import { usePersistedExperiment } from "@/shared/lib/hooks/usePersistedExperiment.ts";

export default function Experiment_g7() {
    const [experiment, setExperiment] = usePersistedExperiment();

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
