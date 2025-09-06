import React from "react";

const Docs: React.FC = () => (
    <div className="card">
        <h1 className="mb-3 text-2xl font-bold text-primary">Документация</h1>
        <ol className="list-decimal pl-5 space-y-2 text-textDim">
            <li>
                Постановка задачи: дана последовательность частичных сумм или членов ряда —
                требуется оценить предел/сумму быстрее.
            </li>
            <li>
                Выбор метода: см. раздел <span className="text-white">Алгоритмы</span>.
            </li>
            <li>
                Практика: используйте страницу <span className="text-white">Эксперименты</span> для тестов.
            </li>
        </ol>
    </div>
);

export default Docs;
