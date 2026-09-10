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

## Руководитель проекта
+ **Денис Васильевич Парфенов** — [promasterden@yandex.ru](mailto:promasterden@yandex.ru)
 
## Команда разработки (Осень 2025 - Весна 2026)
+ **Крейнин Роман** — Тимлид [kreynin.rg@mail.ru](mailto:kreynin.rg@mail.ru)
+ **Наумов Александр** — Главный программист (C++) [alex2004naumov@gmail.com](mailto:alex2004naumov@gmail.com)
+ **Соболев Юрий** — Главный программист (C++, Python, Rust) 
+ **Ядренцев Илья** — Программист (Python, Docker)
+ **Шевыров Аркадий** — Программист (Python, Web)
+ **Мхитарян Арам** — Программист (C++)
+ **Герасимов Александр** — Программист (C++)
+ **Чижик Мария** — Теоретик [masha.tchizhik@yandex.ru](mailto:masha.tchizhik@yandex.ru)
+ **Карбовская Анна**  — Главный Теоретик annkva06@gmail.com
+ **Сиразетдинов Рустем** — Теоретик avesirazetdinov@gmail.com
+ **Погорелов Михаил** — Теоретик mishelpogorelov@yandex.ru
+ **Измалкова Полина** — Теоретик daadys.00@mail.ru
+ **Хлебникова Виктория** — Теоретик vxrus03@yandex.ru
+ **Жемчужная Юлия** — Теоретик zhemchz@yandex.ru
+ **Бесленей Арабелла** — Теоретик arabella.besleney@gmail.com
+ **Филатова Мария** — Теоретик mashaschelk@mail.ru
+ **Кехлеров Тимур ** — Теоретик 1298007@gmail.com
+ **Шарифулин Тимур** — Аналитик tsharifulin353@gmail.com
+ **Утушкин Евгений ** — Аналитик john-utush@mail.ru
+ **Богданова Елизавета** — Аналитик Lisska35361@yandex.ru
+ **Топеха Юлия** — Аналитик

## Команда разработки (Осень 2024 - Весна 2025) 
+ **Крейнин Роман** — Тимлид [kreynin.rg@mail.ru](mailto:kreynin.rg@mail.ru)
+ **Александр Максимов** — Программист (C++) [max6xoft@gmail.com](mailto:max6xoft@gmail.com)
+ **Михаил Сухачёв** — Программист (C++) [mikhail.sukhachev@mail.ru](mailto:mikhail.sukhachev@mail.ru)
+ **Данила Калинин** — Теоретик [danila.vovf@yandex.ru](mailto:danila.vovf@yandex.ru)
+ **Новак Марк** — Программист (C++) [novakmark@list.ru](mailto:novakmark@list.ru)
+ **Бойков Илья** — Теоретик и Программист (C++) [bojkov05@list.ru](mailto:bojkov05@list.ru)
+ **Цыгипа Иван** — Программист (C++) [ivtsygipa@mail.ru](mailto:ivtsygipa@mail.ru)
+ **Бутенко Михаил** — Теоретик [atya200909@yandex.ru](mailto:atya200909@yandex.ru)
+ **Шаронов Кирилл** — Программист (C++) [kiri4sharonov@mail.ru](mailto:kiri4sharonov@mail.ru)
+ **Ююкин Андрей** — Программист (C++) [andryuka05@yandex.ru](mailto:andryuka05@yandex.ru)
+ **Чижик Мария** — Теоретик [masha.tchizhik@yandex.ru](mailto:masha.tchizhik@yandex.ru)

## Команда разработки (Весна 2024)
+ **Крейнин Роман** — Тимлид [kreynin.rg@mail.ru](mailto:kreynin.rg@mail.ru)
+ **Наумов Александр** — Программист (C++) [alex2004naumov@gmail.com](mailto:alex2004naumov@gmail.com)
+ **Юров Павел** — Программист (C++) [pavel.yurov04@bk.ru](mailto:pavel.yurov04@bk.ru)
+ **Беззаборов Антон** — Теоретик и Программист (C++) [antonbezzaborov929@gmail.com](mailto:antonbezzaborov929@gmail.com)
+ **Петерс Егор** — Тимлид [cool47.cool@yandex.ru](mailto:cool47.cool@yandex.ru)
+ **Матков Николай** — Программист (C++) [nikgenom@mail.ru](mailto:nikgenom@mail.ru)
+ **Трудолюбов Никита** — Тимлид [trudolubovnikita259@gmail.com](mailto:trudolubovnikita259@gmail.com)
+ **Павлова Анастасия** — Программист (C++) [vihuhol04@mail.ru](mailto:vihuhol04@mail.ru)

## Авторы оригинального проекта (Осень 2023 - Весна 2024)
+ **Большаков Михаил** — Главный Программист (C++) [mike1024b@mail.ru](mailto:mike1024b@mail.ru)
+ **Евдокимова Екатерина** — Программист (C++) [e_katerina.a_l@mail.ru](mailto:e_katerina.a_l@mail.ru)
+ **Пашков Борис** — Программист (C++) [pashkovborya@gmail.com](mailto:pashkovborya@gmail.com)
+ **Солониченко Злата** — Теоретик [zlaaata_s@mail.ru](mailto:zlaaata_s@mail.ru)
+ **Кармалина Ольга** — Теоретик [karmalinaolga@mail.ru](mailto:karmalinaolga@mail.ru)
