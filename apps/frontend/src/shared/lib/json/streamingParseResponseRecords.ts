import { ResponseRecordSchema } from "@/shared/api/experiments/schema";
import type { ResponseRecord } from "@/shared/api/experiments/dto";
import type { ValidationIssue } from "./validationTypes";
import { formatZodIssues } from "./formatZodIssues";

function makeFileLevelIssue(message: string, snippet?: string): ValidationIssue {
    return {
        path: "root",
        code: "invalid_json",
        message,
        rawPath: [],
        snippet,
    };
}

export async function streamParseResponseRecords(
    file: File,
    onRecord: (record: ResponseRecord) => void,
    onError: (issues: ValidationIssue[]) => void,
): Promise<void> {
    console.log("[streamParse] start", {
        name: file.name,
        size: file.size,
        type: file.type,
    });

    const reader = file.stream().getReader();
    const decoder = new TextDecoder();

    let buffer = "";

    // состояние автомата
    let pos = 0;                // текущая позиция в buffer
    let safePos = 0;            // до какого индекса можно безопасно отрезать buffer
    let inString = false;
    let escape = false;
    let depth = 0;              // глубина фигурных скобок { }
    let objectStart = -1;       // индекс '{' на depth = 0
    let inArray = false;        // увидели '[' верхнего уровня
    let finishedArray = false;  // увидели ']'

    let indexInArray = 0;       // root[index]
    let flushedObjects = 0;

    let chunkIndex = 0;
    let totalBytes = 0;

    let hasFatalError = false;

    const fail = (issues: ValidationIssue[]) => {
        if (hasFatalError) return;
        hasFatalError = true;
        console.error("[streamParse] fail", { issues });
        onError(issues);
    };

    const flushObject = (endPos: number) => {
        if (objectStart < 0) return;

        const start = objectStart;
        const objectIndex = indexInArray;
        indexInArray += 1;
        objectStart = -1;

        const jsonText = buffer.slice(start, endPos);
        flushedObjects += 1;

        if (flushedObjects <= 5 || flushedObjects % 1000 === 0) {
            console.log("[streamParse] flushObject", {
                objectIndex,
                jsonLength: jsonText.length,
                flushedObjects,
            });
        }

        let raw: unknown;
        try {
            raw = JSON.parse(jsonText);
        } catch (e) {
            const err = e as Error;
            const msg = err.message || "Unknown JSON parse error";
            const snippet = jsonText.slice(0, 200);

            fail([
                {
                    path: `root[${objectIndex}]`,
                    code: "invalid_json_object",
                    message: `Ошибка парсинга объекта №${objectIndex}: ${msg}`,
                    rawPath: [objectIndex],
                    objectIndex,
                    snippet,
                },
            ]);
            return;
        }

        const parsed = ResponseRecordSchema.safeParse(raw);
        if (!parsed.success) {
            const baseIssues = formatZodIssues(parsed.error);
            const issues: ValidationIssue[] = baseIssues.map((iss) => {
                const path =
                    iss.path === "root"
                        ? `root[${objectIndex}]`
                        : `root[${objectIndex}].${iss.path.slice("root.".length)}`;

                return {
                    path,
                    code: iss.code,
                    message: iss.message,
                    rawPath: [objectIndex, ...iss.rawPath],
                    objectIndex,
                };
            });

            fail(issues);
            return;
        }

        onRecord(parsed.data);
    };

    const processBuffer = () => {
        while (pos < buffer.length && !finishedArray && !hasFatalError) {
            const ch = buffer[pos];

            if (!inArray) {
                if (/\s/.test(ch)) {
                    pos += 1;
                    safePos = pos;
                    continue;
                }
                if (ch === "[") {
                    inArray = true;
                    pos += 1;
                    safePos = pos;
                    console.log("[streamParse] found '[' at", pos - 1);
                    continue;
                }

                fail([
                    makeFileLevelIssue(
                        `Ожидался '[' в начале массива, получен '${ch}'`,
                        buffer.slice(0, 80),
                    ),
                ]);
                return;
            }

            if (!inString) {
                if (ch === '"') {
                    inString = true;
                    escape = false;
                    pos += 1;
                    continue;
                }

                if (ch === "{") {
                    if (depth === 0) {
                        objectStart = pos;
                        if (indexInArray <= 5 || indexInArray % 1000 === 0) {
                            console.log("[streamParse] object start", {
                                objectIndex: indexInArray,
                                atPos: pos,
                            });
                        }
                    }
                    depth += 1;
                    pos += 1;
                    continue;
                }

                if (ch === "}") {
                    depth -= 1;
                    pos += 1;

                    if (depth === 0 && objectStart >= 0) {
                        flushObject(pos);      // pos уже после '}'
                        safePos = pos;         // всё до pos можно выбросить
                    }
                    continue;
                }

                if (ch === "]" && depth === 0) {
                    finishedArray = true;
                    pos += 1;
                    safePos = pos;
                    console.log("[streamParse] found closing ']'", {
                        atPos: pos - 1,
                        totalObjects: indexInArray,
                    });
                    continue;
                }

                // всё остальное (запятые, пробелы и т.п.)
                pos += 1;
            } else {
                // внутри строки
                if (escape) {
                    escape = false;
                } else if (ch === "\\") {
                    escape = true;
                } else if (ch === '"') {
                    inString = false;
                }
                pos += 1;
            }
        }

        // если есть что выбросить из начала буфера
        if (safePos > 0) {
            const oldLen = buffer.length;

            buffer = buffer.slice(safePos);

            // сдвигаем позиции
            pos -= safePos;
            if (pos < 0) pos = 0;

            if (objectStart >= 0) {
                objectStart -= safePos;
                if (objectStart < 0) {
                    // теоретически не должно случиться, но на всякий пожарный
                    objectStart = -1;
                }
            }

            console.log("[streamParse] slice buffer", {
                safePos,
                oldLen,
                newLen: buffer.length,
                finishedArray,
                depth,
                inString,
            });

            safePos = 0;
        }
    };

    while (!hasFatalError) {
        const { done, value } = await reader.read();

        if (done) {
            const tail = decoder.decode();
            if (tail.length > 0) {
                buffer += tail;
            }

            console.log("[streamParse] reader done", {
                chunkIndex,
                totalBytes,
                bufferLength: buffer.length,
            });

            processBuffer();
            break;
        }

        chunkIndex += 1;
        totalBytes += value.byteLength;

        console.log("[streamParse] got chunk", {
            chunkIndex,
            chunkBytes: value.byteLength,
            totalBytes,
        });

        buffer += decoder.decode(value, { stream: true });

        processBuffer();
    }

    if (!hasFatalError) {
        if (!finishedArray) {
            console.error("[streamParse] no closing ']' for top-level array", {
                flushedObjects,
                indexInArray,
                depth,
                inString,
                bufferTail: buffer.slice(-200),
            });

            fail([
                makeFileLevelIssue(
                    "Не найдена закрывающая ']' для массива верхнего уровня",
                    buffer.slice(-80),
                ),
            ]);
        } else {
            console.log("[streamParse] finished OK", {
                flushedObjects,
                indexInArray,
                totalBytes,
                chunkIndex,
            });
        }
    } else {
        console.log("[streamParse] finished with error", {
            flushedObjects,
            indexInArray,
            totalBytes,
            chunkIndex,
        });
    }
}
