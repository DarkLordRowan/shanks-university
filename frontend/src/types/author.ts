import { AUTHORS } from "../data/authors.ts";

export type Author = {
    id: string;
    name: string;
    url?: string;
};

export type AuthorId = keyof typeof AUTHORS;

