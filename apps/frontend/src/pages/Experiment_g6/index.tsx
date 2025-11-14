import { useState } from "react";
import { AccelerationGainChartByN } from "../../charts/AccelerationGainChartByN.tsx";
import { DeltaToLimitPartialSumChart } from "../../charts/DeltaToLimitPartialSumChart.tsx";
import { LogPsDevChartByN } from "../../charts/LogPsDevChartByN.tsx";
import { PartialSumChartByN } from "../../charts/PartialSumChartByN.tsx";
import { AccelValueChartByN } from "../../charts/AccelValueChartByN.tsx";
import { LogAccelDevChartByN } from "../../charts/LogAccelDevChartByN.tsx";
import { EOCPartialSumChartByN } from "../../charts/EOCPartialSumChartByN.tsx";
import { SeriesTermChartByN } from "../../charts/SeriesTermChartByN.tsx";
import { EOCAccelChartByN } from "../../charts/EOCAccelChartByN.tsx";
import { StepsToToleranceExplorer } from "../../charts/StepsToToleranceExplorer.tsx";
import type { Item } from "@/types/item.ts";
import { StreamFileInput } from "@/features/import-experiment-from-file/ui/StreamFileInput.tsx";
import { ItemsFilterPanel } from "@/components/ItemsFilterPanel.tsx";

export default function Experiment_g6() {

    const [items, setItems] = useState<Item[]>([]);

    const [filteredItems, setFilteredItems] = useState<Item[]>([]);



    return (
        <div className="mx-auto max-w-6xl px-4 py-6">

            <StreamFileInput onItemsChange={setItems} />
            <br/>

            <div>
                {items.length > 0 && (
                    <div className="mt-4 w-full space-y-6">

                        <ItemsFilterPanel items={items} onChange={setFilteredItems} />

                        <div className="space-y-10">
                            {/* 0. Число шагов до точности ε */}
                            <StepsToToleranceExplorer items={filteredItems} defaultEps={1e-4}/>

                            {/* 1. Частичные суммы S_n */}
                            <PartialSumChartByN items={filteredItems}/>

                            {/* 2. Ошибка частичных сумм |S_n - L| */}
                            <DeltaToLimitPartialSumChart items={filteredItems}/>

                            {/* 3. log10(|S_n - L|) */}
                            <LogPsDevChartByN items={filteredItems}/>

                            {/* 4. Ускоренные значения A_n */}
                            <AccelValueChartByN items={filteredItems}/>

                            {/* 5. log10(|A_n - L|) */}
                            <LogAccelDevChartByN items={filteredItems}/>

                            {/* 6. Коэффициент усиления G(n) = log10(|S_n-L| / |A_n-L|) */}
                            <AccelerationGainChartByN items={filteredItems}/>

                            {/* 7. Порядок сходимости частичных сумм */}
                            <EOCPartialSumChartByN items={filteredItems}/>

                            {/* 8. Порядок сходимости ускоренных */}
                            <EOCAccelChartByN items={filteredItems}/>

                            {/* 9. Значения членов ряда a_n */}
                            <SeriesTermChartByN items={filteredItems}/>
                        </div>
                    </div>
                )}

            </div>

        </div>
    );
}
