#pragma once

template<typename T, typename K, typename series_templ>
epsilon_two_algorithm<T, K, series_templ>::epsilon_two_algorithm(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T epsilon_two_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    if (n == 0)
        return DEF_UNDEFINED_SUM;

    if (order == 0)
        return this->series->S_n(n);

    K k = 2 * order + n - (n & 1);

    std::vector<std::vector<T> > e(4, std::vector<T>(k + 3, 0));
    //4 vectors k+3 length containing four Epsilon Table rows

    for (K j = k; j >= 0; --j) //Counting first row of Epsilon Table
        e[3][j] = this->series->S_n(j);

    T a = 0, a1 = 0, a2 = 0;

    while (k > -1) {
        for (int i = 0; i < k; ++i) {
            e[0][i] = static_cast<T>(e[2][i + 1] + 1.0 / (e[3][i + 1] - e[3][i])); //Standart Epsilon Wynn algorithm

            if (!std::isfinite(e[0][i]) && i + 2 <= k) //This algorithm is used if new elliment is corrupted.
            {
                a2 = static_cast<T>(1.0 / e[2][i + 1]);

                a1 = static_cast<T>(1.0 / (1.0 - a2 * e[2][i + 2]));
                a = e[2][i + 2] * a1;

                a1 = static_cast<T>(1.0 / (1.0 - a2 * e[2][i]));
                a += e[2][i] * a1;

                a1 = static_cast<T>(1.0 / (1.0 - a2 * e[0][i + 2]));
                a -= e[0][i + 2] * a1;

                e[0][i] = static_cast<T>(1.0 / e[2][i + 1]);
                e[0][i] = static_cast<T>(1.0 / (1.0 + a * e[0][i]));
                e[0][i] = e[0][i] * a;
            }
            if (!std::isfinite(e[0][i]))
                //If new element is still corrupted we just copy prev. element, so we will get result
                    e[0][i] = e[2][i];
        }
        std::swap(e[0], e[1]); //Swapping rows of Epsilon Table. First ine will be overwriteen next turn
        std::swap(e[1], e[2]);
        std::swap(e[2], e[3]);

        --k;
    }

    if (!std::isfinite(e[0][0]))
        throw std::overflow_error("division by zero");

    return e[0][0]; //Only odd rows have mathmatical scense. Always returning e[0][0]
}