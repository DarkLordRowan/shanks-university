// domain-intervals.ts
import type { DomainSpec, IntervalSpec } from "@/entities/experiment/model/seriesData";

export type NormalizedInterval = {
    spec: IntervalSpec;
    a: number;
    b: number;
    lc: boolean;
    rc: boolean;
    bounded: boolean;
    labelLatex: string;
};

const EPS = 1e-12;

export function latexToNumber(s: string): number {
    const t = s.trim();
    if (t === "+\\infty" || t === "\\infty" || t === "+∞" || t === "∞") return Number.POSITIVE_INFINITY;
    if (t === "-\\infty" || t === "-∞") return Number.NEGATIVE_INFINITY;

    let expr = t;
    // \frac{a}{b} / \tfrac{a}{b} → (a)/(b)
    expr = expr.replace(/\\t?frac\s*\{([^}]*)\}\s*\{([^}]*)\}/g, "($1)/($2)");
    // Служебные токены
    expr = expr.replace(/\\left|\\right|\\bigl|\\bigr|\\,|\\;|\\!/g, "");
    expr = expr.replace(/\{/g, "(").replace(/\}/g, ")");
    // Константы
    expr = expr.replace(/\\pi/g, "Math.PI");
    expr = expr.replace(/\\mathrm\{e\}/g, "Math.E");
    // Имплицитные умножения: 2\pi, (a)\pi, \pi(1+2)
    expr = expr.replace(/(\d|\))\s*(Math\.PI|Math\.E)/g, "$1*$2");
    expr = expr.replace(/(Math\.PI|Math\.E)\s*(\d|\()/g, "$1*$2");

    if (!/^[0-9+\-*/().\sMathPIE]*$/.test(expr)) throw new Error("Unsupported token in LaTeX bound");
    const f = new Function(`return (${expr});`);
    const val = Number(f());
    if (!Number.isFinite(val)) throw new Error("Non-finite");
    return val;
}

export function parseBound(s: string): number {
    try { return latexToNumber(s); } catch { return NaN; }
}

export function formatIntervalLatex(iv: IntervalSpec): string {
    const lb = iv.left_closed ? "[" : "(";
    const rb = iv.right_closed ? "]" : ")";
    return `\\bigl${lb} ${iv.from},\\, ${iv.to} \\bigr${rb}`;
}

export function normalizeInterval(iv: IntervalSpec): NormalizedInterval {
    const a0 = parseBound(iv.from);
    const b0 = parseBound(iv.to);
    const lo = Math.min(a0, b0);
    const hi = Math.max(a0, b0);
    const bounded = Number.isFinite(lo) && Number.isFinite(hi);
    return {
        spec: iv,
        a: lo,
        b: hi,
        lc: !!iv.left_closed,
        rc: !!iv.right_closed,
        bounded,
        labelLatex: formatIntervalLatex(iv),
    };
}

export function intervalsFromDomain(domain: DomainSpec): NormalizedInterval[] {
    return (domain.intervals || []).map(normalizeInterval);
}

export function inInterval(x: number, it: NormalizedInterval): boolean {
    const leftOk  = it.lc ? x >= it.a - EPS : x > it.a + EPS;
    const rightOk = it.rc ? x <= it.b + EPS : x < it.b - EPS;
    return leftOk && rightOk;
}

export function domainContains(domain: DomainSpec, x: number): boolean {
    for (const iv of intervalsFromDomain(domain)) {
        if (inInterval(x, iv)) return true;
    }
    for (const p of domain.points || []) {
        const v = parseBound(p);
        if (!Number.isNaN(v) && Math.abs(x - v) <= EPS) return true;
    }
    return false;
}

export function uniqueNumbers(nums: number[], eps = EPS): number[] {
    const out: number[] = [];
    for (const v of nums) {
        if (Number.isFinite(v) && !out.some(u => Math.abs(u - v) <= eps)) out.push(v);
    }
    return out.sort((x, y) => x - y);
}

export function characteristicPoints(domain: DomainSpec): number[] {
    const ints = intervalsFromDomain(domain);
    const ends = uniqueNumbers(ints.flatMap(iv => [iv.a, iv.b]));
    const pts = uniqueNumbers((domain.points || []).map(parseBound));
    return uniqueNumbers([...ends, ...pts]);
}
