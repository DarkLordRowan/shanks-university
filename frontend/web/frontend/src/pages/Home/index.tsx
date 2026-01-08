import React from "react";
import { Link } from "react-router-dom";

const GITHUB_REPO = "https://github.com/DarkLordRowan/shanks-university";
const BRANCH = "main";

type Feature = {
    title: string;
    description: string;
    bullets: string[];
    to: string;
    toneClass: string;
    icon: React.ReactNode;
};

type RepoItem = {
    path: string;
    kind: "dir" | "file";
    href: string;
    description: string;
};

type Manual = {
    title: string;
    description: string;
    bullets: string[];
    to: string;
    toneClass: string;
    icon: React.ReactNode;
};

function IconSliders(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M4 21v-7m0-4V3m10 18v-9m0-4V3m6 18v-5m0-4V3"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
            />
            <path
                d="M2 14h4m6-6h4m6 8h4"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
            />
        </svg>
    );
}

function IconChart(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path d="M4 19V5" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
            <path d="M4 19h16" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
            <path
                d="M7 15l3-4 3 2 4-6"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
                strokeLinejoin="round"
            />
            <path d="M18 7h2v2" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
        </svg>
    );
}

function IconBook(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M6 4h10a2 2 0 0 1 2 2v14a2 2 0 0 0-2-2H6a2 2 0 0 0-2 2V6a2 2 0 0 1 2-2Z"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinejoin="round"
            />
            <path d="M8 8h8" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
            <path d="M8 12h6" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
        </svg>
    );
}

function IconFolder(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M3 7a2 2 0 0 1 2-2h5l2 2h7a2 2 0 0 1 2 2v8a3 3 0 0 1-3 3H6a3 3 0 0 1-3-3V7Z"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinejoin="round"
            />
        </svg>
    );
}

function IconFile(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M7 3h7l3 3v15a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2Z"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinejoin="round"
            />
            <path d="M14 3v4h4" stroke="currentColor" strokeWidth="2" strokeLinejoin="round" />
        </svg>
    );
}

function IconUser(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M20 21a8 8 0 1 0-16 0"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
            />
            <path
                d="M12 13a4 4 0 1 0-4-4 4 4 0 0 0 4 4Z"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinejoin="round"
            />
        </svg>
    );
}

function IconBeaker(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M10 2h4"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
            />
            <path
                d="M10 2v5l-5.5 9.5A4 4 0 0 0 8 22h8a4 4 0 0 0 3.5-5.5L14 7V2"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinejoin="round"
            />
            <path
                d="M8 14h8"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
            />
        </svg>
    );
}

function IconCode(props: { className?: string }) {
    return (
        <svg className={props.className} viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path
                d="M9 18 3 12l6-6"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
                strokeLinejoin="round"
            />
            <path
                d="M15 6l6 6-6 6"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
                strokeLinejoin="round"
            />
        </svg>
    );
}

const features: Feature[] = [
    {
        title: "Генератор конфигурации эксперимента",
        description: "Формирование постановки эксперимента (ряды, алгоритмы, параметры, точности).",
        bullets: [
            "выбор набора рядов и ускорителей (алгоритмов)",
            "выбор точностей",
            "TODO: сохранение/переиспользование конфигураций",
        ],
        to: "/configurations",
        toneClass: "text-primary",
        icon: <IconSliders className="h-6 w-6" />,
    },
    {
        title: "Визуальный анализ эксперимента",
        description:
            "Анализ результатов вычислений с помощью таблиц и графиков с возможностью экспорта визуализаций в jpeg и xlsx.",
        bullets: ["таблицы ошибок", "графики сходимости и отклонений", "сравнение рядов и алгоритмов"],
        to: "/experiments",
        toneClass: "text-accent",
        icon: <IconChart className="h-6 w-6" />,
    },
    {
        title: "Документация",
        description: "TODO: придумать описание",
        bullets: ["TODO: придумать что тут должно быть"],
        to: "/docs",
        toneClass: "text-secondary",
        icon: <IconBook className="h-6 w-6" />,
    },
];

