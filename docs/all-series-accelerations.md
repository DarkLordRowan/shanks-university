# Ряды и преобразования

Этот документ содержит исчерпывающий список всех доступных рядов и преобразований последовательностей в проекте Shanks University, включая их математические описания, параметры и ограничения.

---

## Ряды (Series)

| Имя (CamelCase) | Описание | Ограничения основного параметра `x` | Дополнительные параметры |
| :--- | :--- | :--- | :--- |
| **AbsSinXMinus2DivPiSeries** | Ряд Фурье для $\| \sin(x) \| - \frac{2}{\pi}$ | $x \in [0, 2\pi]$ | Нет |
| **ArcsinX2Series** | Разложение Маклорена для $\arcsin(x^2)$ | $\|x\| \le 1$ | Нет |
| **ArcsinXMinusXSeries** | Разложение Маклорена для $\arcsin(x) - x$ | $\|x\| \le 1$ | Нет |
| **ArcsinXSeries** | Разложение Маклорена для $\arcsin(x)$ | $\|x\| \le 1$ | Нет |
| **ArcsinhXSeries** | Разложение Маклорена для $\text{arcsinh}(x)$ | $\|x\| \in [-1, 1]$ | Нет |
| **ArtanhX2Series** | Разложение Маклорена для $\text{arctanh}(x^2)$ | $\|x\| < 1$ | Нет |
| **ArtanhXSeries** | Разложение Маклорена для $\text{arctanh}(x)$ | $\|x\| < 1$ | Нет |
| **ArctanX2Series** | Разложение Маклорена для $\arctan(x^2)$ | $\|x\| \le 1$ | Нет |
| **ArctanX3Series** | Разложение Маклорена для $\arctan(x^3)$ | $\|x\| \le 1$ | Нет |
| **ArctanXSeries** | Разложение Маклорена для $\arctan(x)$ | $\|x\| \le 1$ | Нет |
| **BinSeries** | Обобщенный биномиальный ряд $(1+x)^\alpha$ | $\|x\| < 1$ | `alpha` (T): степень |
| **CiXSeries** | Разложение интегрального косинуса $\text{Ci}(x)$ | Конечное | Нет |
| **Cos3xMinus1DivXsquareSeries** | Разложение $\frac{\cos(3x-1)}{x^2}$ | $x \ne 0$, Конечное | Нет |
| **CosSeries** | Разложение Маклорена для $\cos(x)$ | Конечное | Нет |
| **CosSqrtXSeries** | Разложение $\cos(\sqrt{x})$ | Вещественное $x \ge 0$, Конечное | Нет |
| **CosX2Series** | Разложение Маклорена для $\cos(x^2)$ | Конечное | Нет |
| **CoshSeries** | Разложение Маклорена для $\cosh(x)$ | Конечное | Нет |
| **EXSeries** | Ряд для функции, связанной с полным эллиптическим интегралом $E_x(x)$ | $\|x\| < 1$ | Нет |
| **EighthPiMOneThirdSeries** | Разложение ряда, сходящегося к $x \cdot (\frac{\pi}{8} - \frac{1}{3})$ | Конечное | Нет |
| **ErfSeries** | Разложение Маклорена для $\frac{\sqrt{\pi}}{2} \text{erf}(x)$ | Конечное | Нет |
| **ExpMCosXSinsinXSeries** | Ряд для $\exp(-\cos(x)) \sin(\sin(x))$ | Конечное | Нет |
| **ExpSeries** | Разложение Маклорена для $\exp(x)$ | Конечное | Нет |
| **ExpSquaredErfSeries** | Степенной ряд для $\exp(x^2) \text{erf}(x)$ | Конечное | Нет |
| **FivePiTwelveSeries** | Разложение ряда, сходящегося к $x \cdot \frac{5\pi}{12}$ | Конечное | Нет |
| **FourArctanSeries** | Разложение Маклорена для $4 \arctan(x)$ | $\|x\| \le 1$ | Нет |
| **FourLn2M3Series** | Разложение ряда, сходящегося к $x \cdot (4 \ln(2) - 3)$ | Конечное | Нет |
| **HalfAsinTwoXSeries** | Разложение Маклорена для $0.5 \arcsin(2x)$ | $\|x\| \le 0.5$ | Нет |
| **HalfMinusSinxMultiPi4Series** | Ряд для $0.5 - \frac{\pi}{4} \sin(x)$ | $x \in [0, \pi/2]$ (вещественное) | Нет |
| **HalfMultiLn1Div2Multi1MinusCosxSeries**| Ряд Фурье для $-0.5 \ln(2 - 2 \cos(x))$ | $\|x\| < \pi$ | Нет |
| **IncompleteGammaFuncSeries** | Степенной ряд для нижней неполной гамма-функции $\gamma(\alpha, x)$ | Конечное | `alpha` (T): форма |
| **Inverse1mxSeries** | Геометрический ряд для $\frac{1}{1-x}$ | $\|x\| < 1$ | Нет |
| **InvSqrt1m4xSeries** | Разложение Маклорена для $\frac{1}{\sqrt{1-4x}}$ | $\|x\| < 0.25$ | Нет |
| **KXSeries** | Степенной ряд для полного эллиптического интеграла $K(x)$ | $\|x\| < 1$ | Нет |
| **LambertWFuncSeries** | Разложение главной ветви $W_0(x)$ | $\|x\| < 1/e$ | Нет |
| **Ln1MinusX2Series** | Разложение Маклорена для $\ln(1-x^2)$ | $\|x\| < 1$ | Нет |
| **Ln1mxSeries** | Разложение Маклорена для $-\ln(1-x)$ | $\|x\| < 1$ | Нет |
| **Ln1px4Series** | Разложение Маклорена для $\ln(1+x^4)$ | $\|x\| < 1$ | Нет |
| **Ln2Series** | Разложение ряда, сходящегося к $x \ln(2)$ | Конечное | Нет |
| **Ln13MinusLn7Div7Series** | Разложение ряда, сходящегося к $x \frac{\ln(13/7)}{7}$ | Конечное | Нет |
| **Ln1PlusX3Series** | Разложение Маклорена для $\ln(1+x^3)$ | $\|x\| < 1$ | Нет |
| **Ln1PlusSqrt1PlusXsquareMinusLn2Series** | Разложение для $\ln(1+\sqrt{1+x^2}) - \ln(2)$ | $\|x\| < 1$ | Нет |
| **Ln1plusXDiv1plusX2Series** | Разложение для $\frac{\ln(1+x)}{1+x^2}$ | $\|x\| < 1$ | Нет |
| **LnCosxSeries** | Разложение для $\ln(\cos(x))$ | $\|x\| < \pi/2$ | Нет |
| **LnSinxMinusLnXSeries** | Разложение для $\ln(\frac{\sin(x)}{x})$ | $x \in (0, \pi)$ (вещественное) | Нет |
| **LnXPlusOneXMinusOneHalfedSeries** | В настоящее время реализовано как $\ln(\cos(x))$ | $\|x\| < \pi/2$ | Нет |
| **MFact1mxMp1InverseSeries** | Разложение для $\frac{m!}{(1-x)^{m+1}}$ | $\|x\| < 1$ | `m` (K): порядок |
| **MeanSinhSinSeries** | Разложение Маклорена для $0.5 (\sinh(x) + \sin(x))$ | Конечное | Нет |
| **Minus3Div4OrXMinus3Div4Series** | Ряд Фурье для пилообразной линейной функции | $\|x\| < 3$ | Нет |
| **MinusOneNFactNInNSeries** | Комбинаторный ряд, сходящийся к $\approx -0.6558x$ | Конечное | Нет |
| **MinusOneNedInNSeries** | Ряд, связанный с "Мечтой второкурсника" ($\approx -0.7834x$) | Конечное | Нет |
| **MinusOneQuarterSeries** | Разложение ряда, сходящегося к $-0.25x$ | Конечное | Нет |
| **MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2Series** | Ряд, сходящийся к $x(\ln(4)+1.5\ln(3)-3)$ | Конечное | Нет |
| **MinusXMinusPi4OrMinusPi4Series** | Ряд Фурье для полувыпрямленной линейной функции | $\|x\| < \pi$ | Нет |
| **OneDivSqrt2SinXDivSqrt2Series** | Разложение Маклорена для $\frac{1}{\sqrt{2}} \sin(\frac{x}{\sqrt{2}})$ | Конечное | Нет |
| **OneDivTwoMinusXMultiThreePlusXSeries** | Степенной ряд для $\frac{1}{(2-x)(3+x)}$ | $\|x\| < 2$ | Нет |
| **OneMinusSqrt1minus4xDiv2xSeries** | Производящая функция Каталана $\frac{1-\sqrt{1-4x}}{2x}$ | $\|x\| \le 0.25, x \ne 0$ | Нет |
| **OneSeries** | Разложение ряда, сходящегося к $x$ | Конечное | Нет |
| **OneThirdPiSquaredMNineSeries** | Ряд, сходящийся к $x \frac{\pi^2 - 9}{3}$ | Конечное | Нет |
| **OneTwelfth3x2Pi2Series** | Ряд Фурье для $\frac{3x^2 - \pi^2}{12}$ | $\|x\| \le \pi$ | Нет |
| **OneTwelfthSeries** | Разложение ряда, сходящегося к $x / 12$ | Конечное | Нет |
| **OneTwelfthX2Pi2Series** | Ряд Фурье для $\frac{x(x^2 - \pi^2)}{12}$ | Конечное | Нет |
| **Pi3Series** | Разложение ряда, сходящегося к $x \pi / 3$ | Конечное | Нет |
| **Pi4Series** | Разложение ряда, сходящегося к $x \pi / 4$ | Конечное | Нет |
| **Pi8CosxSquareMinus1Div3CosxSeries** | Тригонометрический ряд для $\frac{\pi}{8} \cos^2(x) - \frac{\cos(x)}{3}$ | $\|x\| \le \pi/2$ | Нет |
| **PiCubed32Series** | Разложение ряда, сходящегося к $x \pi^3 / 32$ | Конечное | Нет |
| **PiFourMinusLn2HalfedSeries** | Разложение ряда, сходящегося к $x (\frac{\pi}{4} - \frac{\ln(2)}{2})$ | Конечное | Нет |
| **PiMinus3pi4AndPiMinusXMinus3pi4Series**| Ряд Фурье для кусочно-линейной функции | $\|x\| < \pi$ | Нет |
| **PiMinusX2Series** | Ряд Фурье для $\frac{\pi - x}{2}$ | $x \in (0, \pi)$ (вещественное) | Нет |
| **PiSeries** | Разложение ряда, сходящегося к $x \pi$ | Конечное | Нет |
| **PiSixMinHalfSeries** | Разложение ряда, сходящегося к $x (\frac{\pi}{6} - \frac{1}{2})$ | Конечное | Нет |
| **PiSquared6MinusOneSeries** | Разложение ряда, сходящегося к $x (\frac{\pi^2}{6} - 1)$ | Конечное | Нет |
| **PiSquaredTwelveSeries** | Разложение ряда, сходящегося к $x \frac{\pi^2}{12}$ | Конечное | Нет |
| **PiXMinusXSquareAndXSquareMinusThreePiXPlusTwoPiSquareSeries** | Ряд Фурье по синусам для периодических параболических сегментов | $x \in (0, 2\pi)$ (вещественное) | Нет |
| **PiXMultiE_XpiPlusEMinusXpiDividedE_XpiMinusEMinusXpiMinusOneSeries** | Частичная дробь для $\pi x \coth(\pi x) - 1$ | Конечное | Нет |
| **RiemannZetaFuncSeries** | Ряд Дирихле для $\zeta(s)$ | $\text{Re}(x) > 1$ | Нет |
| **RiemannZetaFuncXmin1DivRiemannZetaFuncXSeries** | Ряд Дирихле для $\zeta(s-1)/\zeta(s)$ | $\text{Re}(x) > 2$ | Нет |
| **SeriesWithLnNumber1** | Комплексное разложение, сходящееся к $\approx 0.5992x$ | Конечное | Нет |
| **SeriesWithLnNumber2** | Комплексное разложение, сходящееся к $\approx 5.7162x$ | Конечное | Нет |
| **SiXSeries** | Разложение Маклорена интегрального синуса $\text{Si}(x)$ | Конечное | Нет |
| **SinSeries** | Разложение Маклорена для $\sin(x)$ | Конечное | Нет |
| **SinX2Series** | Разложение Маклорена для $\sin(x^2)$ | Конечное | Нет |
| **SinhSeries** | Разложение Маклорена для $\sinh(x)$ | Конечное | Нет |
| **SinhX2Series** | Разложение Маклорена для $\sinh(x^2)$ | Конечное | Нет |
| **Sqrt1plusXMinus1MinusXDiv2Series** | Разложение Маклорена для $\sqrt{1+x} - 1 - x/2$ | $\|x\| \le 1$ | Нет |
| **Sqrt1plusXSeries** | Разложение Маклорена для $\sqrt{1+x}$ | $\|x\| \le 1$ | Нет |
| **SqrtOneminusSqrtoneminusxDivXSeries** | Разложение для $\sqrt{\frac{1-\sqrt{1-x}}{x}}$ | $\|x\| < 1, x \ne 0$ | Нет |
| **TenMinusXSeries** | Разложение Фурье для $10 - x$ | $\|x - 10\| \le 5$ | Нет |
| **ThreeMinusPiSeries** | Разложение ряда, сходящегося к $x(3 - \pi)$ | Конечное | Нет |
| **TwoArcsinSquareXHalfedSeries** | Разложение Маклорена для $2 \arcsin(x/2)^2$ | $\|x\| \le 2$ | Нет |
| **TwoDegreeXSeries** | Разложение Маклорена для $2^x$ | Конечное | Нет |
| **TwoLn2Series** | Разложение ряда, сходящегося к $x \cdot 2\ln(2)$ | Конечное | Нет |
| **X_1mx_SquaredSeries** | Разложение для $\frac{x}{(1-x)^2}$ | $\|x\| < 1$ | Нет |
| **XDiv1minX2Series** | Разложение для $\frac{x}{\sqrt{1-x^2}}$ | $\|x\| < 1$ | Нет |
| **XDiv1minXSeries** | Разложение для $\frac{x}{\sqrt{1-x}}$ | $\|x\| < 1$ | Нет |
| **XMinSqrtXSeries** | Ряд для $x - \sqrt{x}$ в окрестности $x=1$ | $\|x-1\| < 1$ | Нет |
| **XSeries** | Разложение Фурье тождественной функции $f(x)=x$ | $\|x\| < \pi$ | Нет |
| **XTwoSeries** | Разложение ряда, сходящегося к $x/2$ | Конечное | Нет |
| **XTwoThroughtSquaresSeries** | Альтернативное разложение, сходящееся к $x/2$ | Конечное | Нет |
| **Xsquareplus3DivXsquareplus2multixMinus1Series** | Разложение для $\frac{x^2+3}{x^2+2x}-1$ в окрестности $x=1$ | $\|x-1\| < 1$ | Нет |

