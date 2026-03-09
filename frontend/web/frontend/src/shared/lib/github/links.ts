export type GithubRepoConfig = {
    owner: string;
    repo: string;
    branch: string;
};

export type GithubFileLinks = {
    raw: string;
    web: string;
    path: string;
};

export function buildGithubFileLinks(
    config: GithubRepoConfig,
    folder: string,
    fileName: string,
): GithubFileLinks {
    const path = `${folder}/${fileName}`;
    const raw = `https://raw.githubusercontent.com/${config.owner}/${config.repo}/${config.branch}/${path}`;
    const web = `https://github.com/${config.owner}/${config.repo}/blob/${config.branch}/${path}`;

    return { raw, web, path };
}
