import { AUTHORS } from "../data/authors.ts";

export type Author = {
    id: string;
    name: string;
    url?: string;
};

export type AuthorId = keyof typeof AUTHORS;

export function resolveAuthors(ids?: AuthorId[]): Author[] {
    if (!ids?.length) return [];
    return ids.map((id) => AUTHORS[id]).filter(Boolean);
}
