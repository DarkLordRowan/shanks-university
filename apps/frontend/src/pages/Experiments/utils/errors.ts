export const normalizeNetworkError = (e: unknown) =>
    e instanceof TypeError && e.message === "Failed to fetch"
        ? "Не удалось выполнить запрос (возможна CORS/сетевая ошибка). Проверьте доступность API и CORS."
        : e instanceof Error && e.message
            ? e.message
            : "Неизвестная ошибка сети.";
