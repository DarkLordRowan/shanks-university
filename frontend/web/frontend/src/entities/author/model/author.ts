import { AUTHORS } from "@/entities/author/model/authorsData";

export type Author = {
    id: string;
    name: string;
    url?: string;
};

export type AuthorId = keyof typeof AUTHORS;