---

## Методы ускорения рядов (Transformations)

### Anderson Acceleration
*   **CamelName**: `AndersonAccelerationAlgorithm`
*   **Описание**: Ускоряет итерации с фиксированной точкой, используя линейные комбинации предыдущих шагов.
*   **Дополнительные параметры**:
    *   `m` (K, по умолчанию 50): Глубина памяти (размер истории).
    *   `beta` (float_type, по умолчанию 1.0): Параметр смешивания (коэффициент демпфирования) $\in (0, 1]$.
    *   `safeguard` (float_type, по умолчанию 1e-12): Малое значение для численной устойчивости.

### Brezinski Theta
*   **CamelName**: `BrezinskiThetaAlgorithm`
*   **Описание**: Нелинейное преобразование последовательности, эффективное для ошибок, ведущих себя как экспоненты.
*   **Дополнительные параметры**: Нет.
*   **Ограничения**: Порядок должен быть четным.

### Chang-Wynn
*   **CamelName**: `ChangWynnAlgorithm`
*   **Описание**: Гибридный алгоритм, объединяющий эпсилон-алгоритм Винна с модификациями Чанга для устойчивости.
*   **Дополнительные параметры**: Нет.

### Drummond D
*   **CamelName**: `DrummondDAlgorithm`
*   **Описание**: Эффективен для знакочередующихся рядов и специфического поведения остатка.
*   **Варианты**: `u`, `t`, `v`, `t_wave`, `v_wave`.
*   **Дополнительные параметры**:
    *   `remainder_type` (shanks::remainders::remainder_type): Стратегия оценки остатка.
    *   `use_recurrent_formula` (bool, по умолчанию false): Использовать рекурсивную схему для устойчивости.

