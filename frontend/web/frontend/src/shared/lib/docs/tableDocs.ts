export const DOCS_HOME_ANCHOR_ID = "docs-overview";

export type DocumentedTableKey = "algo-ranking" | "series-computed";

export type DocsColumnPreference = "lower" | "higher" | "neutral";

export type AlgoRankingDocsColumnKey =
    | "precision"
    | "m"
    | "arg1"
    | "arg2"
    | "arg3"
    | "seriesCount"
    | "avgBestDeviation"
    | "avgRelativeError"
    | "avgOrdersGain"
    | "avgAmpAtMinN"
    | "notBetterThanSeriesShare"
    | "avgMinDeviationN"
    | "avgLastMinusMin"
    | "avgStepsToTol"
    | "fracReachedTol"
    | "oneSidedShare"
    | "bestMinShare"
    | "worstMinShare"
    | "bestLastShare"
    | "worstLastShare"
    | "rankPrecision"
    | "rankSpeed"
    | "rankStability"
    | "totalRankScore";

export type SeriesComputedDocsColumnKey =
    | "name"
    | "precision"
    | "args"
    | "class"
    | "k"
    | "sign"
    | "viol"
    | "devMin"
    | "minN"
    | "devLast"
    | "lastN"
    | "lastMinusMin"
    | "ampOrders"
    | "maxAmpOrders"
    | "devMean"
    | "devMedian"
    | "devMax";

export interface DocsColumnSection<TColumnKey extends string = string> {
    key: TColumnKey;
    id: string;
    title: string;
    preference: DocsColumnPreference;
    markdown: string;
}

export interface DocsXlsxSheetInfo {
    name: string;
    description: string;
    markdown: string;
}

export interface DocsTableSection<TColumnKey extends string = string> {
    key: DocumentedTableKey;
    id: string;
    screenSectionId: string;
    xlsxSectionId: string;
    title: string;
    summary: string;
    introMarkdown: string;
    screenMarkdown: string;
    xlsxMarkdown: string;
    columns: readonly DocsColumnSection<TColumnKey>[];
    xlsxSheets: readonly DocsXlsxSheetInfo[];
}

function buildTableAnchorId(key: DocumentedTableKey): string {
    return `docs-${key}`;
}

function buildScreenAnchorId(key: DocumentedTableKey): string {
    return `docs-${key}-screen`;
}

function buildXlsxAnchorId(key: DocumentedTableKey): string {
    return `docs-${key}-xlsx`;
}

function buildColumnAnchorId(key: DocumentedTableKey, columnKey: string): string {
    return `docs-${key}-col-${columnKey}`;
}

function buildArgMarkdown(slot: number): string {
    return [
        `Показывает ${slot}-й слот списка аргументов после сортировки параметров по имени и сборки в строку вида \`key=value\`.`,
        "",
        "На экране выводятся только первые три непустых слота. Если аргумента для этого места нет, в таблице и xlsx будет пустое значение или `-`.",
        "",
        "**Интерпретация:** это идентификатор конфигурации алгоритма, а не метрика качества.",
    ].join("\n");
}

function createColumn<TColumnKey extends string>(
    tableKey: DocumentedTableKey,
    key: TColumnKey,
    title: string,
    preference: DocsColumnPreference,
    markdown: string
): DocsColumnSection<TColumnKey> {
    return {
        key,
        id: buildColumnAnchorId(tableKey, key),
        title,
        preference,
        markdown,
    };
}

