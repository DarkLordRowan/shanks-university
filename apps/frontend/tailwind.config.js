/** @type {import('tailwindcss').Config} */
export default {
    content: ["./index.html", "./src/**/*.{ts,tsx}"],
    theme: {
        extend: {
            colors: {
                // фон и панели
                surface:  "#0a0a0f",
                panel:    "#1e293b",
                // акценты
                primary:   "#3b82f6",
                secondary: "#a855f7",
                accent:    "#06b6d4",
                border:    "#334155",
                textDim:   "#cbd5e1",
            },
            boxShadow: {
                panel: "0 8px 20px rgba(0,0,0,0.6)",
            },
            borderRadius: {
                xl2: "1rem",
            },
            container: { center: true, padding: "1rem" },
        },
    },
    plugins: [
        require("@tailwindcss/typography"),
        require("@tailwindcss/forms")({ strategy: "class" }),
    ],
};
