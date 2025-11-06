export type AlgorithmNode = {
    id: string;
    title: string;
    subtitle?: string;
    authorIds?: string[];
    python_id: string;
    args?: unknown[];
};

const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "Fixes",
    dataFile: "data/algorithm/algorithms.json",
};

const buildAlgorithmsFileLink = () => {
    return `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${GH.dataFile}`;
};

export const getAlgorithmsDataFromGitHub = async (): Promise<AlgorithmNode[]> => {
    const rawUrl = buildAlgorithmsFileLink();
    const response = await fetch(rawUrl);

    if (!response.ok) {
        console.error("Error fetching algorithm data from GitHub", response.statusText);
        throw new Error("Failed to fetch algorithm data from GitHub");
    }

    const data: AlgorithmNode[] = await response.json();

    return data.map((jsonData) => ({
        id: jsonData.id,
        title: jsonData.title,
        subtitle: jsonData.subtitle,
        authorIds: jsonData.authorIds,
        python_id: jsonData.python_id,
        args: jsonData.args,
    }));
};

export const ALGORITHMS: AlgorithmNode[] = await getAlgorithmsDataFromGitHub();
