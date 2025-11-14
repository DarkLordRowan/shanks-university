import { useState } from "react";
import { StepsToToleranceExplorer } from "../../charts/StepsToToleranceExplorer.tsx";
import type { Item } from "@/types/item.ts";
import { StreamFileInput } from "@/features/import-experiment-from-file/ui/StreamFileInput.tsx";
import { ItemsFilterPanel } from "@/components/ItemsFilterPanel.tsx";
import { AccelValueByN_Universal } from "@/charts/universal/AccelValueByN_Universal.tsx";
import { PartialSumByN_Universal } from "@/charts/universal/PartialSumByN_Universal.tsx";
import { LogAccelDevByN_Universal } from "@/charts/universal/LogAccelDevByN_Universal.tsx";
import { LogPsDevByN_Universal } from "@/charts/universal/LogPsDevByN_Universal.tsx";
import { ItemsTable } from "@/components/ItemsTable.tsx";

export default function Experiment_g6() {

    const [items, setItems] = useState<Item[]>([]);

    const [filteredItems, setFilteredItems] = useState<Item[]>([]);


    return (
        <div className="mx-auto max-w-6xl px-4 py-6">

            <StreamFileInput onItemsChange={setItems}/>
            <br/>

            <div>
                {items.length > 0 && (
                    <div className="mt-4 w-full space-y-6">

                        <ItemsFilterPanel items={items} onChange={setFilteredItems}/>

                        <div className="space-y-10">
                            {/* 0. Число шагов до точности ε */}

                            {filteredItems.length}

                            {filteredItems.length > 0 && (
                               <div className="mt-4 w-full space-y-6">
                                   test

                                   <ItemsTable items={filteredItems} />


                                   <StepsToToleranceExplorer items={filteredItems}/>

                                   <AccelValueByN_Universal items={filteredItems}/>
                                   <PartialSumByN_Universal items={filteredItems}/>
                                   <LogAccelDevByN_Universal items={filteredItems}/>
                                   <LogPsDevByN_Universal items={filteredItems}/>
                               </div>
                            )}

                        </div>
                    </div>
                )}

            </div>

        </div>
    );
}
