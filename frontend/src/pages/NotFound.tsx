import React from "react";
import { Link } from "react-router-dom";

const NotFound: React.FC = () => (
    <div className="card text-center">
        <h1 className="mb-3 text-3xl font-bold text-red-400">404 — Not Found</h1>
        <p className="mb-4 text-textDim">The page you’re looking for does not exist.</p>
        <Link to="/" className="btn">Go Home</Link>
    </div>
);
export default NotFound;
