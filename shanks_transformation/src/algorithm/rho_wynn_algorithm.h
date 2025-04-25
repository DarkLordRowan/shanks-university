#pragma once
#define DEF_UNDEFINED_SUM 0

#include "../series_acceleration.h"

template<typename T, typename K, typename series_templ>
class rho_Wynn_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const numerator_base<T, K> *numerator_func;
    const T gamma;
    const T RHO;

    T calculate(const K n, int order) const {
        //const int order
        if (order & 1) {
            ++order;
            throw std::domain_error("order should be even number");
        }

        if (n < 0 || order < 0)
            throw std::domain_error("negative integer in the input");

        if (order == 0)
            return this->series->S_n(n);

        T S_n = this->series->S_n(n);

        int order_1 = order - 1;

        T res = recursive_calculate_body(n, order_1 - 1, S_n, 1) + (numerator_func->operator()(
                    n, order, this->series, gamma, RHO));
        res /= (recursive_calculate_body(n, order_1, S_n, 1) - recursive_calculate_body(n, order_1, S_n, 0));

        if (!std::isfinite(res))
            throw std::overflow_error("division by zero");

        return res;
    }

    T recursive_calculate_body(const K n, const int order, T S_n, const K j) const {
        /**
        * S_n - previous sum;
        * j - adjusts n: (n + j);
        */
        S_n += (j == K(0)) ? T(0) : this->series->operator()(n + j);
        if (order == 0)
            return S_n;

        if (order == -1)
            return 0;

        int order_1 = order - 1;
        K n_j = n + j;

        T res = recursive_calculate_body(n_j, order_1 - 1, S_n, 1) + (numerator_func->operator()(
                    n_j, order, this->series, gamma, RHO));
        res /= (recursive_calculate_body(n_j, order_1, S_n, 1) - recursive_calculate_body(n_j, order_1, S_n, 0));
        if (!std::isfinite(res))
            throw std::overflow_error("division by zero");

        return res;
    }

public:
    rho_Wynn_algorithm(const series_templ &series, const numerator_base<T, K> *func, const T gamma_ = T(1),
                       const T RHO_ = T(0)) : series_acceleration<T, K, series_templ>(series), numerator_func(func),
                                              gamma(gamma_), RHO(RHO_) {
        if (func == nullptr)
            throw std::domain_error("null poniter numerator function");
    }

    ~rho_Wynn_algorithm() {
        if (numerator_func != nullptr)
            delete numerator_func;
    }

    T operator()(const K n, const int order) const {
        return calculate(n, order);
    }
};
