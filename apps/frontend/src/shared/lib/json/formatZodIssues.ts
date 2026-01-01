import type { ZodError } from "zod";
import type { ValidationIssue } from "./validationTypes";

function pathToString(path: readonly PropertyKey[]): string {
    if (path.length === 0) return "root";
    let result = "root";
    for (const segment of path) {
        if (typeof segment === "number") {
            result += `[${segment}]`;
        } else if (typeof segment === "string") {
            result += `.${segment}`;
        } else {
            result += `.[${String(segment)}]`;
        }
    }
    return result;
}

export function formatZodIssues(error: ZodError): ValidationIssue[] {
    return error.issues.map(
        (issue): ValidationIssue => ({
            path: pathToString(issue.path),
            code: issue.code,
            message: issue.message,
            rawPath: issue.path,
        }),
    );
}
