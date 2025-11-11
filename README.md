# Алгоритмы ускорения сходимости рядов на C++

## Описание
В данном проекте реализованы несколько алгоритмов ускорения сходимости рядов, а именно:
+ Алгоритм Шенкса
+ Эпсилон-алгоритм
    - Базовый
    - Вторая версия
    - Третья версия
+ Алгоритм Левина
    - Базовый
    - Рекурсивный
    - M-алгоритм Левина-Сиди
    - S-алгоритм Левина-Сиди
+ Алгоритм Лубкина W
+ Алгоритм Ричардсона
+ Преобразование Шенкса
+ Ро-алгоритм Винна
+ Алгоритм Венигера
+ алгоритм Ford-Sidi 
+ тета-алгоритм Брезински 
+ алгоритм Чанг-Винн 
+ D-алгоритм Драммонда

Некоторые из этих алгоритмов реализованы в разных имплементациях и работа еще идет. 

Теория: [ссылка на теорию](https://drive.google.com/drive/folders/19KFEQhl9ZR4EE2zDFvi610bNdNBWfGIb?usp=sharing)

## Установка
##### Клонировать проект 
```
git clone git@github.com:DarkLordRowan/shanks-university.git
```
##### Забилдить проект
```
mkdir build
cd build
cmake ..
make
```

## Документация
Doxygen документация доступна [по ссылке](https://bimbobam.github.io)

## Руководитель проекта

Денис Васильевич Парфенов promasterden@yandex.ru

## Авторы
#### Авторы начального проекта(2023-2024)
Большаков Михаил - Главный Программист (организация структуры кода, классический Шенкс) mike1024b@mail.ru

Евдокимова Екатерина - Программист (документация, комментарии, исправления) e_katerina.a_l@mail.ru

Пашков Борис - Программист (Эпсилон алгоритм, ряды), Помощь с Отчетом(Построение графиков, расчет времени выполнения) pashkovborya@gmail.com

Солониченко Злата - Теоретик, Отчет zlaaata_s@mail.ru

Кармалина Ольга - Теоретик, Отчет karmalinaolga@mail.ru

#### Авторы (2024)

Крейнин Роман - Тимлид (Эпсилон-Винн и модификации) kreynin.rg@mail.ru  

Наумов Александр - Программист (Sidi-Levin S-transformation u,t,v) alex2004naumov@gmail.com

Юров Павел - Программист (Левин-Сиди ```t~, lu, lv, lt~, lk```, Ро-Винн, Брезински и Венигер) pavel.yurov04@bk.ru

Беззаборов Антон - Программист (Левин-Сиди ```t~, lu, lv, lt~, lk```, Ро-Винн, Брезински и Венигер) antonbezzaborov929@gmail.com

Петерс Егор - Программист (Форд-Зиди) cool47.cool@yandex.ru

Матков Николай - Программист (Форд-Зиди) nikgenom@mail.ru

Трудолюбов Никита - Программист (30+ ряды, Ричардсон) trudolubovnikita259@gmail.com

Павлова Анастасия - Программист (30+ ряды, Ричардсон) vihuhol04@mail.ru

#### Авторы (2025)
Крейнин Роман - Тимлид kreynin.rg@mail.ru  

Александр Максимов - () max6xoft@gmail.com

Михаил Сухачёв - Теоретик () mikhail.sukhachev@mail.ru

Данила Калинин - () danila.vovf@yandex.ru

Новак Марк - Программист () novakmark@list.ru

Бойков Илья - () bojkov05@list.ru

Цыгипа Иван - Теоретик () ivtsygipa@mail.ru

Бутенко Михаил - Теоретик () atya200909@yandex.ru

Шаронов Кирилл - () kiri4sharonov@mail.ru

Ююкин Андрей - () andryuka05@yandex.ru

Чижик Мария - Теоретик () masha.tchizhik@yandex.ru



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