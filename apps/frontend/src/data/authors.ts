import type { Author, AuthorId } from "../types/author.ts";

const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "VecImpl",
    dataFile: "data/author/authors.json",
};

const buildAuthorsFileLink = () => {
    return `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${GH.dataFile}`;
};

export const getAuthorsDataFromGitHub = async (): Promise<Record<string, Author>> => {
    const rawUrl = buildAuthorsFileLink();
    const response = await fetch(rawUrl);

    if (!response.ok) {
        console.error('Error fetching authors data from GitHub', response.statusText);
        throw new Error('Failed to fetch authors data from GitHub');
    }

    const data: Author[] = await response.json();

    return data.reduce((acc, authorData) => {
        acc[authorData.id] = authorData;
        return acc;
    }, {} as Record<string, Author>);
};

export const AUTHORS: Record<string, Author> = await getAuthorsDataFromGitHub();

export function resolveAuthors(ids?: AuthorId[]): Author[] {
    if (!ids?.length) return [];
    return ids.map((id) => AUTHORS[id]).filter(Boolean);
}
