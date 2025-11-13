import { ResponseRecordsSchema } from "../../api/experiments/schema";
import type { ResponseRecords } from "../../api/experiments/dto";
import type { ValidationResult } from "./validationTypes";
import { formatZodIssues } from "./formatZodIssues";

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
            issues: [
                {
                    path: "root",
                    code: "invalid_json",
                    message: `Некорректный JSON: ${(e as Error).message}`,
                    rawPath: [],
                },
            ],
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
