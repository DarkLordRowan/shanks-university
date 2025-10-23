export const EPS = 1e-12;

export function latexToNumber(v: unknown): number {
    if (v == null) return NaN;
    if (typeof v === "number") return v;
    if (typeof v !== "string") return Number(v);

    let expr = v.trim();
    if (!expr) return NaN;

    if (/^-\\infty$/.test(expr)) return -Infinity;
    if (/^\+?\\infty$/.test(expr)) return Infinity;

    expr = expr.replace(/\s+/g, "");
    expr = expr.replace(/\\t?frac\s*{([^}]+)}\s*{([^}]+)}/g, "($1)/($2)");
    expr = expr.replace(/\\pi/g, "Math.PI");
    expr = expr
        .replace(/(\d)(Math\.PI)/g, "$1*$2")
        .replace(/(\))(?=Math\.PI)/g, "$1*")
        .replace(/(Math\.PI)(\d)/g, "$1*$2")
        .replace(/(Math\.PI)\(/g, "$1*(");

    // eslint-disable-next-line no-new-func
    const val = Function(`"use strict"; return (${expr});`)();
    return Number(val);
}
