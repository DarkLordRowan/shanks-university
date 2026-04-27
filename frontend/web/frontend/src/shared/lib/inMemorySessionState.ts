import { type Dispatch, type SetStateAction, useEffect, useRef, useState } from "react";

const SESSION_STATE_CACHE = new Map<string, unknown>();

function cloneWith<T>(value: T, clone?: (value: T) => T): T {
    return clone ? clone(value) : value;
}

function resolveInitialValue<T>(initialValue: T | (() => T)): T {
    return typeof initialValue === "function"
        ? (initialValue as () => T)()
        : initialValue;
}

function readOrCreateInitialValue<T>(args: {
    key?: string | null;
    initialValue: T | (() => T);
    clone?: (value: T) => T;
}): T {
    const { key, initialValue, clone } = args;
    const cached = readInMemorySessionState<T>(key, clone);
    return cached !== undefined
        ? cached
        : cloneWith(resolveInitialValue(initialValue), clone);
}

export function readInMemorySessionState<T>(
    key?: string | null,
    clone?: (value: T) => T
): T | undefined {
    if (!key) return undefined;
    if (!SESSION_STATE_CACHE.has(key)) return undefined;
    return cloneWith(SESSION_STATE_CACHE.get(key) as T, clone);
}

export function writeInMemorySessionState<T>(
    key: string | null | undefined,
    value: T,
    clone?: (value: T) => T
): void {
    if (!key) return;
    SESSION_STATE_CACHE.set(key, cloneWith(value, clone));
}

export function clearInMemorySessionState(key?: string | null): void {
    if (!key) return;
    SESSION_STATE_CACHE.delete(key);
}

export function resetInMemorySessionStateForTests(): void {
    SESSION_STATE_CACHE.clear();
}

export function buildExperimentSessionStateKey(
    experimentId: string | number,
    namespace: string,
    version = "v1"
): string {
    return `exp-session:${version}:${String(experimentId)}:${namespace}`;
}

export function useInMemorySessionState<T>(args: {
    key?: string | null;
    initialValue: T | (() => T);
    clone?: (value: T) => T;
}): readonly [T, Dispatch<SetStateAction<T>>] {
    const { key, initialValue, clone } = args;
    const [state, setState] = useState<T>(() =>
        readOrCreateInitialValue({ key, initialValue, clone })
    );
    const prevKeyRef = useRef<string | null | undefined>(key);

    useEffect(() => {
        if (prevKeyRef.current === key) return;
        prevKeyRef.current = key;
        setState(readOrCreateInitialValue({ key, initialValue, clone }));
    }, [key, initialValue, clone]);

    useEffect(() => {
        writeInMemorySessionState(key, state, clone);
    }, [key, state, clone]);

    return [state, setState] as const;
}
