import type { AuthorId } from "./author.ts";

export type AlgNode = {
    id: string;
    title: string;
    subtitle?: string;
    url?: string;
    document?: string;
    authorIds?: AuthorId[];
    children?: AlgNode[];
};
