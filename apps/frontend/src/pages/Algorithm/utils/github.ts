export const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "Fixes",
    srcFolder: "shanks_transformation/methods/algorithm", // файлы .hpp
    docsFolder: "theory/algorithm", // файлы .tex
};

export function buildSrcLinks(algId: string) {
    const path = `${GH.srcFolder}/${algId}.hpp`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}

export function buildDocLinks(algId: string) {
    const path = `${GH.docsFolder}/${algId}.tex`;
    const raw = `https://raw.githubusercontent.com/${GH.owner}/${GH.repo}/${GH.branch}/${path}`;
    const web = `https://github.com/${GH.owner}/${GH.repo}/blob/${GH.branch}/${path}`;
    return { raw, web, path };
}