### Ford-Sidi
*   **CamelName**: `FordSidi2Algorithm`, `FordSidi3Algorithm`
*   **Описание**: Эффективные методы экстраполяции, требующие меньше операций, чем E-алгоритм.
*   **Дополнительные параметры**: Нет.

### J-Transformation
*   **CamelName**: `JTransformationAlgorithm`
*   **Описание**: Нелинейное преобразование для медленно сходящихся или расходящихся рядов (особенно знакочередующихся/с факториальным ростом).
*   **Дополнительные параметры**:
    *   `max_order` (K, по умолчанию 30): Максимально допустимый порядок.
    *   `safeguard` (float_type, по умолчанию 1e-12): Порог устойчивости.

### Levin / Levin-Sidi
*   **Алгоритмы**: `LevinAlgorithm`, `LevinSidiMAlgorithm`, `LevinSidiSAlgorithm`
*   **Описание**: Мощные преобразования для последовательностей со специфическими асимптотическими шаблонами остатков.
*   **Варианты**: `u`, `t`, `v`, `t_wave`, `v_wave`.
*   **Дополнительные параметры**:
    *   `remainder_type` (shanks::remainders::remainder_type).
    *   `use_recurrent_formula` (bool, по умолчанию false) - для вариантов Levin и S.
    *   `beta` (float_type, по умолчанию 1.0) - для вариантов Levin и S: Должно быть $> 0$.
    *   `gamma` (float_type, по умолчанию 100.5) - для варианта M: Должно быть $> 0$, обычно $\ge$ порядок $- 1$.

