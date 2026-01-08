import React, { useEffect, useState } from "react";
import { Link, NavLink, Outlet, useLocation } from "react-router-dom";

const nav = [
    { to: "/", label: "Главная", end: true },
    { to: "/configurations", label: "Конфигурации" },
    { to: "/experiments", label: "Эксперименты" },
    { to: "/docs", label: "Документация" },
];

const Layout: React.FC = () => {
    const [mobileOpen, setMobileOpen] = useState(false);
    const location = useLocation();

    useEffect(() => {
        setMobileOpen(false);
    }, [location.pathname]);

    useEffect(() => {
        const onKeyDown = (e: KeyboardEvent) => {
            if (e.key === "Escape") setMobileOpen(false);
        };
        window.addEventListener("keydown", onKeyDown);
        return () => window.removeEventListener("keydown", onKeyDown);
    }, []);

    return (
        <div className="flex min-h-screen flex-col">
            {/* Header */}
            <header className="sticky top-0 z-40 border-b border-border/60 bg-panel/90 backdrop-blur">
                <div className="container">
                    <div className="flex h-14 items-center justify-between gap-3">
                        <Link
                            to="/"
                            className="min-w-0 truncate font-semibold tracking-wide text-accent"
                            aria-label="Series Accelerator: главная"
                        >
                            Series Accelerator
                        </Link>

                        {/* Desktop nav */}
                        <nav className="hidden md:flex gap-4 text-sm">
                            {nav.map(({ to, label, end }) => (
                                <NavLink
                                    key={to}
                                    to={to}
                                    end={end}
                                    className={({ isActive }) =>
                                        `px-2 py-1 rounded-lg ${
                                            isActive
                                                ? "bg-primary/20 text-primary"
                                                : "text-textDim hover:text-white"
                                        }`
                                    }
                                >
                                    {label}
                                </NavLink>
                            ))}
                        </nav>

                        {/* Mobile menu button */}
                        <button
                            type="button"
                            className="md:hidden inline-flex items-center justify-center rounded-lg border border-border bg-surface px-3 py-2 text-sm text-textDim hover:text-white hover:border-primary/60"
                            aria-label="Открыть меню"
                            aria-expanded={mobileOpen}
                            onClick={() => setMobileOpen((v) => !v)}
                        >
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" aria-hidden="true">
                                <path
                                    d="M4 7h16M4 12h16M4 17h16"
                                    stroke="currentColor"
                                    strokeWidth="2"
                                    strokeLinecap="round"
                                />
                            </svg>
                        </button>
                    </div>

                    {/* Mobile dropdown */}
                    <div
                        className={[
                            "md:hidden overflow-hidden transition-[max-height,opacity] duration-200",
                            mobileOpen ? "max-h-96 opacity-100" : "max-h-0 opacity-0",
                        ].join(" ")}
                    >
                        <div className="border-t border-border/60 py-2">
                            <nav className="grid gap-1 text-sm">
                                {nav.map(({ to, label, end }) => (
                                    <NavLink
                                        key={to}
                                        to={to}
                                        end={end}
                                        className={({ isActive }) =>
                                            `px-3 py-2 rounded-lg ${
                                                isActive
                                                    ? "bg-primary/20 text-primary"
                                                    : "text-textDim hover:text-white hover:bg-surface/60"
                                            }`
                                        }
                                    >
                                        {label}
                                    </NavLink>
                                ))}
                            </nav>
                        </div>
                    </div>
                </div>
            </header>

            {/* Main */}
            <main className="container flex-1 py-6">
                <Outlet />
            </main>

            {/* Footer */}
            <footer className="border-t border-border/60 bg-panel/70">
                <div className="container py-4 text-center text-sm text-textDim">
                    © {new Date().getFullYear()} Acceleration of Series Convergence
                </div>
            </footer>
        </div>
    );
};

export default Layout;
