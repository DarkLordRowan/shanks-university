import { useState } from "react";
import type { Item } from "@/types/item.ts";
import { StreamFileInput } from "@/features/import-experiment-from-file/ui/StreamFileInput.tsx";
import { AlgorithmSeriesErrorMatrix } from "@/components/AlgorithmSeriesErrorMatrix.tsx";

export default function Experiment_g6() {

    const [items, setItems] = useState<Item[]>([]);

    return (
        <div className="mx-auto">

            <StreamFileInput onItemsChange={setItems}/>
            <br/>

            <AlgorithmSeriesErrorMatrix
                items={items}
                maxSeries={40}
            />

        </div>
    );
}
