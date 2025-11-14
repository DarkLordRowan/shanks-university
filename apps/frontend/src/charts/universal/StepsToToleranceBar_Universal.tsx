import { useMemo } from "react";
import type { Item } from "../../types/item";
import {
    Bar,
    BarChart,
    CartesianGrid,
    Legend,
    ResponsiveContainer,
    Tooltip,
    XAxis,
    YAxis,
} from "recharts";
import { universalSeriesKey } from "./common";

type RowAgg = {
    key: string;               // человекочитаемый ключ (seriesName · x · algo(m))
    n: number | null;          // агрегированное значение (min n)
    count: number;             // сколько items попало в агрегат
    hitCount: number;          // сколько из них достигли eps (n != null)
    ns: Array<number | null>;  // все n для тултипа
};

function yWidthPx(labels: string[], min = 180, max = 420) {
    const len = labels.reduce((m, s) => Math.max(m, s.length), 0);
    return Math.max(min, Math.min(max, 7 * len + 24));
}

export function StepsToToleranceBarUniversal({
                                                 items,
                                                 eps = 1e-8,
                                             }: {
    items: Item[];
    eps?: number;
}) {
    const data = useMemo<RowAgg[]>(() => {
        // 1) сырые пары key->nHit
        const pairs: { key: string; n: number | null }[] = [];

        for (const it of items) {
            const key = universalSeriesKey(it); // seriesName · x=… · algo(m)
            let nHit: number | null = null;

            for (const c of it.computed) {
                const d = c.accel_value_deviation;
                if (typeof d === "number" && d >= 0 && d <= eps) {
                    nHit = c.n;
                    break;
                }
            }
            pairs.push({ key, n: nHit });
        }

        // 2) агрегация по key
        const map = new Map<string, RowAgg>();
        for (const p of pairs) {
            let row = map.get(p.key);
            if (!row) {
                row = {
                    key: p.key,
                    n: null,
                    count: 0,
                    hitCount: 0,
                    ns: [],
                };
                map.set(p.key, row);
            }
            row.count += 1;
            row.ns.push(p.n);
            if (p.n != null) {
                row.hitCount += 1;
                row.n = row.n == null ? p.n : Math.min(row.n, p.n);
            }
        }

        // 3) сортировка: сначала по n (null внизу), затем по key
        const arr = Array.from(map.values());
        arr.sort((a, b) => {
            if (a.n == null && b.n == null) return a.key.localeCompare(b.key);
            if (a.n == null) return 1;
            if (b.n == null) return -1;
            return a.n - b.n || a.key.localeCompare(b.key);
        });
        return arr;
    }, [items, eps]);

    const yWidth = useMemo(
        () =>
            yWidthPx(
                data.map((d) => {
                    const suffix = d.count > 1 ? ` ×${d.count}` : "";
                    return d.key + suffix;
                }),
            ),
        [data],
    );

    const formatYAxis = (v: string) => {
        const row = data.find((d) => d.key === v);
        if (!row) return v;
        return row.count > 1 ? `${v} ×${row.count}` : v;
    };

    const tooltipLabel = (v: any) => String(v);

    const tooltipContent = (info: any) => {
        const p = info?.payload?.[0]?.payload as RowAgg | undefined;
        if (!info?.active || !p) return null;

        const nums = p.ns
            .map((x) => (x == null ? "—" : String(x)))
            .sort((a, b) => {
                if (a === "—" && b === "—") return 0;
                if (a === "—") return 1;
                if (b === "—") return -1;
                return Number(a) - Number(b);
            });

        return (
            <div className="rounded-xl2 border border-border bg-panel/95 p-2 text-xs text-textDim shadow-panel">
                <div className="mb-1 font-semibold text-white">{p.key}</div>
                <div>ε = {eps}</div>
                <div>
                    всего: {p.count}, попали: {p.hitCount}
                </div>
                <div>min n: {p.n == null ? "—" : p.n}</div>
                <div className="mt-1 opacity-80">все n: {nums.join(", ")}</div>
            </div>
        );
    };

    return (
        <ResponsiveContainer
            width="100%"
            height={Math.max(240, 28 * data.length + 40)}
        >
            <BarChart data={data} layout="vertical">
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis type="number" dataKey="n" />
                <YAxis
                    type="category"
                    dataKey="key"
                    width={yWidth}
                    interval={0} // показывать все подписи
                    tickMargin={6}
                    tickFormatter={formatYAxis}
                />
                <Tooltip
                    labelFormatter={tooltipLabel}
                    content={tooltipContent as any}
                />
                <Legend />
                <Bar dataKey="n" name={`nₑ (ε=${eps})`} />
            </BarChart>
        </ResponsiveContainer>
    );
}
