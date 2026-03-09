import React, { useState } from "react";
import { Check, Copy } from "lucide-react";

type Props = {
    value: string;
    size?: number;
    label?: string;
    successLabel?: string;
    className?: string;
};

export const CopyToClipboard: React.FC<Props> = (
    {
        value,
        size = 14,
        label = "Копировать",
        successLabel = "Скопировано",
        className = "",
    }) => {
    const [copied, setCopied] = useState(false);

    const handleCopy = async () => {
        try {
            await navigator.clipboard.writeText(value);
            setCopied(true);
            setTimeout(() => setCopied(false), 1500);
        } catch {
            setCopied(false);
        }
    };

    return (
        <button
            onClick={handleCopy}
            className={`flex items-center rounded-md border border-border/60 bg-background/60 px-2 py-1 text-xs hover:bg-panel transition-colors ${className}`}
        >
            {copied ? (
                <>
                    <Check size={size} className="mr-1 text-green-400"/>
                    {successLabel}
                </>
            ) : (
                <>
                    <Copy size={size} className="mr-1"/>
                    {label}
                </>
            )}
        </button>
    );
};
