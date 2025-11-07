import { useMemo } from "react";
import type { Item } from "../types/item";
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

type RowAgg = {
    algo: string;         // ключ категории
    n: number | null;     // агрегированное значение (min n)
    count: number;        // сколько строк попало в агрегат
    hitCount: number;     // сколько из них достигли eps (имеют n != null)
    ns: Array<number | null>; // все n для тултипа
};

function yWidthPx(labels: string[], min = 180, max = 420) {
    const len = labels.reduce((m, s) => Math.max(m, s.length), 0);
    // грубая оценка: 7px на символ + паддинги
    return Math.max(min, Math.min(max, 7 * len + 24));
}

export function StepsToToleranceBar({
                                        items,
                                        eps = 1e-8,
                                    }: {
    items: Item[];
    eps?: number;
}) {
    const data = useMemo<RowAgg[]>(() => {
        // 1) собираем сырые пары algo->nHit
        const pairs: { algo: string; n: number | null }[] = [];
        for (const it of items) {
            const base =
                it.algorithm.m != null
                    ? `${it.algorithm.algorithmName} (m=${it.algorithm.m})`
                    : it.algorithm.algorithmName;

            let nHit: number | null = null;
            for (const c of it.computed) {
                // достигли заданной точности по ускоренному значению
                const d = c.accel_value_deviation;
                if (typeof d === "number" && d >= 0 && d <= eps) {
                    nHit = c.n;
                    break;
                }
            }
            pairs.push({ algo: base, n: nHit });
        }

        // 2) агрегация по algo
        const map = new Map<string, RowAgg>();
        for (const p of pairs) {
            let row = map.get(p.algo);
            if (!row) {
                row = { algo: p.algo, n: null, count: 0, hitCount: 0, ns: [] };
                map.set(p.algo, row);
            }
            row.count += 1;
            row.ns.push(p.n);
            if (p.n != null) {
                row.hitCount += 1;
                row.n = row.n == null ? p.n : Math.min(row.n, p.n); // берём минимум шагов
            }
        }

        // 3) сортировка: сначала по n (null внизу), потом по алфавиту
        const arr = Array.from(map.values());
        arr.sort((a, b) => {
            if (a.n == null && b.n == null) return a.algo.localeCompare(b.algo);
            if (a.n == null) return 1;
            if (b.n == null) return -1;
            return a.n - b.n || a.algo.localeCompare(b.algo);
        });
        return arr;
    }, [items, eps]);

    const yWidth = useMemo(() => yWidthPx(data.map(d => {
        const suffix = d.count > 1 ? ` ×${d.count}` : "";
        return d.algo + suffix;
    })), [data]);

    const formatYAxis = (v: string) => {
        const row = data.find(d => d.algo === v);
        if (!row) return v;
        return row.count > 1 ? `${v} ×${row.count}` : v;
    };

    const tooltipLabel = (v: any) => String(v);

    const tooltipContent = (info: any) => {
        // info.payload[0].payload содержит всю строку RowAgg
        const p = info?.payload?.[0]?.payload as RowAgg | undefined;
        if (!info?.active || !p) return null;

        const nums = p.ns
            .map(x => (x == null ? "—" : String(x)))
            .sort((a, b) => {
                if (a === "—" && b === "—") return 0;
                if (a === "—") return 1;
                if (b === "—") return -1;
                return Number(a) - Number(b);
            });

        return (
            <div className="rounded-lg border bg-white/95 p-2 text-xs shadow-md dark:bg-neutral-900/95 dark:border-neutral-700">
                <div className="mb-1 font-semibold">{p.algo}</div>
                <div>ε = {eps}</div>
                <div>всего: {p.count}, попали: {p.hitCount}</div>
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
                    dataKey="algo"
                    width={yWidth}
                    interval={0}          // ← показывать все подписи
                    tickMargin={6}
                    tickFormatter={formatYAxis}
                />
                <Tooltip labelFormatter={tooltipLabel} content={tooltipContent as any} />
                <Legend />
                <Bar dataKey="n" name={`nₑ (ε=${eps})`} fill="#777" />
            </BarChart>
        </ResponsiveContainer>
    );
}
