import React from "react";

const Home: React.FC = () => (
    <div className="grid gap-4 md:grid-cols-2">
        <div className="card">
            <h1 className="mb-2 text-2xl font-bold text-primary">Home</h1>
            <p className="text-textDim">
                Welcome to the homepage. Use the navbar to explore pages.
            </p>
        </div>
    </div>
);
export default Home;
