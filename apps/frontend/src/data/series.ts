// Простой тип для узла ряда
export type SeriesNode = {
    id: string;
    title: string;
    subtitle?: string;
    formula?: string;
    url?: string;
    document?: string;
};

export const SERIES: SeriesNode[] = [
    {
        id: "geom_series",
        title: "Геометрический ряд",
        subtitle: "Простейший пример",
        formula: String.raw`\sum_{n=0}^{\infty} r^n = \frac{1}{1-r},\quad |r|<1`,
    },
    {
        id: "harmonic_series",
        title: "Гармонический ряд",
        subtitle: "Медленная расходимость",
        formula: String.raw`\sum_{n=1}^{\infty} \frac{1}{n} \text{ — расходится}`,
    },
];
