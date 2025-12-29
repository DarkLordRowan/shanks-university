// src/shared/parquet/readParquetFile.ts

import initParquet, { readParquet } from "parquet-wasm";
import { tableFromIPC } from "apache-arrow";

// Версия должна совпадать с parquet-wasm в package.json.
const PARQUET_WASM_URL = "https://cdn.jsdelivr.net/npm/parquet-wasm@0.7.1/esm/parquet_wasm_bg.wasm";

// ленивый init (один раз на вкладку)
let parquetInitPromise: Promise<unknown> | null = null;

async function ensureInit(): Promise<void> {
    if (!parquetInitPromise) {
        parquetInitPromise = initParquet(PARQUET_WASM_URL);
    }
    await parquetInitPromise;
}

export type ParquetReadOptions = {
    // Column projection. ВАЖНО: для nested колонок (computed/errors/events) может ломать данные.
    columns?: string[];

    batchSize?: number;
    rowGroups?: number[];
    limit?: number;
    offset?: number;
};

export async function readParquetFile<T extends object = Record<string, unknown>>(
    file: File,
    options?: ParquetReadOptions
): Promise<T[]> {
    await ensureInit();

    const parquetBytes = new Uint8Array(await file.arrayBuffer());

    const arrowWasmTable = readParquet(parquetBytes, {
        columns: options?.columns,
        batchSize: options?.batchSize,
        rowGroups: options?.rowGroups,
        limit: options?.limit,
        offset: options?.offset,
    });

    const arrowTable = tableFromIPC(arrowWasmTable.intoIPCStream());

    const fieldNames =
        options?.columns && options.columns.length > 0
            ? options.columns
            : arrowTable.schema.fields.map((f) => f.name);

    const vectors = fieldNames.map((name) => arrowTable.getChild(name));
    const rowCount = arrowTable.numRows;

    const rows = new Array<T>(rowCount);

    for (let r = 0; r < rowCount; r++) {
        const row: Record<string, unknown> = {};
        for (let c = 0; c < fieldNames.length; c++) {
            row[fieldNames[c]] = vectors[c]?.get(r) ?? null;
        }
        rows[r] = row as T;
    }

    return rows;
}
