import type { AlgNode } from "../types/algorithms";

// Дерево алгоритмов: используем только authorIds: AuthorId[]
export const TREE: AlgNode[] = [
    {
        id: "weniger_algorithm",
        title: "weniger_algorithm",
        subtitle: "Алгоритм Венигера (Преобразование Венигера)",
        document: "Weniger.pdf -> weniger_algorithm.pdf",
        authorIds: ["weniger"],
        children: [],
    },
    {
        id: "drummond_d_algorithm",
        title: "drummond_d_algorithm",
        subtitle: "Алгоритм Драммонда",
        authorIds: ["drummond"],
        children: [],
    },
    {
        id: "levin_algorithm",
        title: "levin_algorithm",
        subtitle: "Алгоритм Левина (Levin’s Transformation)",
        authorIds: ["levin"],
        children: [
            {
                id: "levin_basic_algorithm",
                title: "levin_basic_algorithm",
                subtitle: "Алгоритм Левина (базовый)",
                authorIds: ["levin"],
            },
            {
                id: "levin_recursion_algorithm",
                title: "levin_recursion_algorithm",
                subtitle: "Алгоритм Левина (рекурсивный)",
                authorIds: ["levin"],
            },
        ],
    },
    {
        id: "lubkin_w_algorithm",
        title: "W_lubkin_algorithm",
        subtitle: "Алгоритм Лубкина (W-алгоритм Лубкина)",
        authorIds: ["lubkin"],
        children: [],
    },
    {
        id: "richardson_algorithm",
        title: "richardson_algorithm",
        subtitle: "Алгоритм Ричардсона (Richardson Extrapolation)",
        authorIds: ["richardson"],
        children: [],
    },
    {
        id: "ford_sidi_algorithm",
        title: "ford_sidi_algorithm",
        subtitle: "Алгоритм Форд–Сиди (Ford–Sidi Algorithm, W_k)",
        authorIds: ["ford", "sidi"],
        children: [
            {
                id: "ford_sidi_one_algorithm",
                title: "ford_sidi_one_algorithm",
                subtitle: "первая версия",
                authorIds: ["ford", "sidi"]
            },
            {
                id: "ford_sidi_two_algorithm",
                title: "ford_sidi_two_algorithm",
                subtitle: "вторая версия",
                authorIds: ["ford", "sidi"]
            },
            {
                id: "ford_sidi_three_algorithm",
                title: "ford_sidi_three_algorithm",
                subtitle: "третья версия",
                authorIds: ["ford", "sidi"]
            },
        ],
    },
    {
        id: "chang_wynn_algorithm",
        title: "chang_wynn_algorithm",
        subtitle: "Алгоритм Чанг–Винна (Chang–Wynn Algorithm)",
        authorIds: ["chang", "wynn"],
        children: [],
    },
    {
        id: "shanks_transform",
        title: "shanks_transform",
        subtitle: "Алгоритм Шенкса (Shanks’ Transformation)",
        authorIds: ["shanks"],
        children: [
            {
                id: "shanks_transform_alternating",
                title: "shanks_transform_alternating",
                subtitle: "Шэнкс (знакопеременный)",
                authorIds: ["shanks"],
            },
        ],
    },
    {
        id: "wynn_epsilon_algorithm",
        title: "wynn_epsilon_algorithm",
        subtitle: "Эпсилон-алгоритм Винна (Wynn’s ε-algorithm)",
        authorIds: ["wynn"],
        children: [
            {
                id: "wynn_epsilon_one_algorithm",
                title: "wynn_epsilon_one_algorithm",
                subtitle: "базовый",
                authorIds: ["wynn"]
            },
            {
                id: "wynn_epsilon_two_algorithm",
                title: "wynn_epsilon_two_algorithm",
                subtitle: "вторая версия",
                authorIds: ["wynn"]
            },
            {
                id: "wynn_epsilon_three_algorithm",
                title: "wynn_epsilon_three_algorithm",
                subtitle: "третья версия",
                authorIds: ["wynn"]
            },
        ],
    },
    {
        id: "wynn_rho_algorithm",
        title: "wynn_rho_algorithm",
        subtitle: "Ро-алгоритм Винна (Wynn’s ρ-algorithm)",
        authorIds: ["wynn"],
        children: [],
    },
    {
        id: "brezinski_theta_algorithm",
        title: "brezinski_theta_algorithm",
        subtitle: "Тета-алгоритм Брезински (Brezinski’s θ-algorithm)",
        authorIds: ["brezinski"],
        children: [],
    },
];
