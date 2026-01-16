# Алгоритмы ускорения сходимости рядов на C++

**Для более подробной документации о текущей структуре проекта, пожалуйста, проверьте [docs/](docs/README.md).**
Doxygen документация доступна [по ссылке](https://darklordrowan.github.io/shanks-university/)

## Описание
В данном проекте реализованы несколько алгоритмов ускорения сходимости рядов, а именно:
1. Алгоритм ускорения Андерсона (Anderson's acceleration)
2. Тета-преобразование Брезински (Brezinski Theta)
3. Преобразование Чанг-Винна (Chang-Wynn)
4. D-алгоритм Драммонда
    * Варианты: U, T, T-волна, V, V-волна.
    * Рекурсивные версии для каждого типа.
5. J-преобразование (J transformation)
    * Примечание: это «ускорение» для последовательностей, связанных с непрерывными дробями.
6. Преобразование Форда-Сиди
    * Версии: Sidi-2 и Sidi-3.
7. Алгоритм Левина (Levin Sidi-L)
    * Варианты: U, T, T-волна, V, V-волна.
    * Рекурсивные версии для каждого типа.
8. M-алгоритм Левина-Сиди
    * Варианты по типу остатка: U, T, T-волна, V, V-волна.
9. S-алгоритм Левина-Сиди
    * Варианты: U, T, T-волна, V, V-волна.
    * Рекурсивные версии для каждого типа.
10. W-преобразование Лубкина (Lubkin W)
11. Преобразование Ричардсона (Richardson)
12. Преобразование Шенкса
    * Базовое и Альтернирующее (Shanks Alternating).
13. Преобразование Венигера (Weniger)
14. Эпсилон-алгоритм Винна
    * Версии: Epsilon-1, Epsilon-2, Epsilon-3.
15. Ро-алгоритм Винна
    * Варианты: Rho-Rho, Обобщенный Rho-Gamma-Rho.

Теория: [ссылка на теорию](https://drive.google.com/drive/folders/19KFEQhl9ZR4EE2zDFvi610bNdNBWfGIb?usp=sharing)

## Руководитель проекта

Денис Васильевич Парфенов promasterden@yandex.ru

## Команда разработки

+ **Крейнин Роман** — Тимлид [kreynin.rg@mail.ru](mailto:kreynin.rg@mail.ru)
+ **Наумов Александр** — Программист (C++)
+ **Соболев Юрий** — Программист (C++, Python, Rust)
+ **Лыков Даниил** — Программист (C++)
+ **Ядренцев Илья** — Программист (Python, Docker)
+ **Шевыров Александр** — Программист (Python, Web)
+ **Мхитарян Армен** — Программист (C++)
+ **Герасимов Александр** — Программист (C++)

## Ссылки на литературу

- [Статья Шенкса про его преобразования](https://onlinelibrary.wiley.com/doi/abs/10.1002/sapm19553411)

- [Публикации Винна (в частности есть статья про Эпсилон алгоритм Винна)](https://mathresearch.utsa.edu/Legacy/Peter-Wynn/publications.html)

- [Про Эпсилон алгоритм, примененный к монотонным и осцилирующим последовательностям](https://www.sciencedirect.com/science/article/pii/S0377042700005616)

- [Подробный анализ трансформации Шенкса и Эпсилон алгоритма](https://www.researchgate.net/publication/327178717_The_genesis_and_early_developments_of_Aitken's_process_Shanks'_transformation_the_e-algorithm_and_related_fixed_point_methods)

# Запуск backend api:

```
docker build -t shanks-api:latest .
docker run -d --name shanks-api -p 8000:8000 --restart unless-stopped shanks-api:latest
```
