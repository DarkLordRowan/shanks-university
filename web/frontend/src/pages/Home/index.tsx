import { Link } from "react-router-dom";

const Home: React.FC = () => {
    return (
        <div className="rounded-xl2 border border-border bg-panel p-6 shadow-panel space-y-5 text-textDim">
            <h1 className="text-3xl font-bold text-white">Ускорение сходимости рядов</h1>

            <p>
                Данный интерфейс позволяет выполнять вычислительные эксперименты по ускорению
                сходимости числовых рядов и визуализировать результаты.
            </p>

            <div className="space-y-1">
                <p className="font-semibold text-white">Глобальный эксперимент</p>
                <Link to="/experiments" className="text-primary hover:underline underline-offset-4">
                    Перейти к глобальному эксперименту
                </Link>

                <div className="space-y-1">
                    <p>Для тестирования можно использовать пример входных данных:</p>
                    <a
                        href="https://github.com/DarkLordRowan/shanks-university/blob/VecImpl/consumer/data/one_series_all_algo.json"
                        target="_blank"
                        rel="noreferrer"
                        className="text-primary hover:underline underline-offset-4 break-all text-sm"
                    >
                        one_series_all_algo.json
                    </a>
                </div>
            </div>

            <div className="space-y-2">
                <p className="font-semibold text-white">Эксперименты с графиками</p>

                <ul className="space-y-1 text-sm">
                    <li>
                        1) Один алгоритм, фиксированное m. Разные ряды —{" "}
                        <Link
                            to="/experiments_1"
                            className="text-primary hover:underline underline-offset-4"
                        >
                            открыть
                        </Link>
                    </li>
                    <li>
                        2) Один ряд, один алгоритм, одно m. Разные x —{" "}
                        <Link
                            to="/experiments_2"
                            className="text-primary hover:underline underline-offset-4"
                        >
                            открыть
                        </Link>
                    </li>
                    <li>
                        3) Один ряд, фиксированные x и m. Разные алгоритмы —{" "}
                        <Link
                            to="/experiments_3"
                            className="text-primary hover:underline underline-offset-4"
                        >
                            открыть
                        </Link>
                    </li>
                    <li>
                        4) Один ряд, один x, один алгоритм. Разные m —{" "}
                        <Link
                            to="/experiments_4"
                            className="text-primary hover:underline underline-offset-4"
                        >
                            открыть
                        </Link>
                    </li>
                </ul>
            </div>

            <p>Загрузите файл и выполните эксперимент.</p>
        </div>
    );
};

export default Home;