export const ALGO_RANKING_TABLE_DOCS: DocsTableSection<AlgoRankingDocsColumnKey> = {
    key: "algo-ranking",
    id: buildTableAnchorId("algo-ranking"),
    screenSectionId: buildScreenAnchorId("algo-ranking"),
    xlsxSectionId: buildXlsxAnchorId("algo-ranking"),
    title: "Ранги алгоритмов",
    summary:
        "Агрегированная таблица по алгоритмам: как близко они подходят к пределу, насколько устойчиво ведут себя по рядам и как выглядят их сводные ранги.",
    introMarkdown: String.raw`
Таблица агрегирует **все запуски одного алгоритма с фиксированными параметрами** и считает по ним сводные метрики.

Алгоритм считается «тем же самым», если совпадают:

- имя ускорителя;
- параметр \(m\);
- набор \`args\`.

После этого по всем подходящим рядам считаются средние значения, доли успеха и итоговые ранги.  
Экранная таблица и лист \`algo_ranking\` в xlsx используют **одни и те же агрегированные значения**.
`,
    screenMarkdown: String.raw`
Экранная таблица показывает уже **отфильтрованный** набор алгоритмов. На расчёт влияют:

- выбранный \`precision\`;
- слайдер \`epsilon\`;
- фильтры по алгоритмам и рядам;
- текущая сортировка.

\`rank precision\`, \`rank speed\` и \`rank stability\` считаются не по исходным числам напрямую, а как **суммы плотных рангов** по нескольким столбцам. Поэтому маленький итоговый ранг означает, что алгоритм стабильно держится вверху сразу по нескольким критериям.
`,
    xlsxMarkdown: String.raw`
Экспорт xlsx повторяет текущий срез таблицы. Если на экране применены фильтры, в файл уйдут только строки, оставшиеся после фильтрации.

В xlsx для этой таблицы используются два листа:

- \`overview\` — параметры экспорта и активные настройки;
- \`algo_ranking\` — сама сводная таблица по алгоритмам.
`,
    columns: [
        createColumn(
            "algo-ranking",
            "precision",
            "precision",
            "neutral",
            [
                "Показывает `precision` рядов, из которых собрана строка алгоритма.",
                "",
                "Если в строку попали данные из нескольких precision без дополнительного фильтра, на экране может появиться `-`.",
                "",
                "**XLSX:** то же агрегированное значение уходит в лист `algo_ranking`.",
            ].join("\n")
        ),
        createColumn(
            "algo-ranking",
            "m",
            "m",
            "neutral",
            [
                "Параметр ускорителя `m`.",
                "",
                "Это параметр конфигурации, а не оценка качества. По нему удобно отделять семейства одного и того же алгоритма.",
                "",
                "**XLSX:** числовое значение `m`, если оно задано.",
            ].join("\n")
        ),
        createColumn("algo-ranking", "arg1", "arg 1", "neutral", buildArgMarkdown(1)),
        createColumn("algo-ranking", "arg2", "arg 2", "neutral", buildArgMarkdown(2)),
        createColumn("algo-ranking", "arg3", "arg 3", "neutral", buildArgMarkdown(3)),
        createColumn(
            "algo-ranking",
            "seriesCount",
            "series",
            "higher",
            [
                "Число уникальных рядов, по которым этот алгоритм реально участвовал в сравнении после всех фильтров.",
                "",
                "Чем больше значение, тем надёжнее остальные средние показатели: они посчитаны на большем количестве серий данных.",
                "",
                "**XLSX:** числовой столбец `series`.",
            ].join("\n")
        ),
        createColumn(
            "algo-ranking",
            "avgBestDeviation",
            "avg min |dev|",
            "lower",
            String.raw`
Среднее лучшее отклонение алгоритма от предела:

$$
\operatorname{avgMinDev}
=
\frac{1}{M}\sum_{r=1}^{M}\min_n \left|A_n^{(r)} - S^{(r)}\right|.
$$

Здесь \(A_n^{(r)}\) — значение алгоритма на ряде \(r\), а \(S^{(r)}\) — предел этого ряда.

**Как читать:** чем меньше значение, тем ближе алгоритм в среднем подходит к пределу в своей лучшей точке.

**XLSX:** числовой столбец в научной записи.
`
        ),
        createColumn(
            "algo-ranking",
            "avgRelativeError",
            "avg rel error",
            "lower",
            String.raw`
Среднее относительное отклонение минимума алгоритма к минимуму частичных сумм:

$$
\operatorname{avgRelError}
=
\operatorname{mean}\left(
\frac{\min_n |A_n - S|}{\min_n |S_n - S|}
\right),
\qquad \min_n |S_n - S| > 0.
$$

Если у ряда \(\min_n |S_n - S| = 0\), такой случай **не попадает в это среднее** и оценивается через столбцы амплитуд \`series/algo amp\`.

**Как читать:**  
- \`1\` — алгоритм в среднем на уровне частичных сумм;  
- \`< 1\` — лучше ряда;  
- \`> 1\` — хуже ряда.

Если для алгоритма не нашлось ни одного ряда с положительным знаменателем, таблица использует нейтральное значение \`1\`.
`
        ),
        createColumn(
            "algo-ranking",
            "avgOrdersGain",
            "avg series/algo amp",
            "higher",
            String.raw`
Средний выигрыш алгоритма над частичными суммами по порядкам:

$$
\operatorname{avgSeriesAlgoAmp}
=
\operatorname{mean}\left(
\log_{10}\min_n |S_n - S|
-
\log_{10}\min_n |A_n - S|
\right).
$$

**Как читать:**  
- \`1\` означает примерно **в 10 раз** лучшую точность алгоритма по сравнению с лучшим значением ряда;  
- \`2\` — примерно в **100 раз** лучше;  
- \`0\` — на одном порядке;  
- \`< 0\` — алгоритм хуже ряда.

Эта метрика особенно полезна там, где обычное относительное отношение плохо работает из-за нулевого минимума ряда.
`
        ),
        createColumn(
            "algo-ranking",
            "avgAmpAtMinN",
            "avg series@min n/algo amp",
            "higher",
            String.raw`
Сравнение алгоритма с рядом **на том самом первом шаге**, где алгоритм впервые достиг своего минимума.

Сначала определяется

$$
n_A^\* = \min\left\{n : |A_n - S| = \min_k |A_k - S| \right\}.
$$

После этого считается

$$
\operatorname{avgAmpAtMinN}
=
\operatorname{mean}\left(
\log_{10}|S_{n_A^\*} - S|
-
\log_{10}\min_n |A_n - S|
\right).
$$

**Как читать:** метрика показывает, насколько алгоритм выигрывает у самого ряда именно в момент достижения собственного минимума.

Сейчас этот столбец служит дополнительной диагностикой и **не входит** в \`rank precision\`, чтобы не дублировать вклад \`avg series/algo amp\`.
`
        ),
        createColumn(
            "algo-ranking",
            "notBetterThanSeriesShare",
            "min algo >= min series, %",
            "lower",
            String.raw`
Доля рядов, где алгоритм не смог превзойти частичные суммы:

$$
\frac{
\#\left\{r : \min_n |A_n^{(r)} - S^{(r)}| \ge \min_n |S_n^{(r)} - S^{(r)}| \right\}
}{
\#\left\{r : \text{для ряда есть сравнимый минимум}\right\}
}.
$$

**Идеал:** \`0%\`.

Если значение растёт, это означает, что всё больше рядов алгоритм решает не лучше собственных частичных сумм.
`
        ),
        createColumn(
            "algo-ranking",
            "avgMinDeviationN",
            "avg min dev n",
            "lower",
            String.raw`
Средний номер шага, на котором алгоритм впервые достигает своего минимума:

$$
\operatorname{avgMinN}
=
\frac{1}{M}\sum_{r=1}^{M} n_{A,r}^\*.
$$

Меньшее значение означает, что алгоритм в среднем выходит на лучшую найденную точку раньше.
`
        ),
        createColumn(
            "algo-ranking",
            "avgLastMinusMin",
            "avg last-min",
            "lower",
            String.raw`
Средний уход от лучшей точки к последней:

$$
\operatorname{avgLastMinusMin}
=
\operatorname{mean}\left(
|A_{n_{\text{last}}} - S|
-
\min_n |A_n - S|
\right).
$$

**Как читать:**  
- \`0\` — алгоритм не испортил качество после достижения минимума;  
- большое положительное значение — после лучшей точки алгоритм ушёл хуже.

Это один из главных столбцов устойчивости.
`
        ),
        createColumn(
            "algo-ranking",
            "avgStepsToTol",
            "avg steps to eps",
            "lower",
            String.raw`
Средний первый шаг, на котором алгоритм достигает заданной точности \`epsilon\`:

$$
n_{\varepsilon}^\*
=
\min\{n : |A_n - S| \le \varepsilon\}.
$$

В среднее попадают только те ряды, где такой шаг вообще существует.

**Как читать:** чем меньше значение, тем быстрее алгоритм достигает целевой точности.
`
        ),
        createColumn(
            "algo-ranking",
            "fracReachedTol",
            "reached eps, %",
            "higher",
            String.raw`
Доля рядов, на которых алгоритм хотя бы раз достиг порога \`epsilon\`:

$$
\operatorname{reachedEpsShare}
=
\frac{\#\{r : \exists n,\ |A_n^{(r)} - S^{(r)}| \le \varepsilon\}}{M}.
$$

**Как читать:** чем больше процент, тем чаще алгоритм вообще добирается до заданной точности.
`
        ),
        createColumn(
            "algo-ranking",
            "oneSidedShare",
            "1-sided, %",
            "higher",
            String.raw`
Доля запусков, где алгоритм классифицирован как односторонний:

$$
\operatorname{oneSidedShare}
=
\frac{\#\{\text{односторонние запуски}\}}{M}.
$$

Класс \`→\` тоже считается односторонним.

**Как читать:** высокий процент обычно означает более предсказуемую и «чистую» траекторию приближения к пределу.
`
        ),
        createColumn(
            "algo-ranking",
            "bestMinShare",
            "best min div, %",
            "higher",
            String.raw`
Доля рядов, где минимум этого алгоритма оказался лучшим среди всех алгоритмов:

$$
\operatorname{bestMinShare}
=
\frac{\#\{r : \min_n |A_n - S| = \min_{\text{algo}} \min_n |A_n - S| \}}{\#\{\text{сравнимые ряды}\}}.
$$

Если несколько алгоритмов дали одинаковый лучший минимум, процент засчитывается **каждому из них**.

Из-за этого сумма процентов по столбцу между несколькими алгоритмами может быть **больше 100%**.
`
        ),
        createColumn(
            "algo-ranking",
            "worstMinShare",
            "worst min div, %",
            "lower",
            String.raw`
Доля рядов, где минимум этого алгоритма оказался худшим среди всех алгоритмов.

Логика tie-case такая же, как и у \`best min div, %\`: если несколько алгоритмов разделили одинаково плохой минимум, процент получает каждый из них.

Из-за этого сумма процентов по столбцу тоже может быть **больше 100%**.

**Как читать:** чем меньше, тем лучше.
`
        ),
        createColumn(
            "algo-ranking",
            "bestLastShare",
            "best last div, %",
            "higher",
            String.raw`
Доля рядов, где на последнем доступном шаге именно этот алгоритм имел лучшее отклонение от предела среди всех алгоритмов.

Метрика отвечает на вопрос: **кто выглядит лучше в финальной точке**, а не только в лучшем моменте.
`
        ),
        createColumn(
            "algo-ranking",
            "worstLastShare",
            "worst last div, %",
            "lower",
            String.raw`
Доля рядов, где на последнем доступном шаге этот алгоритм оказался худшим среди всех алгоритмов.

Если метрика высокая, это означает, что алгоритм часто заканчивает в неудачной точке даже тогда, когда раньше мог показывать неплохой минимум.
`
        ),
        createColumn(
            "algo-ranking",
            "rankPrecision",
            "rank precision",
            "lower",
            String.raw`
Сводный ранг по точности. Для каждого алгоритма сначала считаются плотные ранги по столбцам:

- \`avg min |dev|\`,
- \`avg rel error\`,
- \`avg series/algo amp\`,
- \`min algo >= min series, %\`,
- \`best min div, %\`,
- \`worst min div, %\`.

После этого ранги суммируются:

$$
\operatorname{rankPrecision}
=
\rho(\text{avgMinDev})
\!+\!
\rho(\text{avgRelError})
\!+\!
\rho(\text{avgSeriesAlgoAmp})
\!+\!
\rho(\text{notBetterThanSeriesShare})
\!+\!
\rho(\text{bestMinShare})
\!+\!
\rho(\text{worstMinShare}).
$$

**Как читать:** меньше — лучше.
`
        ),
        createColumn(
            "algo-ranking",
            "rankSpeed",
            "rank speed",
            "lower",
            String.raw`
Сводный ранг по скорости достижения хороших значений.

Суммируются плотные ранги двух метрик:

$$
\operatorname{rankSpeed}
=
\rho(\text{avgMinDevN})
\!+\!
\rho(\text{avgStepsToEps}).
$$

Чем меньше значение, тем раньше алгоритм выходит на минимум и тем быстрее достигает порога \`epsilon\`.
`
        ),
        createColumn(
            "algo-ranking",
            "rankStability",
            "rank stability",
            "lower",
            String.raw`
Сводный ранг по устойчивости поведения:

$$
\operatorname{rankStability}
=
\rho(\text{reachedEpsShare})
\!+\!
\rho(\text{oneSidedShare})
\!+\!
\rho(\text{avgLastMinusMin})
\!+\!
\rho(\text{bestLastShare})
\!+\!
\rho(\text{worstLastShare}).
$$

Он поощряет алгоритмы, которые:

- часто доходят до \`epsilon\`;
- делают это односторонне;
- не убегают от минимума;
- хорошо выглядят на последнем шаге.
`
        ),
        createColumn(
            "algo-ranking",
            "totalRankScore",
            "total rank",
            "lower",
            String.raw`
Общий итоговый ранг:

$$
\operatorname{totalRank}
=
\operatorname{rankPrecision}
\!+\!
\operatorname{rankSpeed}
\!+\!
\operatorname{rankStability}.
$$

Это главный сводный столбец таблицы.

**Как читать:**  
- меньше — лучше;  
- при равенстве итоговый порядок дополнительно уточняется по \`avg min |dev|\`, \`avg series/algo amp\`, \`avg series@min n/algo amp\` и \`avg rel error\`.
`
        ),
    ],
    xlsxSheets: [
        {
            name: "overview",
            description: "Параметры текущего экспорта.",
            markdown: [
                "Содержит служебную информацию о том, **как именно** был собран файл:",
                "",
                "- выбранный `epsilon` и его показатель степени;",
                "- фильтр по `precision`;",
                "- активный ключ сортировки и направление;",
                "- число строк и число экспортированных колонок.",
            ].join("\n"),
        },
        {
            name: "algo_ranking",
            description: "Основная сводная таблица по алгоритмам.",
            markdown: [
                "Повторяет экранную таблицу для текущего набора фильтров.",
                "",
                "Первые колонки — `place` и `algorithm`, дальше идут те же метрики, что и на экране. Форматы чисел и процентов уже проставлены в самом xlsx.",
            ].join("\n"),
        },
    ],
};

