import React, { useMemo, useRef, useState } from "react";

export function useJsonInput() {
    const [rawText, setRawText] = useState("");
    const [fileName, setFileName] = useState("");
    const [jsonPayload, setJsonPayload] = useState<any | null>(null);
    const fileInputRef = useRef<HTMLInputElement | null>(null);

    const prettyPayload = useMemo(() => (jsonPayload ? JSON.stringify(jsonPayload, null, 2) : ""), [jsonPayload]);

    const handleFile = async (file: File, onError: (msg: string) => void) => {
        try {
            const text = await file.text();
            setRawText(text);
            setFileName(file.name);
            setJsonPayload(JSON.parse(text));
            onError(""); // сброс ошибок
        } catch {
            onError("Не удалось прочитать/распарсить JSON-файл.");
            setJsonPayload(null);
        }
    };

    const onFileChange: React.ChangeEventHandler<HTMLInputElement> = async (e) => {
        const f = e.target.files?.[0];
        if (!f) return;
        await handleFile(f, () => {});
    };

    const onDrop: React.DragEventHandler<HTMLDivElement> = async (e) => {
        e.preventDefault();
        if (!e.dataTransfer.files?.length) return;
        await handleFile(e.dataTransfer.files[0], () => {});
    };

    const applyPasted = (onError: (msg: string) => void) => {
        try {
            setJsonPayload(JSON.parse(rawText));
            onError("");
            setFileName("");
            if (fileInputRef.current) fileInputRef.current.value = "";
        } catch {
            onError("Вставленный текст не является корректным JSON.");
            setJsonPayload(null);
        }
    };

    const reset = () => {
        setRawText("");
        setFileName("");
        setJsonPayload(null);
        if (fileInputRef.current) fileInputRef.current.value = "";
    };

    return {
        state: { rawText, fileName, jsonPayload, prettyPayload },
        actions: { setRawText, onFileChange, onDrop, applyPasted, reset },
        refs: { fileInputRef },
    };
}
