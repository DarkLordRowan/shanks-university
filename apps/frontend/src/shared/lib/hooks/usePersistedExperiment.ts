import { useEffect, useState } from "react";
import type { Experiment } from "@/types/experiment";

export function usePersistedExperiment() {
    const [experiment, setExperiment] = useState<Experiment | null>(null);

    useEffect(() => {
        const raw = localStorage.getItem("experiment");
        if (!raw) return;
        try {
            const parsed = JSON.parse(raw) as Experiment;
            setExperiment(parsed);
        } catch {}
    }, []);

    const update = (ex: Experiment | null) => {
        setExperiment(ex);
        if (ex === null) {
            localStorage.removeItem("experiment");
        } else {
            localStorage.setItem("experiment", JSON.stringify(ex));
        }
    };

    return [experiment, update] as const;
}
