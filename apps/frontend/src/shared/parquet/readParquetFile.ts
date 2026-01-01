// src/shared/parquet/readParquetFile.ts

import initParquet, { readParquet } from "parquet-wasm";
import { tableFromIPC } from "apache-arrow";

// URL wasm из CDN, версия должна совпадать с package.json
const PARQUET_WASM_URL =
    "https://cdn.jsdelivr.net/npm/parquet-wasm@0.7.1/esm/parquet_wasm_bg.wasm";

// ленивый init
let parquetInit: Promise<unknown> | null = null;

async function ensureInit() {
    if (!parquetInit) {
        parquetInit = initParquet(PARQUET_WASM_URL);
    }
    return parquetInit;
}

export async function readParquetFile<T = any>(file: File): Promise<T[]> {
    await ensureInit();

    const buf = new Uint8Array(await file.arrayBuffer());

    // sync API: читаем весь parquet в wasm-таблицу
    const wasmTable: any = readParquet(buf);

    // Arrow IPC -> Arrow JS Table
    const arrowTable = tableFromIPC(wasmTable.intoIPCStream());

    const rows: T[] = [];

    const fieldNames = arrowTable.schema.fields.map((f) => f.name);
    const columns = fieldNames.map((name) => arrowTable.getChild(name));
    const rowCount = arrowTable.numRows;

    for (let r = 0; r < rowCount; r++) {
        const row: any = {};
        for (let c = 0; c < fieldNames.length; c++) {
            row[fieldNames[c]] = columns[c]?.get(r) ?? null;
        }
        rows.push(row as T);
    }

    return rows;
}
