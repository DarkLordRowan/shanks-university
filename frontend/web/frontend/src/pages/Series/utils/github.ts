import { buildGithubFileLinks } from "@/shared/lib/github/links";

export const GH_SERIES = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "VecImpl",
    srcFolder: "shanks_transformation/series/serieses",
    docsFolder: "theory/series",
} as const;

const repoConfig = {
    owner: GH_SERIES.owner,
    repo: GH_SERIES.repo,
    branch: GH_SERIES.branch,
};

export function buildSeriesSrcLinks(id: string) {
    return buildGithubFileLinks(repoConfig, GH_SERIES.srcFolder, `${id}.hpp`);
}

export function buildSeriesDocLinks(id: string) {
    return buildGithubFileLinks(repoConfig, GH_SERIES.docsFolder, `${id}.tex`);
}
