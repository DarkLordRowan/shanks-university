import React, { useState } from "react";

/**
 * Минимальная «песочница»: ввод частичных сумм или членов ряда (через запятую),
 * выбор метода (пока только селект без реализации) и превью первых значений.
 */
const methods = [
    { id: "aitken", name: "Δ² Эйткена (Aitken)" },
    { id: "shanks", name: "Преобразование Шэнкса" },
    { id: "wynn-eps", name: "Алгоритм ε-Винна" },
    { id: "levin-u", name: "Преобразование Левина (u)" },
    { id: "euler", name: "Преобразование Эйлера (альтерн. ряды)" },
    { id: "richardson", name: "Экстраполяция Ричардсона" },
];

const Experiments: React.FC = () => {
    const [input, setInput] = useState<string>("1, 1/2, 1/3, 1/4");
    const [method, setMethod] = useState<string>("aitken");

    const tokens = input
        .split(/[,\s]+/)
        .filter(Boolean)
        .map((t) => t.replace(",", "."))
        .slice(0, 12);

    return (
        <div className="card">
            <h1 className="mb-3 text-2xl font-bold text-primary">Эксперименты</h1>

            <div className="grid gap-4 md:grid-cols-3">
                <div className="md:col-span-2">
                    <label className="mb-1 block text-sm text-textDim">
                        Члены ряда или частичные суммы (через запятую)
                    </label>
                    <textarea
                        className="w-full rounded-xl2 border border-border/60 bg-panel/70 p-3 outline-none focus:ring-2 focus:ring-primary/40"
                        rows={4}
                        value={input}
                        onChange={(e) => setInput(e.target.value)}
                    />
                </div>

                <div className="space-y-2">
                    <label className="mb-1 block text-sm text-textDim">Метод</label>
                    <select
                        className="w-full rounded-xl2 border border-border/60 bg-panel/70 p-2 outline-none focus:ring-2 focus:ring-primary/40"
                        value={method}
                        onChange={(e) => setMethod(e.target.value)}
                    >
                        {methods.map((m) => (
                            <option key={m.id} value={m.id}>
                                {m.name}
                            </option>
                        ))}
                    </select>

                    <button className="btn w-full">Применить метод</button>
                    <p className="text-xs text-textDim">
                        *Здесь только UI-«скелет». Реализацию вычислений добавим позже.
                    </p>
                </div>
            </div>

            <div className="mt-6">
                <h2 className="mb-2 text-lg font-semibold text-secondary">Предпросмотр входных данных</h2>
                <div className="rounded-xl2 border border-border/60 bg-panel/70 p-3 text-sm text-textDim">
                    {tokens.length ? tokens.join(", ") : "Нет данных"}
                </div>
            </div>
        </div>
    );
};

export default Experiments;
