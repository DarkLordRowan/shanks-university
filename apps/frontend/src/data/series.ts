export type IntervalSpec = {
    from: string;
    to: string;
    left_closed?: boolean;  // true → [a, ...
    right_closed?: boolean; // true → ..., b]
};

export type DomainSpec = {
    intervals: IntervalSpec[];
    points?: string[];
};

export type SeriesNode = {
    id: string;
    num: number;
    title: string;
    formula?: string;
    domain: DomainSpec;
    speed?: string;
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
        console.error("Error fetching series data from GitHub", response.statusText);
        throw new Error("Failed to fetch series data from GitHub");
    }

    const data: SeriesNode[] = await response.json();

    return data.map((jsonData) => ({
        id: jsonData.id,
        num: jsonData.num,
        title: jsonData.title,
        formula: jsonData.formula,
        domain: jsonData.domain,
        speed: jsonData.speed,
        document: jsonData.document,
    }));
};

export const SERIES: SeriesNode[] = await getSeriesDataFromGitHub();
