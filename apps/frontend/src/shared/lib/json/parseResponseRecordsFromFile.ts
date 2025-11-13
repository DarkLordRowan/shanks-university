import { ResponseRecordsSchema } from "@/shared/api/experiments/schema";
import type { ResponseRecords } from "@/shared/api/experiments/dto";
import type { ValidationResult } from "./validationTypes";
import { formatZodIssues } from "./formatZodIssues";

function buildJsonParseIssue(text: string, e: Error) {
    const msg = e.message || "Unknown JSON parse error";
    const match = msg.match(/position\s+(\d+)/i);
    let snippet: string | undefined;
    if (match) {
        const pos = Number(match[1]);
        const start = Math.max(0, pos - 40);
        const end = Math.min(text.length, pos + 40);
        snippet = text.slice(start, end);
    } else {
        snippet = text.slice(0, 120);
    }

    return {
        path: "root",
        code: "invalid_json",
        message: `Синтаксическая ошибка JSON: ${msg}`,
        rawPath: [],
        snippet,
    };
}

export async function parseResponseRecordsFromFile(
    file: File,
): Promise<ValidationResult<ResponseRecords>> {
    let text: string;

    try {
        text = await file.text();
    } catch (e) {
        return {
            ok: false,
            issues: [
                {
                    path: "root",
                    code: "file_read_error",
                    message: `Ошибка чтения файла: ${(e as Error).message}`,
                    rawPath: [],
                },
            ],
        };
    }

    let raw: unknown;
    try {
        raw = JSON.parse(text);
    } catch (e) {
        return {
            ok: false,
            issues: [buildJsonParseIssue(text, e as Error)],
        };
    }

    const parsed = ResponseRecordsSchema.safeParse(raw);
    if (!parsed.success) {
        return {
            ok: false,
            issues: formatZodIssues(parsed.error),
        };
    }

    return {
        ok: true,
        data: parsed.data,
    };
}