export const SERIES_COMPUTED_TABLE_DOCS: DocsTableSection<SeriesComputedDocsColumnKey> = {
    key: "series-computed",
    id: buildTableAnchorId("series-computed"),
    screenSectionId: buildScreenAnchorId("series-computed"),
    xlsxSectionId: buildXlsxAnchorId("series-computed"),
    title: "Частичные суммы",
    summary:
        "Таблица по рядам, которая показывает, как ведут себя частичные суммы относительно предела: класс траектории, минимум, последняя точка, амплитуды и сводные статистики.",
    introMarkdown: String.raw`
Таблица строится по **частичным суммам ряда** \(S_n\) и сравнению с пределом \(S\).

Для каждого ряда считается траектория ошибок

$$
d_n = |S_n - S|.
$$

Дальше по этой траектории строятся:

- класс поведения (\`1s ↓→\`, \`2s ↑↑\` и т.д.);
- номер шага лучшей точки;
- последняя ошибка;
- амплитуды в логарифмической шкале;
- средние и экстремальные значения.
`,
    screenMarkdown: String.raw`
Экранная таблица показывает один ряд на строку.  
Сортировка всегда работает по **текущим числам**, которые видны после пересчёта класса с учётом двух порогов:

- \`max sign changes\` — сколько смен знака ещё допускается для режима \`1s\`;
- \`max violations\` — сколько ростов ошибки ещё допускается для «почти монотонного» поведения.

Клик по строке открывает детальный блок с графиками и точками ряда. Та же строка может быть экспортирована в xlsx вместе с подробными листами \`selected_*\`.
`,
    xlsxMarkdown: String.raw`
Экспорт xlsx для частичных сумм состоит из сводного листа и, при выбранном ряде, из нескольких детальных листов.

Важно: в xlsx класс разложен на две части:

- \`side\` — \`1s\` или \`2s\`;
- \`class\` — символ поведения (\`→\`, \`↓→\`, \`↓↓\`, \`↑↓\`, \`↓↑\`, \`↑↑\`).

Для класса \`→\` в xlsx используется \`side = 1s\`.
`,
    columns: [
        createColumn(
            "series-computed",
            "name",
            "Ряд",
            "neutral",
            [
                "Имя ряда.",
                "",
                "Используется как основной идентификатор строки на экране и в xlsx.",
            ].join("\n")
        ),
        createColumn(
            "series-computed",
            "precision",
            "prec",
            "neutral",
            [
                "Precision, с которым были вычислены частичные суммы ряда.",
                "",
                "Это параметр вычисления, а не оценка качества.",
            ].join("\n")
        ),
        createColumn(
            "series-computed",
            "args",
            "args",
            "neutral",
            [
                "Краткая строка параметров ряда в виде `key=value`.",
                "",
                "Полезна для различения нескольких запусков одного и того же аналитического ряда с разными `x`, `q` и другими параметрами.",
            ].join("\n")
        ),
        createColumn(
            "series-computed",
            "class",
            "класс",
            "neutral",
            String.raw`
Класс показывает **форму траектории** ошибки \(d_n = |S_n - S|\), а цвет — сочетание класса и односторонности.

Обозначения:

- \`→\` — ошибка почти не меняется;
- \`↓→\` — минимум достигнут раньше последней точки и дальше ошибка не растёт;
- \`↓↓\` — минимум достигнут в последней точке;
- \`↑↓\` — к концу пришли к минимуму, но по пути были и рост, и спад;
- \`↓↑\` — минимум был внутри последовательности, потом снова стало хуже;
- \`↑↑\` — лучшая точка была в самом начале, дальше в целом только хуже.

Светлый цвет означает \`1s\`, тёмный — \`2s\`.

Порядок сортировки по классу:

\`violet\`, \`green\`, \`dark-green\`, \`yellow\`, \`dark-yellow\`, \`orange\`, \`dark-orange\`, \`red\`, \`dark-red\`.

То есть от лучших классов к худшим.
`
        ),
        createColumn(
            "series-computed",
            "k",
            "k",
            "neutral",
            [
                "Число анализируемых пар `(n-1, n)` в траектории частичных сумм.",
                "",
                "Это фактически длина наблюдаемой траектории, которая участвует в анализе знаков и нарушений монотонности.",
            ].join("\n")
        ),
        createColumn(
            "series-computed",
            "sign",
            "sign",
            "lower",
            [
                "Число смен знака у `Re(S_n - S)`.",
                "",
                "Малое значение обычно соответствует более «чистому» одностороннему приближению к пределу.",
            ].join("\n")
        ),
        createColumn(
            "series-computed",
            "viol",
            "viol",
            "lower",
            String.raw`
Число ростов ошибки \(d_n = |S_n - S|\) по ходу траектории.

Если значение мало, поведение ближе к монотонному убыванию ошибки. Большое значение означает больше откатов вверх.
`
        ),
        createColumn(
            "series-computed",
            "devMin",
            "min",
            "lower",
            String.raw`
Лучшее отклонение частичных сумм от предела:

$$
\min_n |S_n - S|.
$$

Это главный столбец «насколько близко ряд вообще смог подойти к пределу».
`
        ),
        createColumn(
            "series-computed",
            "minN",
            "n min",
            "lower",
            String.raw`
Первый номер шага, на котором достигается минимум:

$$
n_{\min} = \min\{n : |S_n - S| = \min_k |S_k - S|\}.
$$

Чем меньше значение, тем раньше ряд достигает своей лучшей точки.
`
        ),
        createColumn(
            "series-computed",
            "devLast",
            "last",
            "lower",
            String.raw`
Отклонение в последней доступной точке:

$$
|S_{n_{\text{last}}} - S|.
$$

Позволяет сравнить лучшую точку ряда и его фактическое состояние в конце наблюдаемого диапазона.
`
        ),
        createColumn(
            "series-computed",
            "lastN",
            "n last",
            "neutral",
            [
                "Номер последнего доступного шага `n_last` для ряда.",
                "",
                "Нужен, чтобы понимать, на какой длине траектории измеряется `last`.",
            ].join("\n")
        ),
        createColumn(
            "series-computed",
            "lastMinusMin",
            "last-min",
            "lower",
            String.raw`
Разница между последней ошибкой и минимальной:

$$
|S_{n_{\text{last}}} - S| - \min_n |S_n - S|.
$$

**Как читать:**  
- \`0\` — ряд не потерял качество после своей лучшей точки;  
- большое положительное значение — после минимума поведение ухудшилось.
`
        ),
        createColumn(
            "series-computed",
            "ampOrders",
            "last/min amp",
            "neutral",
            String.raw`
Логарифмическая амплитуда между последней и лучшей точкой:

$$
\operatorname{lastMinAmp}
=
\log_{10}(d_{\text{last}})
-
\log_{10}(d_{\min}).
$$

В проекте используется специальное правило:

$$
\log_{10}(0) = 0.
$$

Поэтому при точном попадании в предел значение не уходит в бесконечность.  
Из-за этого знак столбца не всегда читается как «лучше/хуже» напрямую, особенно если минимум равен нулю.
`
        ),
        createColumn(
            "series-computed",
            "maxAmpOrders",
            "max/min amp",
            "neutral",
            String.raw`
Логарифмическая амплитуда между худшей и лучшей точкой траектории:

$$
\operatorname{maxMinAmp}
=
\log_{10}(d_{\max})
-
\log_{10}(d_{\min}).
$$

Здесь также используется соглашение \(\log_{10}(0)=0\).

Метрика отвечает на вопрос, **насколько широкий диапазон ошибок** вообще проходил ряд по ходу вычисления.
`
        ),
        createColumn(
            "series-computed",
            "devMean",
            "mean",
            "lower",
            String.raw`
Среднее отклонение по всем доступным точкам:

$$
\operatorname{mean}(d_n).
$$

Показывает типичный уровень ошибки по всей траектории, а не только в лучшей точке.
`
        ),
        createColumn(
            "series-computed",
            "devMedian",
            "med",
            "lower",
            String.raw`
Медиана значений \(d_n = |S_n - S|\).

В отличие от \`mean\`, медиана слабее реагирует на отдельные сильные выбросы и даёт более устойчивую оценку «типичной» ошибки.
`
        ),
        createColumn(
            "series-computed",
            "devMax",
            "max",
            "lower",
            String.raw`
Максимальное отклонение по траектории:

$$
\max_n |S_n - S|.
$$

Это худшая наблюдаемая точка ряда относительно предела.
`
        ),
    ],
    xlsxSheets: [
        {
            name: "overview",
            description: "Параметры экспорта и активные пороги классификации.",
            markdown: [
                "Содержит число строк, число строк до фильтрации, активные пороги `max sign changes` и `max violations`, сортировку и имя выбранного ряда.",
            ].join("\n"),
        },
        {
            name: "summary",
            description: "Сводная таблица по всем отфильтрованным рядам.",
            markdown: [
                "Повторяет экранную таблицу, но в xlsx класс уже разложен на `side` и `class`.",
                "",
                "Лист содержит `limit`, статистики отклонения, число шагов, количество смен знака и нарушений монотонности.",
            ].join("\n"),
        },
        {
            name: "selected_meta",
            description: "Подробная мета-информация по выбранному ряду.",
            markdown: [
                "Появляется только если на экране выбран конкретный ряд.",
                "",
                "Содержит предел, класс, описание класса, минимум, последнюю точку, амплитуды и итоговые статистики.",
            ].join("\n"),
        },
        {
            name: "selected_points",
            description: "Все вычисленные точки частичных сумм для выбранного ряда.",
            markdown: [
                "Лист содержит `n`, действительную и мнимую части `S_n`, норму ошибки `|S_n-S|` и signed-вариант ошибки.",
            ].join("\n"),
        },
        {
            name: "selected_diffs",
            description: "Разности между соседними частичными суммами.",
            markdown: [
                "Содержит `S_n - S_{n-1}` по выбранному ряду и позволяет отдельно анализировать величину шага между соседними точками.",
            ].join("\n"),
        },
    ],
};

