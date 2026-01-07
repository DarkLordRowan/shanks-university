// src/shared/lib/dom/useWheelScrollCapture.ts
import { useEffect } from "react";

export function useWheelScrollCapture(ref: React.RefObject<HTMLElement | null>, enabled: boolean) {
    useEffect(() => {
        const el = ref.current;
        if (!el || !enabled) return;

        const onWheel = (e: WheelEvent) => {
            // Только вертикаль
            const dy = e.deltaY;
            if (dy === 0) return;

            const { scrollTop, scrollHeight, clientHeight } = el;
            const atTop = scrollTop <= 0;
            const atBottom = scrollTop + clientHeight >= scrollHeight - 1;

            const scrollingUp = dy < 0;
            const scrollingDown = dy > 0;

            // Если можем прокрутить контейнер, перехватываем колесо и крутим его
            const canScrollUp = !atTop && scrollingUp;
            const canScrollDown = !atBottom && scrollingDown;

            if (canScrollUp || canScrollDown) {
                e.preventDefault();
                e.stopPropagation();
                el.scrollTop += dy;
            }
            // Иначе пусть скроллится страница (ничего не делаем)
        };

        // Важно: passive:false, иначе preventDefault не сработает
        el.addEventListener("wheel", onWheel, { passive: false });

        return () => {
            el.removeEventListener("wheel", onWheel as EventListener);
        };
    }, [ref, enabled]);
}
