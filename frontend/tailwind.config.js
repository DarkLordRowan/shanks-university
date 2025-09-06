/** @type {import('tailwindcss').Config} */
export default {
    content: ["./index.html", "./src/**/*.{ts,tsx}"],
    theme: {
        extend: {
            colors: {
                // фон и панели
                surface: "#0b1220",   // общий фон
                panel:   "#131a2a",   // карточки / header
                // акценты
                primary:   "#6c8cf5", // синий
                secondary: "#a78bfa", // фиолетовый
                accent:    "#22d3ee", // бирюзовый
                border:    "#1f2a44",
                textDim:   "#9aa4b2",
            },
            boxShadow: {
                panel: "0 6px 24px rgba(0,0,0,.35)",
            },
            borderRadius: {
                xl2: "1rem",
            },
            container: { center: true, padding: "1rem" },
        },
    },
    plugins: [],
};
