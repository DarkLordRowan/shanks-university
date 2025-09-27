export type SeriesNode = {
    id: string;
    title: string;
    subtitle?: string;
    formula?: string;
    url?: string;
    document?: string;
};

const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "Fixes",
    dataFile: "data/series/serieses.json",
};

const buildSeriesFileLink = () => {
    return `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${GH.dataFile}`;
};

export const getSeriesDataFromGitHub = async (): Promise<SeriesNode[]> => {
    const rawUrl = buildSeriesFileLink();
    const response = await fetch(rawUrl);

    if (!response.ok) {
        console.error('Error fetching series data from GitHub', response.statusText);
        throw new Error('Failed to fetch series data from GitHub');
    }

    const data: SeriesNode[] = await response.json();

    return data.map((jsonData) => ({
        id: jsonData.id,
        title: jsonData.title,
        subtitle: jsonData.subtitle,
        formula: jsonData.formula,
        document: jsonData.document,
        url: jsonData.url,
    }));
};

export const SERIES: SeriesNode[] = await getSeriesDataFromGitHub();
