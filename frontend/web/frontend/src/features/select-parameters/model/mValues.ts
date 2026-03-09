export const MIN_M = 0;
export const MAX_M = 100;
export const M_STEP = 2;

export function getAllowedMValues(): number[] {
    return Array.from({ length: MAX_M / M_STEP + 1 }, (_, idx) => idx * M_STEP);
}

export function isValidMValue(value: number): boolean {
    return Number.isInteger(value) && value >= MIN_M && value <= MAX_M && value % M_STEP === 0;
}

export function toggleMValue(values: number[], next: number): number[] {
    if (!isValidMValue(next)) {
        return values;
    }

    if (values.includes(next)) {
        return values.filter((value) => value !== next);
    }

    return [...values, next].sort((a, b) => a - b);
}