const repoItems: RepoItem[] = [
    {
        path: "backend/",
        kind: "dir",
        href: `${GITHUB_REPO}/tree/${BRANCH}/backend`,
        description: "Core часть: c++ библиотека и python фреймворк",
    },
    {
        path: "frontend/",
        kind: "dir",
        href: `${GITHUB_REPO}/tree/${BRANCH}/frontend`,
        description: "Клиентская часть: вычислительный клиент, десктопный визуализатор, веб-интерфейс",
    },
    {
        path: "theory/",
        kind: "dir",
        href: `${GITHUB_REPO}/tree/${BRANCH}/theory`,
        description: "Теория: теоретическое обоснование корректности рядов и ускорителей (алгоритмов)",
    },
    {
        path: "docker-compose.yml",
        kind: "file",
        href: `${GITHUB_REPO}/blob/${BRANCH}/docker-compose.yml`,
        description: "Локальный запуск сервисов.",
    },
    {
        path: "flake.nix",
        kind: "file",
        href: `${GITHUB_REPO}/blob/${BRANCH}/flake.nix`,
        description: "Воспроизведение окружения для разработки.",
    },
    {
        path: "README.md",
        kind: "file",
        href: `${GITHUB_REPO}/blob/${BRANCH}/README.md`,
        description: "Вводное описание проекта и ссылки.",
    },
];

const manuals: Manual[] = [
    {
        title: "Мануал пользователя",
        description:
            "Работа с конфигурациями экспериментов, запуск, просмотр результатов, экспорт таблиц и графиков.",
        bullets: ["создание конфигурации", "запуск/загрузка эксперимента", "интерпретация таблиц и графиков"],
        to: "/docs/manuals/user",
        toneClass: "text-primary",
        icon: <IconUser className="h-6 w-6" />,
    },
    {
        title: "Мануал теоретика-аналитика",
        description:
            "Описание исследуемых рядов, пределов, ускорителей и метрик сравнения. Связь теории и эксперимента.",
        bullets: ["модели рядов и параметров", "алгоритмы ускорения", "критерии качества и ограничения"],
        to: "/docs/manuals/analyst",
        toneClass: "text-secondary",
        icon: <IconBeaker className="h-6 w-6" />,
    },
    {
        title: "Мануал разработчика",
        description:
            "Архитектура репозитория, контракты данных, локальный запуск, сборка, CI и принципы расширения.",
        bullets: ["структура backend/frontend/theory", "форматы данных и типы", "сборка и окружение"],
        to: "/docs/manuals/developer",
        toneClass: "text-accent",
        icon: <IconCode className="h-6 w-6" />,
    },
];

function FeatureCard(p: Feature) {
    return (
        <div className="rounded-xl2 border border-border bg-panel p-4 shadow-panel">
            <div className="flex items-start gap-3">
                <div
                    className={[
                        "mt-0.5 inline-flex h-10 w-10 items-center justify-center rounded-xl2 border border-border bg-surface",
                        p.toneClass,
                    ].join(" ")}
                >
                    {p.icon}
                </div>

                <div className="min-w-0 flex-1">
                    <div className="flex items-center justify-between gap-3">
                        <h3 className="text-base font-semibold text-white">{p.title}</h3>
                        <span className="shrink-0 rounded-full border border-border bg-surface px-2 py-0.5 text-xs text-textDim">
                            UI
                        </span>
                    </div>

                    <p className="mt-1 text-sm leading-relaxed text-textDim">{p.description}</p>

                    <ul className="mt-3 space-y-1 text-sm text-textDim">
                        {p.bullets.map((b) => (
                            <li key={b} className="flex gap-2">
                                <span className="mt-2 h-1.5 w-1.5 shrink-0 rounded-full bg-border" />
                                <span className="min-w-0">{b}</span>
                            </li>
                        ))}
                    </ul>

                    <div className="mt-4">
                        <Link
                            to={p.to}
                            className="inline-flex items-center gap-2 rounded-xl2 border border-border bg-surface px-3 py-2 text-sm font-medium text-white hover:border-primary/60"
                        >
                            Открыть
                            <span className="text-textDim">→</span>
                        </Link>
                    </div>
                </div>
            </div>
        </div>
    );
}

function RepoRow(p: RepoItem) {
    const Icon = p.kind === "dir" ? IconFolder : IconFile;

    return (
        <a
            href={p.href}
            target="_blank"
            rel="noreferrer"
            className="flex items-start gap-3 rounded-xl2 border border-border bg-panel p-3 shadow-panel hover:border-primary/60"
        >
            <span className="mt-0.5 text-textDim">
                <Icon className="h-5 w-5" />
            </span>

            <span className="min-w-0 flex-1">
                <span className="flex items-center gap-2">
                    <span className="font-mono text-sm text-white">{p.path}</span>
                    <span className="rounded-full border border-border bg-surface px-2 py-0.5 text-xs text-textDim">
                        {p.kind === "dir" ? "dir" : "file"}
                    </span>
                </span>
                <span className="mt-1 block text-sm text-textDim">{p.description}</span>
            </span>

            <span className="mt-1 shrink-0 text-textDim">↗</span>
        </a>
    );
}

