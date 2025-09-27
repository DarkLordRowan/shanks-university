import type { AlgNode } from "../types/algorithms";

const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "Fixes",
    dataFile: "data/algorithm/algorithms.json",
};

const buildAlgorithmsFileLink = () => {
    return `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${GH.dataFile}`;
};

export const getDataFromGitHub = async (): Promise<AlgNode[]> => {
    const rawUrl = buildAlgorithmsFileLink();
    const response = await fetch(rawUrl);

    if (!response.ok) {
        console.error('Error fetching algorithm data from GitHub', response.statusText);
        throw new Error('Failed to fetch algorithm data from GitHub');
    }

    const data: AlgNode[] = await response.json();

    return data.map((jsonData) => ({
        id: jsonData.id,
        title: jsonData.title,
        subtitle: jsonData.subtitle,
        authorIds: jsonData.authorIds,
        children: [],
    }));
};

export const TREE: AlgNode[] = await getDataFromGitHub();
