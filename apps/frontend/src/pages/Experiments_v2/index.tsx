import React, { useMemo, useRef } from "react";
import { SERIES, type SeriesNode } from "../../data/series";
import { randomFromDomain } from "./utils/domain";

const Experiments_v2: React.FC = () => {
    const seriesSorted: SeriesNode[] = useMemo(
        () => [...(SERIES ?? [])]
            .sort((a, b) => a.num - b.num)
            .filter((s) => s.num!=0),
        []
    );

    const xByIdRef = useRef<Record<string, number>>({});
    const xById = xByIdRef.current;

    return (
        <div>
            {seriesSorted.map((s) => {
                const id = s.id;
                if (!(id in xById)) xById[id] = randomFromDomain(s);
                const x = xById[id];

                return (
                    <div
                        key={id}
                        style={{
                            marginBottom: "1rem",
                            border: "1px solid #ccc",
                            padding: "0.5rem",
                        }}
                    >
                        <div><strong>ID:</strong> {s.id}</div>
                        <div><strong>Номер:</strong> {s.num}</div>
                        <div><strong>Название:</strong> {s.title}</div>
                        {s.formula && (
                            <div><strong>Формула:</strong> {s.formula}</div>
                        )}
                        <div>
                            <strong>Область:</strong>{" "}
                            {s.domain.intervals
                                .map((iv) =>
                                    `${iv.left_closed ? "[" : "("}${iv.from}, ${iv.to}${iv.right_closed ? "]" : ")"}`
                                )
                                .join(" ∪ ")}
                        </div>
                        <div>
                            <strong>Случайный x:</strong>{" "}
                            {Number.isFinite(x)
                                ? x.toPrecision(8)
                                : String(x)}
                        </div>
                        {s.speed && <div><strong>Скорость:</strong> {s.speed}</div>}
                        {s.document && (
                            <div><strong>Документ:</strong> {s.document}</div>
                        )}
                    </div>
                );
            })}
        </div>
    );
};

export default Experiments_v2;
