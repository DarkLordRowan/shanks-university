import { latexToNumber, EPS } from "./math";
import { rand01, normal01 } from "./random";
import type { SeriesNode } from "../../../data/series.ts";

type Bounds = { a: number; b: number; leftClosed: boolean; rightClosed: boolean };

function normalizeInterval(interval: {
    from: string;
    to: string;
    left_closed?: boolean;
    right_closed?: boolean;
}): Bounds {
    const ra = latexToNumber(interval.from);
    const rb = latexToNumber(interval.to);

    let a = Number.isFinite(ra) || ra === -Infinity ? ra : -Infinity;
    let b = Number.isFinite(rb) || rb === Infinity ? rb : Infinity;
    let leftClosed = Boolean(interval.left_closed);
    let rightClosed = Boolean(interval.right_closed);

    if (Number.isFinite(a) && Number.isFinite(b) && a > b) {
        [a, b] = [b, a];
        [leftClosed, rightClosed] = [rightClosed, leftClosed];
    }

    return { a, b, leftClosed, rightClosed };
}

function randomInBounds({ a, b, leftClosed, rightClosed }: Bounds): number {
    if (Number.isFinite(a) && Number.isFinite(b)) {
        const u = rand01();
        let x = a + u * (b - a);
        if (!leftClosed) x = Math.max(x, a + EPS);
        if (!rightClosed) x = Math.min(x, b - EPS);
        return x;
    }
    if (Number.isFinite(a) && !Number.isFinite(b)) {
        const u = rand01();
        let x = a + u / (1 - u);
        if (!leftClosed) x = Math.max(x, a + EPS);
        return x;
    }
    if (!Number.isFinite(a) && Number.isFinite(b)) {
        const u = rand01();
        let x = b - u / (1 - u);
        if (!rightClosed) x = Math.min(x, b - EPS);
        return x;
    }
    return normal01(); // (-∞, +∞)
}

export function randomFromDomain(s: SeriesNode): number {
    const intervals = s.domain?.intervals ?? [];
    if (intervals.length === 0) return normal01();

    // равномерный выбор интервала
    const index = Math.floor(Math.random() * intervals.length);
    const bounds = normalizeInterval(intervals[index]);
    return randomInBounds(bounds);
}
