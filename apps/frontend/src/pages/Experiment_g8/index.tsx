import { ErrorMatrixTable } from "@/widgets/ErrorMatrixTable.tsx";
import { ParquetFolderInput } from "@/features/import-experiment-from-parquet/ui/ParquetFolderInput.tsx";
import { usePersistedExperiment } from "@/shared/lib/hooks/usePersistedExperiment";

export default function Experiment_g6() {
    const [experiment, setExperiment] = usePersistedExperiment();

    return (
        <div className="mx-auto">

            <ParquetFolderInput onExperimentChange={setExperiment}/>
            <br/>

            <ErrorMatrixTable experiment={experiment}/>
        </div>
    );
}
