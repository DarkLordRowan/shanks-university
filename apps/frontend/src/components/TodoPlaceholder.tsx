import React from "react";
import { AlertTriangle } from "lucide-react";

export const TodoPlaceholder: React.FC<{ text?: string }> = ({ text = "// TODO" }) => (
    <div className="flex flex-col items-center justify-center gap-3 rounded-2xl border-2 border-dashed border-border/60 bg-panel/70 p-10 text-center">
        <AlertTriangle className="h-10 w-10 text-yellow-400" />
        <div className="text-xl font-mono text-yellow-400">{text}</div>
        <p className="text-sm text-textDim">Эта страница ещё в разработке</p>
    </div>
);
