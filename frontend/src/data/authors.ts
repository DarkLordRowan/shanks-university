import type { Author, AuthorId } from "../types/author.ts";

export const AUTHORS = {
    wynn: {
        id: "wynn",
        name: "Peter Wynn",
        url: "https://en.wikipedia.org/wiki/Peter_Wynn_(mathematician)",
    },
    brezinski: {
        id: "brezinski",
        name: "Claude Brezinski",
        url: "https://en.wikipedia.org/wiki/Claude_Brezinski", // если появится
    },
    drummond: {
        id: "drummond",
        name: "Peter Drummond",
        url: undefined,
    },
    levin: {
        id: "levin",
        name: "David Levin",
        url: undefined, // пока нет подходящей статьи
    },
    sidi: {
        id: "sidi",
        name: "Avram Sidi",
        url: undefined,
    },
    ford: {
        id: "ford",
        name: "William F. Ford",
        url: undefined,
    },
    lubkin: {
        id: "lubkin",
        name: "Sidney Lubkin",
        url: undefined,
    },
    richardson: {
        id: "richardson",
        name: "Lewis F. Richardson",
        url: "https://en.wikipedia.org/wiki/Lewis_F._Richardson",
    },
    weniger: {
        id: "weniger",
        name: "Ernst Joachim Weniger",
        url: undefined, // нет
    },
    chang: {
        id: "chang",
        name: "Chang", // слишком общий
        url: undefined,
    },
    shanks: {
        id: "shanks",
        name: "Daniel Shanks",
        url: "https://en.wikipedia.org/wiki/Daniel_Shanks_(mathematician)",
    },
} satisfies Record<string, Author>;

export function resolveAuthors(ids?: AuthorId[]): Author[] {
    if (!ids?.length) return [];
    return ids.map((id) => AUTHORS[id]).filter(Boolean);
}
