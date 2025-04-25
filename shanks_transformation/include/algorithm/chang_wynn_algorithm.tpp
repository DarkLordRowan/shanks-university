#pragma once

template<typename T, typename K, typename series_templ>
chang_whynn_algorithm<T, K, series_templ>::chang_whynn_algorithm(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T chang_whynn_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    if (n <= 0)
        throw std::domain_error("negative or zero integer in the input");

    T up, down, coef, coef2;

    K max = n - (n & 1); // int -> K mark

    std::vector<std::vector<T> > e(2, std::vector<T>(n, 0));
    //2 vectors n-1 length containing Epsilon table next and previous
    std::vector<T> f(n, 0); //vector for containing F results from 0 to n-1

    for (int i = 0; i < max; ++i) //Counting first row of Epsilon Table
        e[0][i] = static_cast<T>(1.0 / (this->series->operator()(i + 1)));

    for (int i = 0; i < max; ++i) //Counting F function
    {
        coef = (this->series->S_n(i + 3) + this->series->S_n(i + 1) - 2 * this->series->S_n(i + 2));
        coef2 = (this->series->S_n(i + 2) + this->series->S_n(i) - 2 * this->series->S_n(i + 1));

        up = this->series->operator()(i + 1) * this->series->operator()(i + 2) * coef;
        down = this->series->operator()(i + 3) * coef2;
        down -= (this->series->operator()(i + 1)) * coef;
        down = static_cast<T>(1.0 / down);
        e[1][i] = static_cast<T>(this->series->S_n(i + 1) - up * down);

        f[i] = coef * coef2 * down; //Can make coeff2 ^2 for better effect
    }

    for (int k = 2; k <= max; ++k) //Counting from 2 to n rows of Epsilon Table
    {
        for (int i = 0; i < max - k; ++i) {
            up = 1 - k + k * f[i];
            down = static_cast<T>(1.0 / (e[1][i + 1] - e[1][i]));
            e[0][i] = e[0][i + 1] + up * down;

            if (!std::isfinite(e[0][i])) //Check for invalid values to avoid them
            {
                max = k + i + 1;
                break;
            }
        }
        std::swap(e[0], e[1]); //Swapping 1 and 2 rows of Epsilon Table. First ine will be overwriteen next turn
    }

    const T result = e[max & 1][0]; //Only odd rows have mathmatical scence. Always returning e[0][0]

    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}
