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

{
    "id": "half_minus_sinx_multi_pi_4_series",
    "num": 0,
    "num": 0,
    "title": "",
    "formula": "",
    "x_from": "",
    "x_to": "",
    "left_closed": false,
    "right_closed": false,
    "speed": "",
    "x_from": "",
    "x_to": "",
    "left_closed": false,
    "right_closed": false,
    "speed": "",
    "document": "series.docx"
}

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
