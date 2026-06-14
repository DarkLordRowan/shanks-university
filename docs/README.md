В данном репозитории представлены реализация методов ускорения сходимости числовых рядов, функционал тестирования и визуализации результатов.

Проект имеет модульную структуру. Модули, относящиеся к вычислениям и тестированию, помещены в backend/. Модули визуализации размещены в frontend/.

# Быстрое начало

Требуются установленные Python 3.13, компилятор GCC 11+ или CLang 13+, CMake 3.11+. Также требуются установленные библиотеки GMP, MPFR, Eigen3.

1) Перейдите в директорию backend/runner.
2) Запустите `install_pyshanks.sh` (или `install_pyshanks.bat`) для компиляции C++ составляющей проекта в Python-модуль.
3) Запустите `python3 main run --config ./config/options.json` для запуска эксперимента.
4) Результаты всех экспериментов будут записаны в `backend/runner/output/results.json`

Для визуализации полученных данных можно применить `frontend/vizr`:
1) Создать виртуальную среду: `python -m venv .venv`
2) Войти в виртуальную среду: `source .venv/bin/activate`
3) Установить `vizr`: `pip install -e ./frontend/vizr`
4) Визуализировать данные: `python3 -m vizr.main backend/runner/output/results_json`

Запуск Fronend:
1. Перейти в папку с кодом сайта: 'cd frontend/web/frontend'
2. Установить зависимости: 'npm install'
3. Запустить проект: 'npm run dev'
4.После запуска в консоли появится ссылка на локальный сайт, обычно что-то вроде: 'http://localhost:5173'. Открой эту ссылку в браузере.

ИЛИ

Открой папку frontend/web/frontend в IDE и запусти проект как обычное React TypeScript приложение.

Для анализа необходимо:
1) Запустить анализ и экспорт '.\vizrd.exe --no-cache headless (Адрес json-файла) --export (Название папки)'
2) Папку с parquet отправить на сайт

# Подробнее

- [Архитектура](architecture.md)
- [Dataflow](pipeline.md)
- [Установка Docker](installation/docker.md)
- [Установка Linux](installation/linux.md)
- [Добавление методов/рядов](extending/overview.md)
- [Конфигурация экспериментов](runner-config/overview.md)
