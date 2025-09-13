import React from "react";
import { Link } from "react-router-dom";

const Home: React.FC = () => (
    <div className="grid gap-4 md:grid-cols-2">
        <section className="card">
            <h1 className="mb-2 text-2xl font-bold text-primary">
                Ускорение сходимости рядов
            </h1>
            <p className="text-textDim">
                Набор инструментов для численного ускорения сходимости медленно
                сходящихся числовых рядов и последовательностей.
            </p>
        </section>

        <section className="card">
            <h2 className="mb-2 text-xl font-semibold text-secondary">С чего начать</h2>
            <ul className="list-disc pl-5 space-y-1 text-textDim">
                <li>
                    Откройте <Link to="/algorithms" className="link">Алгоритмы</Link>, чтобы выбрать метод.
                </li>
                <li>
                    Перейдите в <Link to="/experiments" className="link">Эксперименты</Link>, чтобы
                    протестировать метод на своём ряду.
                </li>
                <li>
                    См. <Link to="/docs" className="link">Документацию</Link> с формулами и примерами.
                </li>
            </ul>
        </section>
    </div>
);

export default Home;
