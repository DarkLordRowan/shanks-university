export function parseCsv(csvText: string): { headers: string[]; rows: string[][] } {
    const rows: string[][] = [];
    let row: string[] = [];
    let cell = "";
    let inQuotes = false;

    const pushCell = () => {
        row.push(cell);
        cell = "";
    };
    const pushRow = () => {
        rows.push(row);
        row = [];
    };

    for (let i = 0; i < csvText.length; i++) {
        const ch = csvText[i];
        if (inQuotes) {
            if (ch === '"') {
                if (csvText[i + 1] === '"') {
                    cell += '"';
                    i++;
                } else {
                    inQuotes = false;
                }
            } else {
                cell += ch;
            }
        } else {
            if (ch === '"') {
                inQuotes = true;
            } else if (ch === ",") {
                pushCell();
            } else if (ch === "\n") {
                pushCell();
                pushRow();
            } else if (ch === "\r") {
                // ignore
            } else {
                cell += ch;
            }
        }
    }
    pushCell();
    if (row.length > 1 || (row.length === 1 && row[0] !== "")) pushRow();

    if (!rows.length) return {headers: [], rows: []};
    const headers = rows[0];
    const data = rows.slice(1);
    return {headers, rows: data};
}
