export type SeriesNode = {
    id: string;
    num: string;
    title: string;
    formula?: string;
    x_from?: string;
    x_to?: string;
    left_closed?: boolean;
    right_closed?: boolean;
    speed?:string;
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
        num: jsonData.num,
        title: jsonData.title,
        formula: jsonData.formula,
        x_from: jsonData.x_from,
        x_to: jsonData.x_to,
        left_closed: jsonData.left_closed,
        right_closed: jsonData.right_closed,
        speed: jsonData.speed,
        document: jsonData.document,
    }));
};

export const SERIES: SeriesNode[] = await getSeriesDataFromGitHub();
