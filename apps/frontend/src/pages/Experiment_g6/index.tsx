import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import { AlgorithmSeriesErrorMatrix } from "@/widgets/AlgorithmSeriesErrorMatrix";
import { usePersistedExperiment } from "@/shared/lib/hooks/usePersistedExperiment";

export default function Experiment_g6() {

    const [experiment, setExperiment] = usePersistedExperiment();

    return (
        <div className="mx-auto">

            <ParquetFolderInput onExperimentChange={setExperiment}/>
            <br/>

            {/*<ExperimentViewer experiment={experiment} />*/}


            <AlgorithmSeriesErrorMatrix
                experiment={experiment}
                maxSeries={40}
            />

        </div>
    );
}
