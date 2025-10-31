import React, { useState } from "react";
import { SERIES, type SeriesNode } from "../../data/series";
import { SelectSeries } from "../../components/SelectSeries.tsx";

const Experiments_v2: React.FC = () => {

    const [series, setSeries] = useState<SeriesNode | null>(null);

    return (
        <div className="mx-auto max-w-6xl px-4 py-6">
            <SelectSeries items={SERIES} value={series} onChange={setSeries} />
        </div>
    );
};

export default Experiments_v2;
