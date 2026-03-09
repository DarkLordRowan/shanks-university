import { buildGithubFileLinks } from "@/shared/lib/github/links";

export const GH = {
    owner: "DarkLordRowan",
    repo: "shanks-university",
    branch: "VecImpl",
    srcFolder: "shanks_transformation/methods/algorithm",
    docsFolder: "theory/algorithm",
} as const;

const repoConfig = {
    owner: GH.owner,
    repo: GH.repo,
    branch: GH.branch,
};

export function buildSrcLinks(algId: string) {
    return buildGithubFileLinks(repoConfig, GH.srcFolder, `${algId}.hpp`);
}

export function buildDocLinks(algId: string) {
    return buildGithubFileLinks(repoConfig, GH.docsFolder, `${algId}.tex`);
}
