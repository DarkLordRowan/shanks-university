import type { AuthorId } from "@/entities/author/model/author.ts";

export type AlgNode = {
    id: string;
    title: string;
    subtitle?: string;
    url?: string;
    document?: string;
    authorIds?: AuthorId[];
    children?: AlgNode[];
};
