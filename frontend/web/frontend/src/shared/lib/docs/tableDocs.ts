export const DOCS_HOME_ANCHOR_ID = "docs-overview";

export type DocumentedTableKey =
    | "algo-ranking"
    | "series-computed"
    | "algo-series-convergence";

export type DocsColumnPreference = "lower" | "higher" | "neutral";

export type AlgoRankingDocsColumnKey =
    | "precision"
    | "m"
    | "levinEnding"
    | "args"
    | "howMuchFormula"
    | "complexityFormula"
    | "seriesCount"
    | "avgBestDeviation"
    | "medianBestDeviation"
    | "worstBestDeviation"
    | "avgRelativeError"
    | "medianRelativeError"
    | "worstRelativeError"
    | "avgOrdersGain"
    | "medianOrdersGain"
    | "worstOrdersGain"
    | "avgAmpAtMinN"
    | "medianAmpAtMinN"
    | "worstAmpAtMinN"
    | "avgStepSeriesAmp"
    | "medianStepSeriesAmp"
    | "worstStepSeriesAmp"
    | "avgFilterTriggerN"
    | "medianFilterTriggerN"
    | "worstFilterTriggerN"
    | "avgFilterTriggerDeltaFromMinN"
    | "medianFilterTriggerDeltaFromMinN"
    | "worstFilterTriggerDeltaFromMinN"
    | "avgFilterTriggerLossAmp"
    | "medianFilterTriggerLossAmp"
    | "worstFilterTriggerLossAmp"
    | "avgFilterTriggerLossDiff"
    | "medianFilterTriggerLossDiff"
    | "worstFilterTriggerLossDiff"
    | "notBetterThanSeriesShare"
    | "avgMinDeviationN"
    | "medianMinDeviationN"
    | "worstMinDeviationN"
    | "avgLastMinusMin"
    | "medianLastMinusMin"
    | "worstLastMinusMin"
    | "avgStepsToTol"
    | "medianStepsToTol"
    | "worstStepsToTol"
    | "avgEpsSavedSteps"
    | "medianEpsSavedSteps"
    | "worstEpsSavedSteps"
    | "avgMinDeviationNComplexity"
    | "medianMinDeviationNComplexity"
    | "avgStepsToTolComplexity"
    | "medianStepsToTolComplexity"
    | "fracReachedTol"
    | "divZeroShare"
    | "avgDivZeroFirstN"
    | "medianDivZeroFirstN"
    | "worstDivZeroFirstN"
    | "divZeroRecoveredShare"
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

export type AlgoSeriesConvergenceDocsColumnKey =
    | "axes"
    | "cell"
    | "thresholds"
    | "class"
    | "selectedDetail";

export interface DocsColumnSection<TColumnKey extends string = string> {
    key: TColumnKey;
    id: string;
    title: string;
    preference: DocsColumnPreference;
    markdown: string;
}

export interface DocsXlsxFieldInfo {
    key: string;
    id: string;
    name: string;
    markdown: string;
    preference?: DocsColumnPreference;
    refAnchorId?: string;
}

