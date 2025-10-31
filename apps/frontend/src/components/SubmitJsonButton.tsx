import React from "react";
import { useApiProcessing } from "../pages/Experiments/hooks/useApiProcessing";

type Props = {
    jsonString: string | null;
    label?: string;
    onDone?: (ok: boolean) => void; // опционально: оповещение об успехе/ошибке
};

export const SubmitJsonButton: React.FC<Props> = ({ jsonString, label = "Отправить", onDone }) => {
    const {
        state: { isSending },
        actions: { sendForJson },
    } = useApiProcessing();

    const disabled = isSending || !jsonString;

    const handleClick = async () => {
        if (!jsonString) return;
        let payload: unknown;
        try {
            payload = JSON.parse(jsonString);
        } catch {
            onDone?.(false);
            return;
        }
        const { jsonOk } = await sendForJson(payload as object);
        onDone?.(!!jsonOk);
    };

    return (
        <button
            type="button"
            className="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-black hover:opacity-90 disabled:opacity-60"
            disabled={disabled}
            onClick={handleClick}
        >
            {isSending ? "Отправка…" : label}
        </button>
    );
};
