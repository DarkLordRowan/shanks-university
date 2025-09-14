import React from "react";
import { Link } from "react-router-dom";

const NotFound: React.FC = () => (
    <div className="card text-center">
        <h1 className="mb-3 text-3xl font-bold text-red-400">404 — Страница не найдена</h1>
        <p className="mb-4 text-textDim">Вернитесь на главную и попробуйте снова.</p>
        <Link to="/" className="btn">На главную</Link>
    </div>
);

export default NotFound;
