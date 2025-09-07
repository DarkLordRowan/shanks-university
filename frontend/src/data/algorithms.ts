import type { AlgNode } from "../types/algorithms";

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
        id: "levin_sidi",
        title: "levin_sidi_algorithm",
        subtitle: "Алгоритм Левина-Сиди (Levin-Sidi’s Transformation)",
        authorIds: ["levin",'sidi'],
        children: [
            {
                id: "levin_sidi_basic_algorithm",
                title: "levin_sidi_basic_algorithm",
                subtitle: "базовый",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_m_algorithm",
                title: "levin_sidi_m_algorithm",
                subtitle: "M-Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_s_algorithm",
                title: "levin_sidi_s_algorithm",
                subtitle: "S-Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_t_algorithm",
                title: "levin_sidi_t_algorithm",
                subtitle: "Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_u_algorithm",
                title: "levin_sidi_u_algorithm",
                subtitle: "Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_v_algorithm",
                title: "levin_sidi_v_algorithm",
                subtitle: "Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_t_recursive_algorithm",
                title: "levin_sidi_t_recursive_algorithm",
                subtitle: "Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_u_recursive_algorithm",
                title: "levin_sidi_u_recursive_algorithm",
                subtitle: "Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
            {
                id: "levin_sidi_v_recursive_algorithm",
                title: "levin_sidi_v_recursive_algorithm",
                subtitle: "Алгоритм Левина-Сиди",
                authorIds: ["levin",'sidi'],
            },
        ],
    },
    {
        id: "lubkin_w_algorithm",
        title: "lubkin_w_algorithm",
        subtitle: "W-Алгоритм Лубкина",
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
        id: "shanks_algorithm",
        title: "shanks_algorithm",
        subtitle: "Алгоритм Шенкса (Shanks’ Transformation)",
        authorIds: ["shanks"],
        children: [
            {
                id: "shanks_algorithm",
                title: "shanks_algorithm",
                subtitle: "Алгоритм Шенкса (//TODO)",
                authorIds: ["shanks"],
            },
            {
                id: "shanks_alternating_algorithm",
                title: "shanks_alternating_algorithm",
                subtitle: "Алгоритм Шенкса (//TODO)",
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
                subtitle: "Эпсилон-алгоритм Винна (базовый)",
                authorIds: ["wynn"]
            },
            {
                id: "wynn_epsilon_two_algorithm",
                title: "wynn_epsilon_two_algorithm",
                subtitle: "Эпсилон-алгоритм Винна (вторая версия)",
                authorIds: ["wynn"]
            },
            {
                id: "wynn_epsilon_three_algorithm",
                title: "wynn_epsilon_three_algorithm",
                subtitle: "Эпсилон-алгоритм Винна (третья версия)",
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
