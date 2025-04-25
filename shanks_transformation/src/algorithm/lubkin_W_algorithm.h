#pragma once
#define DEF_UNDEFINED_SUM 0

#include "../series_acceleration.h"

template<typename T, typename K, typename series_templ>
class W_lubkin_algorithm : public series_acceleration<T, K, series_templ> {
protected:

    T calculate(K n, const int order, T S_n, const K j) const {
        /*
        * j - to fix n
        * S_n - partial sum of series.
        */
        for (K i = 0; i < j; ++i)
            S_n += this->series->operator()(n + 1 + i);

        n += j;
        if (order == 0)
            return S_n;

        //calculate all basic parts of transform
        int order_1 = order - 1;
        T W0 = calculate(n, order_1, S_n, 0);
        T W1 = calculate(n, order_1, S_n, 1);
        T W2 = calculate(n, order_1, S_n, 2);
        T W3 = calculate(n, order_1, S_n, 3);

        //optimization calculations
        T Wo0 = (W1 - W0);
        T Wo1 = (W2 - W1);
        T Wo2 = (W3 - W2);
        T Woo1 = Wo0 * (Wo2 - Wo1);
        T Woo2 = Wo2 * (Wo1 - Wo0);

        //T result = W1 - ((W2 - W1) * (W1 - W0) * (W3 - 2 * W2 + W1)) / ((W3 - W2) * (W2 - 2 * W1 + W0) - (W1 - W0) * (W3 - 2 * W2 + W1)); //straigh
        T result = W1 - (Wo1 * Woo1) / (Woo2 - Woo1); // optimized

        if (!std::isfinite(result))
            throw std::overflow_error("division by zero");

        return result;
    }

public:
    W_lubkin_algorithm(const series_templ &series) : series_acceleration<T, K, series_templ>(series) {
    }

    T operator()(const K n, const int order) const {
        if (order < 0)
            throw std::domain_error("negative order input");

        T S_n = this->series->S_n(n);
        return calculate(n, order, S_n, 0);
    }
};
