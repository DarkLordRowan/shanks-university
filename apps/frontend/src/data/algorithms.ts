import type { AlgNode } from "../types/algorithms";

export const TREE: AlgNode[] = [
    {
        id: "brezinski_theta_algorithm",
        title: "brezinski_theta_algorithm",
        subtitle: "Тета-алгоритм Брезински (Brezinski’s θ-algorithm)",
        authorIds: ["brezinski"],
        children: [],
    },
    {
        id: "chang_wynn_algorithm",
        title: "chang_wynn_algorithm",
        subtitle: "Алгоритм Чанг–Винна (Chang–Wynn algorithm)",
        authorIds: ["chang", "wynn"],
        children: [],
    },
    {
        id: "drummond_d_algorithm",
        title: "drummond_d_algorithm",
        subtitle: "Алгоритм Драммонда (Drummond’s algorithm)",
        authorIds: ["drummond"],
        children: [],
    },
    {
        id: "ford_sidi_algorithm",
        title: "ford_sidi_algorithm",
        subtitle: "Алгоритм Форд–Сиди (Ford–Sidi algorithm)",
        authorIds: ["ford", "sidi"],
        children: [
            {
                id: "ford_sidi_2_algorithm",
                title: "ford_sidi_2_algorithm",
                subtitle: "Алгоритм Форд–Сиди (вторая версия)",
                authorIds: ["ford", "sidi"]
            },
            {
                id: "ford_sidi_3_algorithm",
                title: "ford_sidi_3_algorithm",
                subtitle: "Алгоритм Форд–Сиди (третья версия)",
                authorIds: ["ford", "sidi"]
            },
        ],
    },
    {
        id: "levin_algorithm",
        title: "levin_algorithm",
        subtitle: "Алгоритм Левина (Levin’s algorithm)",
        authorIds: ["levin"],
        children: [],
    },
    {
        id: "levin_sidi_algorithm",
        title: "levin_sidi_algorithm",
        subtitle: "Алгоритм Левина–Сиди (Levin–Sidi’s algorithm)",
        authorIds: ["levin", 'sidi'],
        children: [
            {
                id: "levin_sidi_m_algorithm",
                title: "levin_sidi_m_algorithm",
                subtitle: "M-алгоритм Левина–Сиди",
                authorIds: ["levin", 'sidi'],
            },
            {
                id: "levin_sidi_s_algorithm",
                title: "levin_sidi_s_algorithm",
                subtitle: "S-алгоритм Левина–Сиди",
                authorIds: ["levin", 'sidi'],
            },
        ],
    },
    {
        id: "lubkin_w_algorithm",
        title: "lubkin_w_algorithm",
        subtitle: "W-алгоритм Лубкина (Lubkin’s W-algorithm)",
        authorIds: ["lubkin"],
        children: [],
    },
    {
        id: "richardson_algorithm",
        title: "richardson_algorithm",
        subtitle: "Алгоритм Ричардсона (Richardson algorithm)",
        authorIds: ["richardson"],
        children: [],
    },
    {
        id: "shanks_algorithm",
        title: "shanks_algorithm",
        subtitle: "Алгоритм Шенкса (Shanks’ algorithm)",
        authorIds: ["shanks"],
        children: [],
    },
    {
        id: "weniger_algorithm",
        title: "weniger_algorithm",
        subtitle: "Алгоритм Венигера (Weniger’s algorithm)",
        document: "Weniger.pdf -> weniger_algorithm.pdf",
        authorIds: ["weniger"],
        children: [],
    },
    {
        id: "wynn_epsilon_algorithm",
        title: "wynn_epsilon_algorithm",
        subtitle: "Эпсилон-алгоритм Винна (Wynn’s ε-algorithm)",
        authorIds: ["wynn"],
        children: [
            {
                id: "wynn_epsilon_1_algorithm",
                title: "wynn_epsilon_1_algorithm",
                subtitle: "Эпсилон-алгоритм Винна (первая версия)",
                authorIds: ["wynn"]
            },
            {
                id: "wynn_epsilon_2_algorithm",
                title: "wynn_epsilon_2_algorithm",
                subtitle: "Эпсилон-алгоритм Винна (вторая версия)",
                authorIds: ["wynn"]
            },
            {
                id: "wynn_epsilon_3_algorithm",
                title: "wynn_epsilon_3_algorithm",
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
];
