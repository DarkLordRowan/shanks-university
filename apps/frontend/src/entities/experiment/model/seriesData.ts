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
    python_id: string;
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
    branch: "VecImpl",
    dataFile: "data/series/serieses.json",
};

const buildSeriesFileLink = () => {
    return `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${GH.dataFile}`;
};

let cachedSeriesPromise: Promise<SeriesNode[]> | null = null;

export async function getSeriesDataFromGitHub(): Promise<SeriesNode[]> {
    if (cachedSeriesPromise) return cachedSeriesPromise;

    const rawUrl = buildSeriesFileLink();

    cachedSeriesPromise = (async () => {
        const response = await fetch(rawUrl);
        if (!response.ok) {
            console.error("Error fetching series data from GitHub", response.statusText);
            throw new Error("Failed to fetch series data from GitHub");
        }

        const data: SeriesNode[] = await response.json();

        let items = data.map((jsonData) => ({
            id: jsonData.id,
            python_id: jsonData.python_id,
            num: jsonData.num,
            title: jsonData.title,
            formula: jsonData.formula,
            domain: jsonData.domain,
            speed: jsonData.speed,
            document: jsonData.document,
        }));

        items = items.filter((i) => Number.isFinite(i.num) && i.num > 0);

        items
            .sort((a, b) => a.num - b.num || a.id.localeCompare(b.id));

        return items;
    })();

    return cachedSeriesPromise;
}

export const SERIES: SeriesNode[] = await getSeriesDataFromGitHub();
