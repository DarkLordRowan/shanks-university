export const depthAccentClass = (level: number) => {
    const palette = [
        "border-primary/70",
        "border-secondary/70",
        "border-accent/70",
        "border-border/70",
    ];
    return palette[level % palette.length];
};
