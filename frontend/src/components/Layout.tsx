import { Outlet, Link, NavLink } from "react-router-dom";
import React from "react";

const nav = [
    { to: "/", label: "Home", end: true },
    { to: "/experiments", label: "Experiments" },
    { to: "/algorithms", label: "Algorithms" },
    { to: "/docs", label: "Docs" },
];

const Layout: React.FC = () => {
    return (
        <div className="flex min-h-screen flex-col">
            {/* Header */}
            <header className="sticky top-0 z-30 border-b border-border/60 bg-panel/90 backdrop-blur">
                <div className="container flex h-14 items-center justify-between">
                    <Link to="/" className="font-semibold tracking-wide text-accent">Shanks Univ</Link>

                    <nav className="flex gap-4 text-sm">
                        {nav.map(({ to, label, end }) => (
                            <NavLink
                                key={to}
                                to={to}
                                end={end}
                                className={({ isActive }) =>
                                    `px-2 py-1 rounded-lg ${
                                        isActive ? "bg-primary/20 text-primary" : "text-textDim hover:text-white"
                                    }`
                                }
                            >
                                {label}
                            </NavLink>
                        ))}
                    </nav>

                    <button className="btn">Save .json</button>
                </div>
            </header>

            {/* Main */}
            <main className="container flex-1 py-6">
                <Outlet />
            </main>

            {/* Footer */}
            <footer className="border-t border-border/60 bg-panel/70">
                <div className="container py-4 text-center text-sm text-textDim">
                    © {new Date().getFullYear()} Shanks Univ
                </div>
            </footer>
        </div>
    );
};

export default Layout;
