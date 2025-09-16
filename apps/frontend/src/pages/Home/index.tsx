import React from "react";
import { Link } from "react-router-dom";

const Home: React.FC = () => (
    <div className="card space-y-3">
        <h1 className="text-2xl font-bold text-primary">Ускорение сходимости рядов</h1>

        <p className="text-textDim">Пример использования.</p>

        <p className="text-textDim">
            Зайдите на страницу{" "}
            <Link to="/experiments" className="link">
                эксперементов
            </Link>
            ,
        </p>

        <p className="text-textDim">
            Вставьте файл из (
            <a
                href="https://github.com/DarkLordRowan/shanks-university/blob/Fixes/shanks_consumer/data/one_series_all_algo.json"
                target="_blank"
                rel="noreferrer"
                className="link"
            >
                https://github.com/DarkLordRowan/shanks-university/blob/Fixes/shanks_consumer/data/one_series_all_algo.json
            </a>
            )
        </p>

        <p className="text-textDim">И запустите эксперемент</p>
    </div>
);

export default Home;
