export type ValidationIssue = {
    path: string;
    code: string;
    message: string;
    rawPath: PropertyKey[];
};

export type ValidationResultOk<T> = {
    ok: true;
    data: T;
};

export type ValidationResultErr = {
    ok: false;
    issues: ValidationIssue[];
};

export type ValidationResult<T> =
    | ValidationResultOk<T>
    | ValidationResultErr;
