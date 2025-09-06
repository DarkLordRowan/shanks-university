import React from "react";

const items = [
    {
        title: "Δ² Эйткена (Aitken)",
        note: "Ускоряет линейную сходимость последовательностей s_n.",
    },
    {
        title: "Преобразование Шэнкса",
        note: "Рациональные аппроксимации; частный случай ε-алгоритма.",
    },
    {
        title: "Алгоритм ε-Винна",
        note: "Итеративная таблица для построения рациональных сумматоров.",
    },
    {
        title: "Преобразование Левина (u)",
        note: "Эффективно при степенных остатках; требует оценок остатков.",
    },
    {
        title: "Преобразование Эйлера",
        note: "Для знакопеременных рядов; сглаживает колебания.",
    },
    {
        title: "Экстраполяция Ричардсона",
        note: "Убирает ведущий член погрешности с известным порядком.",
    },
];

const Algorithms: React.FC = () => (
    <div className="card">
        <h1 className="mb-3 text-2xl font-bold text-primary">Алгоритмы</h1>
        <ul className="divide-y divide-border/50">
            {items.map((x) => (
                <li key={x.title} className="py-3">
                    <div className="font-semibold">{x.title}</div>
                    <div className="text-sm text-textDim">{x.note}</div>
                </li>
            ))}
        </ul>
    </div>
);

export default Algorithms;