export const TABLE_DOCS: readonly [
    DocsTableSection<AlgoRankingDocsColumnKey>,
    DocsTableSection<SeriesComputedDocsColumnKey>,
] = [ALGO_RANKING_TABLE_DOCS, SERIES_COMPUTED_TABLE_DOCS];

const TABLE_DOCS_BY_KEY = Object.fromEntries(TABLE_DOCS.map((table) => [table.key, table])) as {
    "algo-ranking": DocsTableSection<AlgoRankingDocsColumnKey>;
    "series-computed": DocsTableSection<SeriesComputedDocsColumnKey>;
};

export function buildDocsHref(anchorId: string): string {
    return `/docs#${anchorId}`;
}

export function getTableDocs<TKey extends DocumentedTableKey>(
    key: TKey
): (typeof TABLE_DOCS_BY_KEY)[TKey] {
    return TABLE_DOCS_BY_KEY[key];
}

export function getAlgoRankingColumnAnchorId(key: AlgoRankingDocsColumnKey): string {
    return getTableDocs("algo-ranking").columns.find((column) => column.key === key)!.id;
}

export function getSeriesComputedColumnAnchorId(key: SeriesComputedDocsColumnKey): string {
    return getTableDocs("series-computed").columns.find((column) => column.key === key)!.id;
}

export function listAllDocsAnchorIds(): string[] {
    const ids = [DOCS_HOME_ANCHOR_ID];

    for (const table of TABLE_DOCS) {
        ids.push(table.id, table.screenSectionId, table.xlsxSectionId);
        for (const column of table.columns) ids.push(column.id);
    }

    return ids;
}

export function getDocsColumnPreferenceLabel(
    preference: DocsColumnPreference
): string | null {
    switch (preference) {
        case "lower":
            return "Меньше лучше";
        case "higher":
            return "Больше лучше";
        case "neutral":
        default:
            return null;
    }
}