### Lubkin W
*   **CamelName**: `LubkinWAlgorithm`
*   **Описание**: Эффективен как для линейной, так и для логарифмической сходимости.
*   **Дополнительные параметры**: Нет.

### Richardson
*   **CamelName**: `RichardsonAlgorithm`
*   **Описание**: Полиномиальная экстраполяция для устранения членов ошибки низшего порядка.
*   **Дополнительные параметры**: Нет.

### Shanks
*   **CamelName**: `ShanksAlgorithm`, `ShanksTransformAlternating`
*   **Описание**: Ускоряет последовательности, где члены ведут себя как суммы экспонент. `ShanksAlgorithm` использует представление через определители; `ShanksTransformAlternating` оптимизирован для знакочередующихся рядов.
*   **Дополнительные параметры**: Нет.

### Weniger
*   **CamelName**: `WenigerAlgorithm`
*   **Описание**: Преобразование типа Левина, использующее аппроксимации факториальными рядами; очень эффективно для знакочередующихся/расходящихся рядов.
*   **Дополнительные параметры**: Нет.

### Wynn Epsilon
*   **CamelName**: `WynnEpsilon1Algorithm`, `WynnEpsilon2Algorithm`, `WynnEpsilon3Algorithm`
*   **Описание**: Классический эпсилон-алгоритм для ускорения рядов.
    *   `WynnEpsilon1Algorithm`: Стандартная реализация.
    *   `WynnEpsilon2Algorithm`: Реализация с циклическим буфером и проверками устойчивости.
    *   `WynnEpsilon3Algorithm`: Рекурсивный с контролем ошибок на основе порогового значения.
*   **Дополнительные параметры**:
    *   `epsilon_threshold` (float_type, по умолчанию 1e-3) - только для варианта 3.

### Wynn Rho
*   **CamelName**: `WynnRhoAlgorithm`
*   **Описание**: Особенно эффективен для логарифмически сходящихся последовательностей, использующих цепные дроби.
*   **Варианты**: `rho`, `generalized`, `gamma_rho`.
*   **Дополнительные параметры**:
    *   `numerator_type` (shanks::numerators::numerator_type).
    *   `gamma` (float_type, по умолчанию -1.0).
    *   `rho` (float_type, по умолчанию 1.0).
```
