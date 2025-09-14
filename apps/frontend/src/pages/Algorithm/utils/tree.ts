import type { AlgNode } from "../../../types/algorithms";

export function findNodeById(nodes: AlgNode[], id: string): AlgNode | undefined {
    for (const n of nodes) {
        if (n.id === id) return n;
        if (n.children?.length) {
            const hit = findNodeById(n.children, id);
            if (hit) return hit;
        }
    }
    return undefined;
}
