import type { AlgNode } from "../../../types/algorithms";

export function flattenIds(nodes: AlgNode[]): string[] {
    const acc: string[] = [];
    const walk = (arr: AlgNode[]) => {
        for (const n of arr) {
            if (n.id) acc.push(n.id);
            if (n.children?.length) walk(n.children);
        }
    };
    walk(nodes);
    return acc;
}