function ManualCard(p: Manual) {
    return (
        <div className="rounded-xl2 border border-border bg-panel p-4 shadow-panel">
            <div className="flex items-start gap-3">
                <div
                    className={[
                        "mt-0.5 inline-flex h-10 w-10 items-center justify-center rounded-xl2 border border-border bg-surface",
                        p.toneClass,
                    ].join(" ")}
                >
                    {p.icon}
                </div>

                <div className="min-w-0 flex-1">
                    <div className="flex items-center justify-between gap-3">
                        <h3 className="text-base font-semibold text-white">{p.title}</h3>
                        <span className="shrink-0 rounded-full border border-border bg-surface px-2 py-0.5 text-xs text-textDim">
                            DOC
                        </span>
                    </div>

                    <p className="mt-1 text-sm leading-relaxed text-textDim">{p.description}</p>

                    <ul className="mt-3 space-y-1 text-sm text-textDim">
                        {p.bullets.map((b) => (
                            <li key={b} className="flex gap-2">
                                <span className="mt-2 h-1.5 w-1.5 shrink-0 rounded-full bg-border" />
                                <span className="min-w-0">{b}</span>
                            </li>
                        ))}
                    </ul>

                    <div className="mt-4">
                        <Link
                            to={p.to}
                            className="inline-flex items-center gap-2 rounded-xl2 border border-border bg-surface px-3 py-2 text-sm font-medium text-white hover:border-primary/60"
                        >
                            Читать мануал
                            <span className="text-textDim">→</span>
                        </Link>
                    </div>
                </div>
            </div>
        </div>
    );
}

const Home: React.FC = () => {
    return (
        <div className="rounded-xl2 border border-border bg-panel p-6 shadow-panel text-textDim">
            <div className="space-y-2">
                <div className="flex flex-wrap items-center justify-between gap-3">
                    <h1 className="text-3xl font-bold text-white">Ускорение сходимости рядов</h1>

                    <a
                        href={`${GITHUB_REPO}/tree/${BRANCH}`}
                        target="_blank"
                        rel="noreferrer"
                        className="inline-flex items-center gap-2 rounded-xl2 border border-border bg-surface px-3 py-2 text-sm font-medium text-white hover:border-primary/60"
                    >
                        GitHub
                        <span className="rounded-full border border-border bg-panel px-2 py-0.5 text-xs text-textDim">
                            ветка {BRANCH}
                        </span>
                    </a>
                </div>

                <p className="max-w-4xl text-sm leading-relaxed text-textDim">
                    Интерфейс для постановки вычислительных экспериментов по ускорению сходимости числовых рядов,
                    анализа результатов и просмотра сопутствующей документации.
                </p>
            </div>

            <div className="mt-6 grid gap-6 lg:grid-cols-2">
                <section className="space-y-4">
                    <div className="rounded-xl2 border border-border bg-surface p-4">
                        <h2 className="text-lg font-semibold text-white">Функции сайта</h2>
                        <p className="mt-1 text-sm text-textDim">
                            Три основных сценария работы: конфигурация, анализ, документация.
                        </p>
                    </div>

                    <div className="space-y-4">
                        {features.map((f) => (
                            <FeatureCard key={f.title} {...f} />
                        ))}
                    </div>
                </section>

                <section className="space-y-4">
                    <div className="rounded-xl2 border border-border bg-surface p-4">
                        <h2 className="text-lg font-semibold text-white">Структура проекта (GitHub)</h2>
                        <p className="mt-1 text-sm text-textDim">
                            Репозиторий организован по модулям: backend/frontend/theory + инфраструктура.
                        </p>
                    </div>

                    <div className="rounded-xl2 border border-border bg-panel p-4 shadow-panel">
                        <div className="text-xs text-textDim">Дерево верхнего уровня</div>
                        <pre className="mt-2 overflow-x-auto rounded-xl2 border border-border bg-surface p-3 font-mono text-xs text-textDim">
{`shanks-university/
├─ backend/
├─ frontend/
├─ theory/
├─ docker-compose.yml
├─ flake.nix
└─ README.md`}
                        </pre>
                    </div>

                    <div className="space-y-3">
                        {repoItems.map((x) => (
                            <RepoRow key={x.path} {...x} />
                        ))}
                    </div>
                </section>
            </div>

            <div className="mt-6 space-y-4">
                <div className="rounded-xl2 border border-border bg-surface p-4">
                    <h2 className="text-lg font-semibold text-white">Мануалы</h2>
                    <p className="mt-1 text-sm text-textDim">
                        Быстрые входные точки по ролям. Ссылки ведут на страницы документации (пока заглушки).
                    </p>
                </div>

                <div className="grid gap-4 lg:grid-cols-3">
                    {manuals.map((m) => (
                        <ManualCard key={m.title} {...m} />
                    ))}
                </div>
            </div>
        </div>
    );
};

export default Home;