export interface DocsXlsxSheetInfo {
    key: string;
    id: string;
    name: string;
    description: string;
    markdown: string;
    fields: readonly DocsXlsxFieldInfo[];
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

function buildXlsxSheetAnchorId(key: DocumentedTableKey, sheetKey: string): string {
    return `docs-${key}-sheet-${sheetKey}`;
}

function buildXlsxFieldAnchorId(
    key: DocumentedTableKey,
    sheetKey: string,
    fieldKey: string
): string {
    return `${buildXlsxSheetAnchorId(key, sheetKey)}-field-${fieldKey}`;
}

function buildColumnAnchorId(key: DocumentedTableKey, columnKey: string): string {
    return `docs-${key}-col-${columnKey}`;
}

function buildArgMarkdown(): string {
    return [
        "Справка для arg-колонок рейтинга алгоритмов. Экранная таблица строит отдельные колонки по реальным ключам `args`: например `filter_method`, `filter_window_length`, `alpha`.",
        "",
        "Значение в ячейке — это `args[key]` для конкретного варианта алгоритма. Если у алгоритма нет такого аргумента, в таблице будет `-`, а в xlsx — пустое значение.",
        "",
        "По таким колонкам удобно сортировать и сравнивать варианты одного алгоритма с разными параметрами фильтра или дополнительными аргументами. **Интерпретация:** это идентификатор конфигурации алгоритма, а не метрика качества.",
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

function createXlsxField(
    tableKey: DocumentedTableKey,
    sheetKey: string,
    key: string,
    name: string,
    markdown: string,
    options?: {
        preference?: DocsColumnPreference;
        refAnchorId?: string;
    }
): DocsXlsxFieldInfo {
    return {
        key,
        id: buildXlsxFieldAnchorId(tableKey, sheetKey, key),
        name,
        markdown,
        preference: options?.preference,
        refAnchorId: options?.refAnchorId,
    };
}

function createXlsxSheet(
    tableKey: DocumentedTableKey,
    key: string,
    name: string,
    description: string,
    markdown: string,
    fields: readonly DocsXlsxFieldInfo[]
): DocsXlsxSheetInfo {
    return {
        key,
        id: buildXlsxSheetAnchorId(tableKey, key),
        name,
        description,
        markdown,
        fields,
    };
}

function createKeyValueSheet(
    tableKey: DocumentedTableKey,
    key: string,
    name: string,
    description: string,
    markdown: string,
    fields: readonly DocsXlsxFieldInfo[]
): DocsXlsxSheetInfo {
    return createXlsxSheet(tableKey, key, name, description, markdown, [
        createXlsxField(
            tableKey,
            key,
            "field",
            "field",
            [
                "Первая колонка key-value листа.",
                "",
                "Содержит имя параметра или метрики, записанной в текущей строке.",
            ].join("\n")
        ),
        createXlsxField(
            tableKey,
            key,
            "value",
            "value",
            [
                "Вторая колонка key-value листа.",
                "",
                "Содержит значение параметра или метрики из колонки `field`.",
            ].join("\n")
        ),
        ...fields,
    ]);
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
        createColumn(
            "algo-ranking",
            "levinEnding",
            "Levin end",
            "neutral",
            "Levin-family ending extracted into a separate column: `U`, `T`, `T~`, `V`, or `V~`. The displayed algorithm name is stripped of that suffix."
        ),
        createColumn("algo-ranking", "args", "args.<key>", "neutral", buildArgMarkdown()),
        createColumn(
            "algo-ranking",
            "howMuchFormula",
            "how_much formula",
            "neutral",
            "Formula used to map algorithm step `n` to source sequence index `S(how_much(n, order, args))` for this algorithm row."
        ),
        createColumn(
            "algo-ranking",
            "complexityFormula",
            "complexity formula",
            "neutral",
            "Formula used for complexity score: `how_much(n)` plus the algorithm `O(...)` expression evaluated at the row's `m` and the metric's `n`."
        ),
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
            "medianBestDeviation",
            "med min |dev|",
            "lower",
            "Медиана по рядам от `min |A_n - S|`. Меньше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstBestDeviation",
            "worst min |dev|",
            "lower",
            "Худший случай по рядам для `min |A_n - S|`. Меньше лучше."
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
            "medianRelativeError",
            "med rel error",
            "lower",
            "Медиана по рядам от `min |A_n - S| / min |S_n - S|`. Меньше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstRelativeError",
            "worst rel error",
            "lower",
            "Худший случай по рядам для относительной ошибки. Меньше лучше."
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
            "medianOrdersGain",
            "med series/algo amp",
            "higher",
            "Медиана выигрыша по порядкам `log10(min series) - log10(min algo)`. Больше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstOrdersGain",
            "worst series/algo amp",
            "higher",
            "Худший выигрыш по порядкам среди рядов. Больше лучше."
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
\log_{10}|S_{\operatorname{how\_much}(n_A^\*)} - S|
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
            "medianAmpAtMinN",
            "med series@min n/algo amp",
            "higher",
            "Медиана выигрыша по порядкам против `S(how_much(n_min))`. Больше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstAmpAtMinN",
            "worst series@min n/algo amp",
            "higher",
            "Худший выигрыш по порядкам против `S(how_much(n_min))`. Больше лучше."
        ),
        createColumn(
            "algo-ranking",
            "avgStepSeriesAmp",
            "avg step S*/A-S amp",
            "higher",
            "For each accelerated step n, compute `log10(|S_how_much(n)-S|) - log10(|A_n - S_how_much(n)|)`, average inside each series, then average across series. Higher is better."
        ),
        createColumn(
            "algo-ranking",
            "medianStepSeriesAmp",
            "med step S*/A-S amp",
            "higher",
            "For each series, take the median step gain `log10(|S_how_much(n)-S|) - log10(|A_n - S_how_much(n)|)`, then take the median across series. Higher is better and participates in `rank precision`."
        ),
        createColumn(
            "algo-ranking",
            "worstStepSeriesAmp",
            "worst step S*/A-S amp",
            "higher",
            "For each series, take the worst step gain against `|A_n - S_how_much(n)|`, then take the worst across series. Higher is better and participates in `rank precision`."
        ),
        createColumn(
            "algo-ranking",
            "avgFilterTriggerN",
            "avg filter n",
            "lower",
            "Average first event n whose name or description contains `Filters triggered due to`. Missing samples are displayed as `-`."
        ),
        createColumn(
            "algo-ranking",
            "medianFilterTriggerN",
            "med filter n",
            "lower",
            "Median first filter-trigger event n. Lower is better; missing samples rank as worst for trigger metrics."
        ),
        createColumn(
            "algo-ranking",
            "worstFilterTriggerN",
            "worst filter n",
            "lower",
            "Worst first filter-trigger event n across series. Lower is better."
        ),
        createColumn(
            "algo-ranking",
            "avgFilterTriggerDeltaFromMinN",
            "avg filter-min n",
            "lower",
            "Average difference `filterTriggerN - minDeviationN`. Lower means filters are triggered closer to the algorithm minimum."
        ),
        createColumn(
            "algo-ranking",
            "medianFilterTriggerDeltaFromMinN",
            "med filter-min n",
            "lower",
            "Median difference `filterTriggerN - minDeviationN`. Lower is better; missing samples rank as worst."
        ),
        createColumn(
            "algo-ranking",
            "worstFilterTriggerDeltaFromMinN",
            "worst filter-min n",
            "lower",
            "Worst difference `filterTriggerN - minDeviationN` across series. Lower is better."
        ),
        createColumn(
            "algo-ranking",
            "avgFilterTriggerLossAmp",
            "avg filter loss amp",
            "lower",
            "Average `log10(|A_filter_n-S|) - log10(min |A_n-S|)`. Shows how many orders were lost when filters triggered."
        ),
        createColumn(
            "algo-ranking",
            "medianFilterTriggerLossAmp",
            "med filter loss amp",
            "lower",
            "Median `log10(|A_filter_n-S|) - log10(min |A_n-S|)`. Lower is better."
        ),
        createColumn(
            "algo-ranking",
            "worstFilterTriggerLossAmp",
            "worst filter loss amp",
            "lower",
            "Worst `log10(|A_filter_n-S|) - log10(min |A_n-S|)` across series. Lower is better."
        ),
        createColumn(
            "algo-ranking",
            "avgFilterTriggerLossDiff",
            "avg filter loss diff",
            "lower",
            "Average ordinary loss at the first filter trigger: `|A_filter_n-S| - min |A_n-S|`. Lower is better and avoids log infinities."
        ),
        createColumn(
            "algo-ranking",
            "medianFilterTriggerLossDiff",
            "med filter loss diff",
            "lower",
            "Median `|A_filter_n-S| - min |A_n-S|` at the first filter trigger. Lower is better."
        ),
        createColumn(
            "algo-ranking",
            "worstFilterTriggerLossDiff",
            "worst filter loss diff",
            "lower",
            "Worst `|A_filter_n-S| - min |A_n-S|` at the first filter trigger across series. Lower is better."
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
            "medianMinDeviationN",
            "med min dev n",
            "lower",
            "Медиана шага, на котором алгоритм впервые достигает своего минимума. Меньше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstMinDeviationN",
            "worst min dev n",
            "lower",
            "Худший шаг достижения минимума среди рядов. Меньше лучше."
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
            "medianLastMinusMin",
            "med last-min",
            "lower",
            "Медиана ухода от лучшей точки к последней. Меньше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstLastMinusMin",
            "worst last-min",
            "lower",
            "Худший уход от лучшей точки к последней. Меньше лучше."
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
            "medianStepsToTol",
            "med steps to eps",
            "lower",
            "Медиана первого шага достижения `epsilon`. Меньше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstStepsToTol",
            "worst steps to eps",
            "lower",
            "Худший первый шаг достижения `epsilon`. Меньше лучше."
        ),
        createColumn(
            "algo-ranking",
            "avgEpsSavedSteps",
            "avg eps saved steps",
            "higher",
            "`mean(n_series_eps - how_much(n_algo_eps, order, args))`. Больше значит больше сэкономленных членов ряда."
        ),
        createColumn(
            "algo-ranking",
            "medianEpsSavedSteps",
            "med eps saved steps",
            "higher",
            "`median(n_series_eps - how_much(n_algo_eps, order, args))`. Больше лучше."
        ),
        createColumn(
            "algo-ranking",
            "worstEpsSavedSteps",
            "worst eps saved steps",
            "higher",
            "Худший случай для `n_series_eps - how_much(n_algo_eps, order, args)`. Больше лучше."
        ),
        createColumn(
            "algo-ranking",
            "avgMinDeviationNComplexity",
            "avg min dev complexity",
            "lower",
            "`how_much(avg min dev n) + O(m,n)` evaluated for the row's algorithm. This diagnostic does not participate in ranks."
        ),
        createColumn(
            "algo-ranking",
            "medianMinDeviationNComplexity",
            "med min dev complexity",
            "lower",
            "`how_much(med min dev n) + O(m,n)` evaluated for the row's algorithm. This diagnostic does not participate in ranks."
        ),
        createColumn(
            "algo-ranking",
            "avgStepsToTolComplexity",
            "avg eps complexity",
            "lower",
            "`how_much(avg steps to eps) + O(m,n)` evaluated for the row's algorithm. This diagnostic does not participate in ranks."
        ),
        createColumn(
            "algo-ranking",
            "medianStepsToTolComplexity",
            "med eps complexity",
            "lower",
            "`how_much(med steps to eps) + O(m,n)` evaluated for the row's algorithm. This diagnostic does not participate in ranks."
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
            "divZeroShare",
            "div0, %",
            "lower",
            "Share of algorithm runs with a division-by-zero error message. Only explicit div-by-zero messages are counted."
        ),
        createColumn(
            "algo-ranking",
            "avgDivZeroFirstN",
            "avg div0 first n",
            "lower",
            "Average first finite `n` where a div-by-zero error appears for each affected series."
        ),
        createColumn(
            "algo-ranking",
            "medianDivZeroFirstN",
            "med div0 first n",
            "lower",
            "Median first finite `n` where a div-by-zero error appears for each affected series."
        ),
        createColumn(
            "algo-ranking",
            "worstDivZeroFirstN",
            "worst div0 first n",
            "lower",
            "Maximum first finite div-zero `n` across affected series; this is not the last div-zero event."
        ),
        createColumn(
            "algo-ranking",
            "divZeroRecoveredShare",
            "div0 recovered, %",
            "higher",
            "Among runs with finite div-zero `n`, share that later had a successful computed point after the last div-zero `n`."
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
Сводный ранг по точности. \`avg*\`-столбцы остаются диагностикой, а итоговый ранг использует более устойчивые median/worst-метрики:

- \`med min |dev|\` и \`worst min |dev|\`,
- \`med rel error\` и \`worst rel error\`,
- \`med series/algo amp\` и \`worst series/algo amp\`,
- \`med step S*/A-S amp\` и \`worst step S*/A-S amp\`,
- \`min algo >= min series, %\`,
- \`best min div, %\`,
- \`worst min div, %\`.

После этого ранги суммируются:

$$
\operatorname{rankPrecision}
=
\rho(\text{medianMinDev})
\!+\!
\rho(\text{worstMinDev})
\!+\!
\rho(\text{medianRelError})
\!+\!
\rho(\text{worstRelError})
\!+\!
\rho(\text{medianSeriesAlgoAmp})
\!+\!
\rho(\text{worstSeriesAlgoAmp})
\!+\!
\rho(\text{medianStepSeriesAmp})
\!+\!
\rho(\text{worstStepSeriesAmp})
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
Сводный ранг по скорости достижения хороших значений. \`avg*\`-столбцы остаются диагностикой, а итоговый ранг использует median/worst-метрики:

$$
\operatorname{rankSpeed}
=
\rho(\text{medianStepsToEps})
\!+\!
\rho(\text{worstStepsToEps})
\!+\!
\rho(\text{medianMinDevN})
\!+\!
\rho(\text{worstMinDevN})
\!+\!
\rho(\text{medianEpsSavedSteps})
\!+\!
\rho(\text{worstEpsSavedSteps})
\!+\!
\rho(\text{medianAmpAtMinN})
\!+\!
\rho(\text{worstAmpAtMinN})
\!+\!
\rho(\text{medianFilterTriggerN})
\!+\!
\rho(\text{worstFilterTriggerN})
\!+\!
\rho(\text{medianFilterTriggerDeltaFromMinN})
\!+\!
\rho(\text{worstFilterTriggerDeltaFromMinN})
\!+\!
\rho(\text{medianFilterTriggerLossAmp})
\!+\!
\rho(\text{worstFilterTriggerLossAmp}).
$$

\`median/worst eps saved steps\` rank \`n_series_eps - how_much(n_algo_eps, order, args)\`: higher saved steps gets a better dense rank before the sum.

Чем меньше значение, тем раньше алгоритм выходит на минимум, быстрее достигает \`epsilon\`, дает лучший \`series@min n/algo amp\` и раньше/ближе к минимуму показывает filter trigger.
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
        createKeyValueSheet(
            "algo-ranking",
            "overview",
            "overview",
            "Параметры текущего экспорта.",
            [
                "Содержит служебную информацию о том, **как именно** был собран файл:",
                "",
                "- выбранный `epsilon` и его показатель степени;",
                "- фильтр по `precision`;",
                "- активный ключ сортировки и направление;",
                "- число строк и число экспортированных колонок.",
            ].join("\n"),
            [
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "epsilon",
                    "epsilon",
                    [
                        "Числовое значение `ε`, использованное для метрик `avg steps to eps` и `reached eps, %`.",
                        "",
                        "Это тот же порог точности, который выбран на экране перед экспортом.",
                    ].join("\n")
                ),
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "epsilonExponent",
                    "epsilon exponent",
                    [
                        "Показатель степени для `epsilon`.",
                        "",
                        "Например, `-12` означает `epsilon = 10^{-12}`.",
                    ].join("\n")
                ),
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "precisionFilter",
                    "precision filter",
                    "Текущее значение фильтра `precision` во время экспорта. `all` означает, что фильтр не был ограничен одним precision."
                ),
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "sortKey",
                    "sort key",
                    "Идентификатор колонки, по которой была отсортирована экранная таблица перед выгрузкой в xlsx."
                ),
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "sortDir",
                    "sort dir",
                    "Направление текущей сортировки: `asc` для возрастания или `desc` для убывания."
                ),
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "rows",
                    "rows",
                    "Число алгоритмов, попавших в лист `algo_ranking` после всех фильтров."
                ),
                createXlsxField(
                    "algo-ranking",
                    "overview",
                    "columns",
                    "columns",
                    "Общее число колонок на листе `algo_ranking`, включая служебные `place` и `algorithm`."
                ),
            ]
        ),
        createXlsxSheet(
            "algo-ranking",
            "algo-ranking",
            "algo_ranking",
            "Основная сводная таблица по алгоритмам.",
            [
                "Повторяет экранную таблицу для текущего набора фильтров.",
                "",
                "Первые колонки — `place` и `algorithm`, дальше идут те же метрики, что и на экране. Форматы чисел и процентов уже проставлены в самом xlsx.",
            ].join("\n"),
            [
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "place",
                    "place",
                    [
                        "Позиция алгоритма в текущей сортировке выгруженной таблицы.",
                        "",
                        "`1` означает первую строку после применения текущих фильтров и сортировки.",
                    ].join("\n"),
                    { preference: "lower" }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "algorithm",
                    "algorithm",
                    "Имя алгоритма или ускорителя, для которого посчитана строка рейтинга."
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "precision",
                    "precision",
                    "Precision рядов, по которым агрегирована строка алгоритма в xlsx.",
                    {
                        refAnchorId: buildColumnAnchorId("algo-ranking", "precision"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "m",
                    "m",
                    "Параметр `m` алгоритма.",
                    {
                        refAnchorId: buildColumnAnchorId("algo-ranking", "m"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "levinEnding",
                    "Levin end",
                    "Levin-family ending extracted from the algorithm name or args: `U`, `T`, `T~`, `V`, or `V~`.",
                    {
                        refAnchorId: buildColumnAnchorId("algo-ranking", "levinEnding"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "args",
                    "args.<key>",
                    "Динамические колонки аргументов алгоритма. Вместо `<key>` в xlsx будет реальное имя аргумента, например `args.filter_method` или `args.alpha`.",
                    {
                        refAnchorId: buildColumnAnchorId("algo-ranking", "args"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "howMuchFormula",
                    "how_much formula",
                    "Formula used to map algorithm step `n` to source sequence index `S(how_much(n, order, args))`.",
                    {
                        refAnchorId: buildColumnAnchorId("algo-ranking", "howMuchFormula"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "complexityFormula",
                    "complexity formula",
                    "`how_much(n) + O(...)` formula used by the complexity score columns.",
                    {
                        refAnchorId: buildColumnAnchorId("algo-ranking", "complexityFormula"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "seriesCount",
                    "series",
                    "Число уникальных рядов, по которым этот алгоритм удалось сравнить.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "seriesCount"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgBestDeviation",
                    "avg min |dev|",
                    "Среднее по рядам значение `min |A_n - lim|` для алгоритма.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgBestDeviation"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianBestDeviation",
                    "med min |dev|",
                    "Медиана по рядам от `min |A_n - lim|`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianBestDeviation"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstBestDeviation",
                    "worst min |dev|",
                    "Худший случай по рядам для `min |A_n - lim|`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstBestDeviation"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgRelativeError",
                    "avg rel error",
                    [
                        "Среднее отношение `min |A_n - lim| / min |S_n - lim|`.",
                        "",
                        "В среднее входят только ряды, где `min |S_n - lim| > 0`: `1` означает уровень ряда, `< 1` лучше ряда, `> 1` хуже ряда.",
                    ].join("\n"),
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgRelativeError"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianRelativeError",
                    "med rel error",
                    "Медиана по рядам от `min |A_n - lim| / min |S_n - lim|`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianRelativeError"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstRelativeError",
                    "worst rel error",
                    "Худший случай по рядам для относительной ошибки.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstRelativeError"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgOrdersGain",
                    "avg series/algo amp",
                    [
                        "Средний выигрыш по порядкам между минимумом ряда и минимумом алгоритма.",
                        "",
                        "Считается как `log10(min series) - log10(min algo)`. Положительное значение означает, что алгоритм точнее ряда.",
                    ].join("\n"),
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgOrdersGain"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianOrdersGain",
                    "med series/algo amp",
                    "Медиана выигрыша по порядкам `log10(min series) - log10(min algo)`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianOrdersGain"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstOrdersGain",
                    "worst series/algo amp",
                    "Худший выигрыш по порядкам среди рядов.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstOrdersGain"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgAmpAtMinN",
                    "avg series@min n/algo amp",
                    [
                        "Средний выигрыш по порядкам между минимумом алгоритма и ошибкой ряда на первом `n`, где алгоритм достиг своего минимума.",
                        "",
                        "Положительное значение означает преимущество алгоритма именно в точке его лучшего шага.",
                    ].join("\n"),
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgAmpAtMinN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianAmpAtMinN",
                    "med series@min n/algo amp",
                    "Медиана выигрыша по порядкам против `S(how_much(n_min))`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianAmpAtMinN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstAmpAtMinN",
                    "worst series@min n/algo amp",
                    "Худший выигрыш по порядкам против `S(how_much(n_min))`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstAmpAtMinN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgStepSeriesAmp",
                    "avg step S*/A-S amp",
                    "Average per-series step gain against `|A_n - S_how_much(n)|`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgStepSeriesAmp"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianStepSeriesAmp",
                    "med step S*/A-S amp",
                    "Median per-series step gain against `|A_n - S_how_much(n)|`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianStepSeriesAmp"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstStepSeriesAmp",
                    "worst step S*/A-S amp",
                    "Worst per-series step gain against `|A_n - S_how_much(n)|`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstStepSeriesAmp"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgFilterTriggerN",
                    "avg filter n",
                    "Average first event n whose name or description contains `Filters triggered due to`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgFilterTriggerN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianFilterTriggerN",
                    "med filter n",
                    "Median first filter-trigger event n.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianFilterTriggerN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstFilterTriggerN",
                    "worst filter n",
                    "Worst first filter-trigger event n across series.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstFilterTriggerN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgFilterTriggerDeltaFromMinN",
                    "avg filter-min n",
                    "Average difference `filterTriggerN - minDeviationN`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "avgFilterTriggerDeltaFromMinN"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianFilterTriggerDeltaFromMinN",
                    "med filter-min n",
                    "Median difference `filterTriggerN - minDeviationN`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "medianFilterTriggerDeltaFromMinN"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstFilterTriggerDeltaFromMinN",
                    "worst filter-min n",
                    "Worst difference `filterTriggerN - minDeviationN` across series.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "worstFilterTriggerDeltaFromMinN"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgFilterTriggerLossAmp",
                    "avg filter loss amp",
                    "Average `log10(|A_filter_n-S|) - log10(min |A_n-S|)`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "avgFilterTriggerLossAmp"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianFilterTriggerLossAmp",
                    "med filter loss amp",
                    "Median `log10(|A_filter_n-S|) - log10(min |A_n-S|)`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "medianFilterTriggerLossAmp"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstFilterTriggerLossAmp",
                    "worst filter loss amp",
                    "Worst `log10(|A_filter_n-S|) - log10(min |A_n-S|)` across series.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "worstFilterTriggerLossAmp"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgFilterTriggerLossDiff",
                    "avg filter loss diff",
                    "Average `|A_filter_n-S| - min |A_n-S|` at the first filter trigger.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "avgFilterTriggerLossDiff"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianFilterTriggerLossDiff",
                    "med filter loss diff",
                    "Median `|A_filter_n-S| - min |A_n-S|` at the first filter trigger.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "medianFilterTriggerLossDiff"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstFilterTriggerLossDiff",
                    "worst filter loss diff",
                    "Worst `|A_filter_n-S| - min |A_n-S|` at the first filter trigger.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "worstFilterTriggerLossDiff"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "notBetterThanSeriesShare",
                    "min algo >= min series, %",
                    "Доля рядов, где минимум алгоритма не оказался лучше минимума исходного ряда.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "notBetterThanSeriesShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgMinDeviationN",
                    "avg min dev n",
                    "Средний номер шага `n`, на котором алгоритм впервые достигает своего минимального отклонения.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgMinDeviationN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianMinDeviationN",
                    "med min dev n",
                    "Медиана шага, на котором алгоритм впервые достигает своего минимума.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianMinDeviationN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstMinDeviationN",
                    "worst min dev n",
                    "Худший шаг достижения минимума среди рядов.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstMinDeviationN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgLastMinusMin",
                    "avg last-min",
                    "Среднее значение `|A_last-lim| - min |A_n-lim|`, то есть насколько алгоритм уходит от своего минимума к концу траектории.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgLastMinusMin"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianLastMinusMin",
                    "med last-min",
                    "Медиана ухода от лучшей точки к последней.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianLastMinusMin"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstLastMinusMin",
                    "worst last-min",
                    "Худший уход от лучшей точки к последней.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstLastMinusMin"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgStepsToTol",
                    "avg steps to eps",
                    "Среднее число шагов до первого достижения порога `epsilon`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgStepsToTol"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianStepsToTol",
                    "med steps to eps",
                    "Медиана первого шага достижения `epsilon`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianStepsToTol"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstStepsToTol",
                    "worst steps to eps",
                    "Худший первый шаг достижения `epsilon`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstStepsToTol"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgEpsSavedSteps",
                    "avg eps saved steps",
                    "`mean(n_series_eps - how_much(n_algo_eps, order, args))`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgEpsSavedSteps"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianEpsSavedSteps",
                    "med eps saved steps",
                    "`median(n_series_eps - how_much(n_algo_eps, order, args))`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianEpsSavedSteps"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstEpsSavedSteps",
                    "worst eps saved steps",
                    "Худший случай для `n_series_eps - how_much(n_algo_eps, order, args)`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstEpsSavedSteps"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgMinDeviationNComplexity",
                    "avg min dev complexity",
                    "`how_much(avg min dev n) + O(m,n)` for this algorithm row.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "avgMinDeviationNComplexity"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianMinDeviationNComplexity",
                    "med min dev complexity",
                    "`how_much(med min dev n) + O(m,n)` for this algorithm row.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "medianMinDeviationNComplexity"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgStepsToTolComplexity",
                    "avg eps complexity",
                    "`how_much(avg steps to eps) + O(m,n)` for this algorithm row.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "avgStepsToTolComplexity"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianStepsToTolComplexity",
                    "med eps complexity",
                    "`how_much(med steps to eps) + O(m,n)` for this algorithm row.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId(
                            "algo-ranking",
                            "medianStepsToTolComplexity"
                        ),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "fracReachedTol",
                    "reached eps, %",
                    "Доля рядов, на которых алгоритм вообще достиг заданного `epsilon`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "fracReachedTol"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "divZeroShare",
                    "div0, %",
                    "Share of runs with an explicit division-by-zero error message.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "divZeroShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "avgDivZeroFirstN",
                    "avg div0 first n",
                    "Average first finite `n` where div-zero appears in affected runs.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "avgDivZeroFirstN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "medianDivZeroFirstN",
                    "med div0 first n",
                    "Median first finite `n` where div-zero appears in affected runs.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "medianDivZeroFirstN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstDivZeroFirstN",
                    "worst div0 first n",
                    "Maximum first finite div-zero `n` across affected runs.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstDivZeroFirstN"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "divZeroRecoveredShare",
                    "div0 recovered, %",
                    "Among runs with finite div-zero `n`, share with a later successful computed point after the last div-zero `n`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "divZeroRecoveredShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "oneSidedShare",
                    "1-sided, %",
                    "Доля запусков, в которых алгоритм шёл к пределу односторонне. Для класса `→` это тоже считается как `1-sided`.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "oneSidedShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "bestMinShare",
                    "best min div, %",
                    "Доля рядов, где минимум алгоритма был лучшим среди всех алгоритмов. При tie-case сумма процентов по столбцу может быть больше 100%.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "bestMinShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstMinShare",
                    "worst min div, %",
                    "Доля рядов, где минимум алгоритма был худшим среди всех алгоритмов. При tie-case сумма процентов по столбцу тоже может быть больше 100%.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstMinShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "bestLastShare",
                    "best last div, %",
                    "Доля рядов, где последняя точка алгоритма была лучшей среди всех алгоритмов.",
                    {
                        preference: "higher",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "bestLastShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "worstLastShare",
                    "worst last div, %",
                    "Доля рядов, где последняя точка алгоритма была худшей среди всех алгоритмов.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "worstLastShare"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "rankPrecision",
                    "rank precision",
                    "Итоговый ранг по точности, собранный из точностных метрик. Меньше означает лучшее суммарное место.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "rankPrecision"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "rankSpeed",
                    "rank speed",
                    "Итоговый ранг по скорости: `avg min dev n`, `avg steps to eps` и `avg eps saved steps = mean(n_series_eps - how_much(n_algo_eps, order, args))`. Меньше лучше.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "rankSpeed"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "rankStability",
                    "rank stability",
                    "Итоговый ранг по устойчивости траектории: односторонность, удержание минимума и поведение последней точки.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "rankStability"),
                    }
                ),
                createXlsxField(
                    "algo-ranking",
                    "algo-ranking",
                    "totalRankScore",
                    "total rank",
                    "Общий итоговый ранг алгоритма. Это сумма `rank precision`, `rank speed` и `rank stability`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("algo-ranking", "totalRankScore"),
                    }
                ),
            ]
        ),
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
        createKeyValueSheet(
            "series-computed",
            "overview",
            "overview",
            "Параметры экспорта и активные пороги классификации.",
            [
                "Содержит число строк, число строк до фильтрации, активные пороги `max sign changes` и `max violations`, сортировку и имя выбранного ряда.",
            ].join("\n"),
            [
                createXlsxField(
                    "series-computed",
                    "overview",
                    "rows",
                    "rows",
                    "Число рядов, попавших в лист `summary` после всех фильтров."
                ),
                createXlsxField(
                    "series-computed",
                    "overview",
                    "totalRowsBeforeFilter",
                    "total rows before filter",
                    "Число рядов до применения текущей экранной фильтрации."
                ),
                createXlsxField(
                    "series-computed",
                    "overview",
                    "maxSignChanges",
                    "max sign changes",
                    "Текущее значение порога, которое определяет, сколько смен знака ещё допускается для классификации `1s`."
                ),
                createXlsxField(
                    "series-computed",
                    "overview",
                    "maxViolations",
                    "max violations",
                    "Текущее значение порога, которое определяет, сколько ростов ошибки ещё допускается для почти монотонной траектории."
                ),
                createXlsxField(
                    "series-computed",
                    "overview",
                    "sort",
                    "sort",
                    "Текущая сортировка экранной таблицы, с которой был построен xlsx."
                ),
                createXlsxField(
                    "series-computed",
                    "overview",
                    "selectedSeries",
                    "selected series",
                    "Имя ряда, выбранного на экране для detail-экспорта. `none` означает, что листы `selected_*` не добавлялись."
                ),
            ]
        ),
        createXlsxSheet(
            "series-computed",
            "summary",
            "summary",
            "Сводная таблица по всем отфильтрованным рядам.",
            [
                "Повторяет экранную таблицу, но в xlsx класс уже разложен на `side` и `class`.",
                "",
                "Лист содержит `limit`, статистики отклонения, число шагов, количество смен знака и нарушений монотонности.",
            ].join("\n"),
            [
                createXlsxField("series-computed", "summary", "rowNumber", "#", "Порядковый номер строки на листе `summary`."),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "series",
                    "series",
                    "Имя ряда.",
                    { refAnchorId: buildColumnAnchorId("series-computed", "name") }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "precision",
                    "precision",
                    "Precision выбранного ряда.",
                    { refAnchorId: buildColumnAnchorId("series-computed", "precision") }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "args",
                    "args",
                    "Сводка параметров ряда в виде строки `key=value`.",
                    { refAnchorId: buildColumnAnchorId("series-computed", "args") }
                ),
                createXlsxField("series-computed", "summary", "limit", "limit", "Предел `S`, относительно которого измеряются все отклонения ряда."),
                createXlsxField("series-computed", "summary", "side", "side", "Сторона поведения в xlsx: `1s`, `2s` или `?`. Для класса `→` экспортируется `1s`."),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "class",
                    "class",
                    "Короткий символ класса поведения: `→`, `↓→`, `↓↓`, `↑↓`, `↓↑` или `↑↑`.",
                    { refAnchorId: buildColumnAnchorId("series-computed", "class") }
                ),
                createXlsxField("series-computed", "summary", "classTitle", "class title", "Текстовое название класса, удобное для чтения вне цветовой легенды."),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "steps",
                    "steps",
                    "Число пар `(n-1, n)`, реально попавших в анализ траектории ряда.",
                    { refAnchorId: buildColumnAnchorId("series-computed", "k") }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "signChanges",
                    "sign changes",
                    "Число смен знака у `S_n - S` по реальной части.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "sign"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "violations",
                    "violations",
                    "Число локальных ростов ошибки `|S_n-S|`.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "viol"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "min",
                    "min |S_n-S|",
                    "Минимальное отклонение ряда от предела по всей траектории.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "devMin"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "minN",
                    "min n",
                    "Номер шага `n`, на котором ряд впервые достигает минимального отклонения.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "minN"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "last",
                    "last |S_n-S|",
                    "Отклонение ряда в последней доступной точке.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "devLast"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "lastN",
                    "last n",
                    "Последний `n`, для которого отклонение ряда остаётся конечным.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "lastN"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "lastMinusMin",
                    "last - min",
                    "Разность `|S_last-S| - min |S_n-S|`, то есть насколько ряд ушёл от своей лучшей точки к концу.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "lastMinusMin"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "lastMinAmp",
                    "last/min amp",
                    "Амплитуда `log10(last) - log10(min)`. Показывает, на сколько порядков последняя ошибка отличается от минимума ряда.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "ampOrders"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "maxMinAmp",
                    "max/min amp",
                    "Амплитуда `log10(max) - log10(min)`. Показывает разброс ошибки между худшей и лучшей точками ряда.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "maxAmpOrders"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "mean",
                    "mean |S_n-S|",
                    "Среднее значение отклонения ряда по всем анализируемым точкам.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "devMean"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "median",
                    "median |S_n-S|",
                    "Медиана значений `|S_n-S|` по траектории ряда.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "devMedian"),
                    }
                ),
                createXlsxField(
                    "series-computed",
                    "summary",
                    "max",
                    "max |S_n-S|",
                    "Максимальное отклонение ряда от предела по всей траектории.",
                    {
                        preference: "lower",
                        refAnchorId: buildColumnAnchorId("series-computed", "devMax"),
                    }
                ),
            ]
        ),
        createKeyValueSheet(
            "series-computed",
            "selected-meta",
            "selected_meta",
            "Подробная мета-информация по выбранному ряду.",
            [
                "Появляется только если на экране выбран конкретный ряд.",
                "",
                "Содержит предел, класс, описание класса, минимум, последнюю точку, амплитуды и итоговые статистики.",
            ].join("\n"),
            [
                createXlsxField("series-computed", "selected-meta", "series", "series", "Имя выбранного ряда."),
                createXlsxField("series-computed", "selected-meta", "precision", "precision", "Precision выбранного ряда."),
                createXlsxField("series-computed", "selected-meta", "args", "args", "Сводка параметров выбранного ряда."),
                createXlsxField("series-computed", "selected-meta", "limit", "limit", "Предел `S` для выбранного ряда."),
                createXlsxField("series-computed", "selected-meta", "side", "side", "Сторона поведения: `1s`, `2s` или `?`."),
                createXlsxField("series-computed", "selected-meta", "class", "class", "Короткий символ класса поведения."),
                createXlsxField("series-computed", "selected-meta", "classTitle", "class title", "Текстовое название класса."),
                createXlsxField("series-computed", "selected-meta", "classDescription", "class description", "Полное текстовое описание класса, без необходимости смотреть tooltip на экране."),
                createXlsxField("series-computed", "selected-meta", "steps", "steps", "Число шагов `(n-1, n)` в анализе."),
                createXlsxField("series-computed", "selected-meta", "signChanges", "sign changes", "Число смен знака у `S_n-S`."),
                createXlsxField("series-computed", "selected-meta", "violations", "violations", "Число локальных ростов `|S_n-S|`."),
                createXlsxField("series-computed", "selected-meta", "min", "min |S_n-S|", "Минимальное отклонение ряда."),
                createXlsxField("series-computed", "selected-meta", "minN", "min n", "Шаг, на котором ряд впервые достиг минимума."),
                createXlsxField("series-computed", "selected-meta", "last", "last |S_n-S|", "Отклонение в последней доступной точке."),
                createXlsxField("series-computed", "selected-meta", "lastN", "last n", "Последний `n` с конечным отклонением."),
                createXlsxField("series-computed", "selected-meta", "lastMinusMin", "last - min", "Насколько последняя ошибка отличается от минимума ряда."),
                createXlsxField("series-computed", "selected-meta", "lastMinAmp", "last/min amp", "Амплитуда `log10(last) - log10(min)`."),
                createXlsxField("series-computed", "selected-meta", "maxMinAmp", "max/min amp", "Амплитуда `log10(max) - log10(min)`."),
                createXlsxField("series-computed", "selected-meta", "mean", "mean |S_n-S|", "Среднее отклонение ряда."),
                createXlsxField("series-computed", "selected-meta", "median", "median |S_n-S|", "Медиана отклонения ряда."),
                createXlsxField("series-computed", "selected-meta", "max", "max |S_n-S|", "Максимальное отклонение ряда."),
            ]
        ),
        createXlsxSheet(
            "series-computed",
            "selected-points",
            "selected_points",
            "Все вычисленные точки частичных сумм для выбранного ряда.",
            "Лист содержит `n`, действительную и мнимую части `S_n`, норму ошибки `|S_n-S|` и signed-вариант ошибки.",
            [
                createXlsxField("series-computed", "selected-points", "n", "n", "Номер члена последовательности частичных сумм."),
                createXlsxField("series-computed", "selected-points", "reSn", "Re(S_n)", "Действительная часть частичной суммы `S_n`."),
                createXlsxField("series-computed", "selected-points", "imSn", "Im(S_n)", "Мнимая часть частичной суммы `S_n`."),
                createXlsxField("series-computed", "selected-points", "absDev", "|S_n-S|", "Абсолютное отклонение частичной суммы от предела."),
                createXlsxField("series-computed", "selected-points", "signedAbsDev", "sgn*|S_n-S|", "Подписанное отклонение: модуль ошибки, умноженный на знак реальной части отклонения."),
                createXlsxField("series-computed", "selected-points", "signReDev", "sgn(Re(S_n-S))", "Знак реальной части разности `S_n-S`: `-1`, `0` или `1`."),
            ]
        ),
        createXlsxSheet(
            "series-computed",
            "selected-diffs",
            "selected_diffs",
            "Разности между соседними частичными суммами.",
            "Содержит `S_n - S_{n-1}` по выбранному ряду и позволяет отдельно анализировать величину шага между соседними точками.",
            [
                createXlsxField("series-computed", "selected-diffs", "n", "n", "Номер текущего шага, для которого вычислена разность `S_n - S_{n-1}`."),
                createXlsxField("series-computed", "selected-diffs", "reDiff", "Re(S_n-S_{n-1})", "Действительная часть приращения между соседними частичными суммами."),
                createXlsxField("series-computed", "selected-diffs", "imDiff", "Im(S_n-S_{n-1})", "Мнимая часть приращения между соседними частичными суммами."),
                createXlsxField("series-computed", "selected-diffs", "absDiff", "|S_n-S_{n-1}|", "Модуль приращения между соседними частичными суммами."),
                createXlsxField("series-computed", "selected-diffs", "signedAbsDiff", "sgn*|S_n-S_{n-1}|", "Подписанный модуль приращения: знак берётся по действительной части разности."),
            ]
        ),
    ],
};

export const ALGO_SERIES_CONVERGENCE_TABLE_DOCS: DocsTableSection<AlgoSeriesConvergenceDocsColumnKey> =
    {
        key: "algo-series-convergence",
        id: buildTableAnchorId("algo-series-convergence"),
        screenSectionId: buildScreenAnchorId("algo-series-convergence"),
        xlsxSectionId: buildXlsxAnchorId("algo-series-convergence"),
        title: "Алгоритмы × ряды",
        summary:
            "Матрица по парам алгоритм–ряд: на экране основной объект это цветная ячейка с классом поведения ошибки, а xlsx раскладывает те же пары в плоский summary, отдельную matrix-страницу и опциональные detail-листы.",
        introMarkdown: String.raw`
Таблица сопоставляет **каждую пару «алгоритм × ряд»** и анализирует траекторию ошибки

$$
d_n = |A_n - \lim|.
$$

На экране главное представление — **цветная матрица**, где:

- строка соответствует алгоритму;
- столбец соответствует ряду;
- каждая ячейка описывает одну конкретную пару.

Эта таблица отличается от \`Рангов алгоритмов\`: здесь значения **не агрегируются по алгоритму**, а хранятся отдельно для каждой пары.
`,
        screenMarkdown: String.raw`
Экранная версия специально оптимизирована под быстрый визуальный просмотр:

- в угловой ячейке показано, что строки — это алгоритмы, а столбцы — ряды;
- в теле матрицы каждая ячейка окрашена по классу и показывает \`class\` + \`min@n\`;
- клик по ячейке открывает детальный блок ниже матрицы;
- пороги \`max sign changes\` и \`max violations\` меняют **только классификацию**, но не исходные значения \(A_n\).

Из-за этого xlsx не копирует экран буквально: в файле есть и плоская таблица по парам, и отдельный лист-матрица.
`,
        xlsxMarkdown: String.raw`
У этой таблицы xlsx состоит из **трёх основных листов**:

- \`overview\`;
- \`summary\`;
- \`matrix\`.

Если на экране выбрана конкретная ячейка, в экспорт дополнительно попадают:

- \`selected_meta\`;
- \`selected_points\`;
- \`selected_diffs\`.

Важно: лист \`summary\` и лист \`matrix\` описывают **одни и те же пары**, но в разном формате:

- \`summary\` — это плоская таблица, по одной строке на пару алгоритм–ряд;
- \`matrix\` — компактная цветная сетка, ближе всего к экранному виду;
- \`selected_*\` — уже не обзор, а детализация только одной выбранной пары.
`,
        columns: [
            createColumn(
                "algo-series-convergence",
                "axes",
                "Оси матрицы",
                "neutral",
                String.raw`
Матрица читается так:

- каждая строка — один алгоритм;
- каждый столбец — один ряд;
- пересечение строки и столбца — одна пара \((A, S)\).

Верхний левый угол \`Algorithm \ Series\` нужен не как метрика, а как подсказка по ориентации матрицы.

**Интерпретация:** экран показывает не набор фиксированных колонок, а координатную сетку по парам.
`
            ),
            createColumn(
                "algo-series-convergence",
                "cell",
                "Ячейка матрицы",
                "neutral",
                String.raw`
Обозначим

$$
d_n = |A_n - \lim|.
$$

В ячейке отображаются:

- символ класса (\`→\`, \`↓→\`, \`↓↓\`, \`↑↓\`, \`↓↑\`, \`↑↑\`);
- номер \`n\`, на котором впервые достигнут минимум \`min@n\`.

Для пары считаются, в частности, величины

$$
d_{\min} = \min_n d_n,
\qquad
d_{\text{last}} = d_{n_{\text{last}}}.
$$

Амплитуды в интерфейсе и xlsx трактуются так:

$$
\text{last/min amp} = \log_{10}(d_{\text{last}}) - \log_{10}(d_{\min}),
$$

$$
\text{max/min amp} = \log_{10}\!\left(\max_n d_n\right) - \log_{10}(d_{\min}),
$$

$$
\text{series/algo amp} =
\log_{10}\!\left(\min_n |S_n-\lim|\right) - \log_{10}(d_{\min}).
$$

Для амплитуд принят конвенциональный safe-log: \`log10(0) = 0\`, чтобы точное попадание в предел не давало бесконечности.

**Как читать \`series/algo amp\`:**

- \`> 0\` — алгоритм дал меньшую минимальную ошибку, чем сам ряд;
- \`= 1\` — примерно на один порядок, то есть примерно в 10 раз лучше;
- \`< 0\` — минимум алгоритма хуже минимума ряда.
`
            ),
            createColumn(
                "algo-series-convergence",
                "thresholds",
                "Пороги классификации",
                "neutral",
                String.raw`
Слайдеры в шапке влияют на то, насколько строго таблица считает траекторию односторонней и почти монотонной.

\`max sign changes\` ограничивает число смен знака у \(A_n - \lim\):

- меньшее значение делает классификацию \`1s\` строже;
- большее значение чаще допускает односторонность.

\`max violations\` ограничивает число локальных ростов ошибки:

$$
d_{n+1} > d_n.
$$

- меньшее значение требует почти идеального убывания;
- большее значение терпимее к отдельным всплескам.

Эти пороги записываются в лист \`overview\` и используются при формировании \`summary\`, \`matrix\` и \`selected_meta\`.
`
            ),
            createColumn(
                "algo-series-convergence",
                "class",
                "Класс",
                "neutral",
                String.raw`
Класс объединяет **сторону** (\`1s\` / \`2s\`) и **форму траектории** ошибки.

- \`→\` — ошибка практически не меняется;
- \`↓→\` — минимум достигнут раньше конца и дальше удерживается;
- \`↓↓\` — минимум пришёлся на последнюю точку без ухудшений;
- \`↑↓\` — к концу пришли к минимуму, но по дороге были и рост, и спад;
- \`↓↑\` — минимум был внутри последовательности, потом стало хуже;
- \`↑↑\` — лучшая точка была в начале, дальше только хуже.

Светлый оттенок обычно означает \`1s\`, тёмный — \`2s\`.  
В xlsx \`side\` и сам \`class\` хранятся раздельно. Для статического случая \`→\` сторона экспортируется как \`1s\`.

Порядок сортировки классов от лучших к худшим:

\`violet\`, \`green\`, \`dark-green\`, \`yellow\`, \`dark-yellow\`, \`orange\`, \`dark-orange\`, \`red\`, \`dark-red\`.
`
            ),
            createColumn(
                "algo-series-convergence",
                "selectedDetail",
                "Детализация выбранной пары",
                "neutral",
                String.raw`
После клика по ячейке открывается блок детализации для одной пары алгоритм–ряд.

Он показывает:

- limit;
- класс и его текстовое описание;
- \`min\`, \`min n\`, \`last\`, \`last n\`, \`last-min\`;
- \`last/min amp\`, \`max/min amp\`, \`series/algo amp\`;
- график ошибки и таблицы по точкам \(A_n\).

Эта же выбранная пара определяет опциональные xlsx-листы \`selected_meta\`, \`selected_points\` и \`selected_diffs\`.  
Если ячейка не выбрана, этих листов в экспорте не будет.
`
            ),
        ],
        xlsxSheets: [
            createKeyValueSheet(
                "algo-series-convergence",
                "overview",
                "overview",
                "Параметры текущего экспорта и активные пороги классификации.",
                [
                    "Содержит число алгоритмов, число рядов, число пар после фильтрации, значения `max sign changes`, `max violations` и идентификатор выбранной ячейки.",
                    "",
                    "Это служебный лист, который фиксирует контекст, в котором были построены остальные страницы.",
                ].join("\n"),
                [
                    createXlsxField("algo-series-convergence", "overview", "algorithms", "algorithms", "Число алгоритмов, попавших в текущий экспорт."),
                    createXlsxField("algo-series-convergence", "overview", "series", "series", "Число рядов, попавших в текущий экспорт."),
                    createXlsxField("algo-series-convergence", "overview", "pairs", "pairs", "Число пар алгоритм–ряд, реально вошедших в анализ и лист `summary`."),
                    createXlsxField("algo-series-convergence", "overview", "maxSignChanges", "max sign changes", "Текущее значение порога, разрешающего ограниченное число смен знака для `1s`."),
                    createXlsxField("algo-series-convergence", "overview", "maxViolations", "max violations", "Текущее значение порога, разрешающего ограниченное число ростов ошибки."),
                    createXlsxField("algo-series-convergence", "overview", "selectedCell", "selected cell", "Идентификатор выбранной ячейки вида `algoId × seriesId`. `none` означает, что detail-листы `selected_*` в файл не включались."),
                ]
            ),
            createXlsxSheet(
                "algo-series-convergence",
                "summary",
                "summary",
                "Плоская таблица по всем попавшим в экспорт парам алгоритм–ряд.",
                [
                    "Каждая строка соответствует одной паре. Здесь удобнее всего делать фильтрацию, сортировку и внешнюю постобработку в Excel.",
                    "",
                    "Лист содержит `side`, `class`, `class title`, счётчики `sign changes` и `violations`, а также `min`, `last`, `last-min`, `last/min amp`, `max/min amp`, `series/algo amp`.",
                ].join("\n"),
                [
                    createXlsxField("algo-series-convergence", "summary", "algorithm", "algorithm", "Имя алгоритма в данной паре."),
                    createXlsxField("algo-series-convergence", "summary", "m", "m", "Параметр `m` алгоритма, если он задан."),
                    createXlsxField("algo-series-convergence", "summary", "args", "args", "Сводка параметров алгоритма в виде строки `key=value`."),
                    createXlsxField("algo-series-convergence", "summary", "series", "series", "Имя ряда в данной паре."),
                    createXlsxField("algo-series-convergence", "summary", "x", "x", "Значение параметра `x` для ряда, если он есть в идентификаторе ряда."),
                    createXlsxField("algo-series-convergence", "summary", "precision", "precision", "Precision ряда в данной паре."),
                    createXlsxField("algo-series-convergence", "summary", "limit", "limit", "Предел, относительно которого измеряется ошибка алгоритма."),
                    createXlsxField("algo-series-convergence", "summary", "side", "side", "Сторона поведения: `1s`, `2s` или `?`. Для статического класса `→` экспортируется `1s`."),
                    createXlsxField("algo-series-convergence", "summary", "class", "class", "Короткий символ класса поведения: `→`, `↓→`, `↓↓`, `↑↓`, `↓↑` или `↑↑`."),
                    createXlsxField("algo-series-convergence", "summary", "classTitle", "class title", "Текстовое название класса, удобное вне цветовой легенды."),
                    createXlsxField("algo-series-convergence", "summary", "steps", "steps", "Число пар `(n-1, n)`, реально попавших в анализ данной пары алгоритм–ряд."),
                    createXlsxField("algo-series-convergence", "summary", "signChanges", "sign changes", "Число смен знака у `A_n - lim`.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "violations", "violations", "Число локальных ростов ошибки `|A_n-lim|`.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "min", "min |A_n-lim|", "Минимальное отклонение алгоритма от предела по всей траектории пары.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "minN", "min n", "Шаг `n`, на котором алгоритм впервые достиг своего минимума.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "last", "last |A_n-lim|", "Отклонение алгоритма в последней доступной точке.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "lastN", "last n", "Последний `n` с конечным значением `|A_n-lim|`.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "lastMinusMin", "last - min", "Разность `|A_last-lim| - min |A_n-lim|`, то есть уход алгоритма от своего минимума к концу.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "lastMinAmp", "last/min amp", "Амплитуда `log10(last) - log10(min)`. Показывает, на сколько порядков последняя ошибка отличается от минимума.", { preference: "lower" }),
                    createXlsxField("algo-series-convergence", "summary", "maxMinAmp", "max/min amp", "Амплитуда `log10(max) - log10(min)`. Показывает разброс между худшей и лучшей точкой алгоритма.", { preference: "lower" }),
                    createXlsxField(
                        "algo-series-convergence",
                        "summary",
                        "seriesAlgoAmp",
                        "series/algo amp",
                        [
                            "Сравнение минимальной ошибки ряда и минимальной ошибки алгоритма.",
                            "",
                            "Считается как `log10(min series) - log10(min algo)`: `> 0` означает, что алгоритм точнее ряда, `< 0` — хуже ряда.",
                        ].join("\n"),
                        { preference: "higher" }
                    ),
                ]
            ),
            createXlsxSheet(
                "algo-series-convergence",
                "matrix",
                "matrix",
                "Компактная цветная матрица, ближе всего к экранному виду.",
                [
                    "Строки — алгоритмы, столбцы — ряды. Внутри ячейки записаны класс и `min@n`.",
                    "",
                    "Этот лист нужен для визуального обзора. Для табличного анализа удобнее `summary`.",
                ].join("\n"),
                [
                    createXlsxField("algo-series-convergence", "matrix", "corner", "algorithm \\ series", "Левая верхняя ячейка матрицы. Показывает ориентацию листа: строки — алгоритмы, столбцы — ряды."),
                    createXlsxField("algo-series-convergence", "matrix", "algoRowHeader", "algorithm row header", "Подпись строки алгоритма. Содержит имя алгоритма, при наличии `m`, и строку `args`."),
                    createXlsxField("algo-series-convergence", "matrix", "seriesColumnHeader", "series column header", "Подпись столбца ряда. Содержит имя ряда, `x` и `precision`."),
                    createXlsxField("algo-series-convergence", "matrix", "cellPayload", "cell payload", "Содержимое внутренней ячейки матрицы: короткий класс и `min@n` для соответствующей пары."),
                    createXlsxField("algo-series-convergence", "matrix", "cellColor", "cell color", "Заливка ячейки кодирует цветовой класс поведения. Она соответствует той же легенде классов, что и на экране."),
                ]
            ),
            createKeyValueSheet(
                "algo-series-convergence",
                "selected-meta",
                "selected_meta",
                "Метаданные и сводные метрики выбранной пары.",
                [
                    "Появляется только при выбранной ячейке на экране.",
                    "",
                    "Содержит `side`, `class`, описание класса, счётчики смен знака и нарушений, а также `min`, `last`, `last-min` и все три амплитуды.",
                ].join("\n"),
                [
                    createXlsxField("algo-series-convergence", "selected-meta", "algorithm", "algorithm", "Имя алгоритма в выбранной паре."),
                    createXlsxField("algo-series-convergence", "selected-meta", "m", "m", "Параметр `m` выбранного алгоритма."),
                    createXlsxField("algo-series-convergence", "selected-meta", "args", "args", "Сводка параметров алгоритма."),
                    createXlsxField("algo-series-convergence", "selected-meta", "series", "series", "Имя выбранного ряда."),
                    createXlsxField("algo-series-convergence", "selected-meta", "x", "x", "Значение `x` выбранного ряда."),
                    createXlsxField("algo-series-convergence", "selected-meta", "precision", "precision", "Precision выбранного ряда."),
                    createXlsxField("algo-series-convergence", "selected-meta", "limit", "limit", "Предел для выбранной пары."),
                    createXlsxField("algo-series-convergence", "selected-meta", "side", "side", "Сторона поведения: `1s`, `2s` или `?`."),
                    createXlsxField("algo-series-convergence", "selected-meta", "class", "class", "Короткий символ класса поведения."),
                    createXlsxField("algo-series-convergence", "selected-meta", "classTitle", "class title", "Текстовое название класса."),
                    createXlsxField("algo-series-convergence", "selected-meta", "classDescription", "class description", "Полное текстовое описание класса выбранной пары."),
                    createXlsxField("algo-series-convergence", "selected-meta", "steps", "steps", "Число шагов `(n-1, n)` в анализе выбранной пары."),
                    createXlsxField("algo-series-convergence", "selected-meta", "signChanges", "sign changes", "Число смен знака у `A_n-lim`."),
                    createXlsxField("algo-series-convergence", "selected-meta", "violations", "violations", "Число ростов `|A_n-lim|`."),
                    createXlsxField("algo-series-convergence", "selected-meta", "min", "min |A_n-lim|", "Минимальное отклонение алгоритма."),
                    createXlsxField("algo-series-convergence", "selected-meta", "minN", "min n", "Шаг первого достижения минимума."),
                    createXlsxField("algo-series-convergence", "selected-meta", "last", "last |A_n-lim|", "Отклонение в последней точке."),
                    createXlsxField("algo-series-convergence", "selected-meta", "lastN", "last n", "Последний `n` с конечной ошибкой."),
                    createXlsxField("algo-series-convergence", "selected-meta", "lastMinusMin", "last - min", "Уход последней точки от минимума."),
                    createXlsxField("algo-series-convergence", "selected-meta", "lastMinAmp", "last/min amp", "Амплитуда `log10(last) - log10(min)`."),
                    createXlsxField("algo-series-convergence", "selected-meta", "maxMinAmp", "max/min amp", "Амплитуда `log10(max) - log10(min)`."),
                    createXlsxField("algo-series-convergence", "selected-meta", "seriesAlgoAmp", "series/algo amp", "Сравнение минимумов ряда и алгоритма: `> 0` означает, что алгоритм точнее ряда."),
                ]
            ),
            createXlsxSheet(
                "algo-series-convergence",
                "selected-points",
                "selected_points",
                "Все вычисленные точки `A_n` для выбранной пары.",
                [
                    "Лист содержит `n`, действительную и мнимую части `A_n`, `|A_n-lim|`, signed-ошибку и знак `Re(A_n-lim)`.",
                    "",
                    "Это основной источник для ручной проверки траектории конкретного алгоритма на конкретном ряду.",
                ].join("\n"),
                [
                    createXlsxField("algo-series-convergence", "selected-points", "n", "n", "Номер шага алгоритма."),
                    createXlsxField("algo-series-convergence", "selected-points", "reAn", "Re(A_n)", "Действительная часть значения `A_n`."),
                    createXlsxField("algo-series-convergence", "selected-points", "imAn", "Im(A_n)", "Мнимая часть значения `A_n`."),
                    createXlsxField("algo-series-convergence", "selected-points", "absDev", "|A_n-lim|", "Абсолютная ошибка алгоритма относительно предела."),
                    createXlsxField("algo-series-convergence", "selected-points", "signedAbsDev", "sgn*|A_n-lim|", "Подписанная ошибка: модуль `|A_n-lim|`, умноженный на знак реальной части отклонения."),
                    createXlsxField("algo-series-convergence", "selected-points", "signReDev", "sgn(Re(A_n-lim))", "Знак реальной части разности `A_n-lim`: `-1`, `0` или `1`."),
                ]
            ),
            createXlsxSheet(
                "algo-series-convergence",
                "selected-diffs",
                "selected_diffs",
                "Разности между соседними значениями `A_n`.",
                [
                    "Содержит шаги вида `A_n - A_{n-1}` для выбранной пары.",
                    "",
                    "Лист полезен, когда нужно отдельно смотреть поведение приращений, а не только ошибку относительно предела.",
                ].join("\n"),
                [
                    createXlsxField("algo-series-convergence", "selected-diffs", "n", "n", "Номер текущего шага, для которого записана разность `A_n-A_{n-1}`."),
                    createXlsxField("algo-series-convergence", "selected-diffs", "reDiff", "Re(A_n-A_{n-1})", "Действительная часть приращения алгоритма между соседними шагами."),
                    createXlsxField("algo-series-convergence", "selected-diffs", "imDiff", "Im(A_n-A_{n-1})", "Мнимая часть приращения алгоритма между соседними шагами."),
                    createXlsxField("algo-series-convergence", "selected-diffs", "absDiff", "|A_n-A_{n-1}|", "Модуль приращения алгоритма между соседними шагами."),
                ]
            ),
        ],
    };

export const TABLE_DOCS: readonly [
    DocsTableSection<AlgoRankingDocsColumnKey>,
    DocsTableSection<SeriesComputedDocsColumnKey>,
    DocsTableSection<AlgoSeriesConvergenceDocsColumnKey>,
] = [ALGO_RANKING_TABLE_DOCS, SERIES_COMPUTED_TABLE_DOCS, ALGO_SERIES_CONVERGENCE_TABLE_DOCS];

const TABLE_DOCS_BY_KEY = Object.fromEntries(TABLE_DOCS.map((table) => [table.key, table])) as {
    "algo-ranking": DocsTableSection<AlgoRankingDocsColumnKey>;
    "series-computed": DocsTableSection<SeriesComputedDocsColumnKey>;
    "algo-series-convergence": DocsTableSection<AlgoSeriesConvergenceDocsColumnKey>;
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

export function getAlgoSeriesConvergenceColumnAnchorId(
    key: AlgoSeriesConvergenceDocsColumnKey
): string {
    return getTableDocs("algo-series-convergence").columns.find((column) => column.key === key)!.id;
}

export function listAllDocsAnchorIds(): string[] {
    const ids = [DOCS_HOME_ANCHOR_ID];

    for (const table of TABLE_DOCS) {
        ids.push(table.id, table.screenSectionId, table.xlsxSectionId);
        for (const column of table.columns) ids.push(column.id);
        for (const sheet of table.xlsxSheets) {
            ids.push(sheet.id);
            for (const field of sheet.fields) ids.push(field.id);
        }
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
