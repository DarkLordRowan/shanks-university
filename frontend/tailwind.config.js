/** @type {import('tailwindcss').Config} */
export default {
    content: ["./index.html", "./src/**/*.{ts,tsx}"],
    theme: {
        extend: {
            colors: {
                // фон и панели
                surface: "#0a0a0f",   // ещё темнее фон
                panel:   "#1e293b",   // заметно светлее для контраста
                // акценты
                primary:   "#3b82f6", // ярко-синий
                secondary: "#a855f7", // насыщенный фиолетовый
                accent:    "#06b6d4", // яркая бирюза
                border:    "#334155", // более заметная обводка
                textDim:   "#cbd5e1", // светло-серый, лучше читается на тёмном фоне
            },
            boxShadow: {
                panel: "0 8px 20px rgba(0,0,0,0.6)", // более выражённая тень
            },
            borderRadius: {
                xl2: "1rem",
            },
            container: { center: true, padding: "1rem" },
        },
    },
    plugins: [],
};
