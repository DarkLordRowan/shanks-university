export const GH_SERIES = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "Fixes",
    srcFolder: "shanks_transformation/series", // .hpp с кодом ряда
    docsFolder: "theory/series",               // .tex с теорией по ряду
};

export function buildSeriesSrcLinks(id: string) {
    const path = `${GH_SERIES.srcFolder}/${id}.hpp`;
    const raw = `https://raw.githubusercontent.com/${GH_SERIES.owner}/${GH_SERIES.repo}/${GH_SERIES.branch}/${path}`;
    const web = `https://github.com/${GH_SERIES.owner}/${GH_SERIES.repo}/blob/${GH_SERIES.branch}/${path}`;
    return { raw, web, path };
}

export function buildSeriesDocLinks(id: string) {
    const path = `${GH_SERIES.docsFolder}/${id}.tex`;
    const raw = `https://raw.githubusercontent.com/${GH_SERIES.owner}/${GH_SERIES.repo}/${GH_SERIES.branch}/${path}`;
    const web = `https://github.com/${GH_SERIES.owner}/${GH_SERIES.repo}/blob/${GH_SERIES.branch}/${path}`;
    return { raw, web, path };
}
