#pragma once

#include "../series_acceleration.h"
#include "../series.h"

template<typename T, typename K, typename series_templ>
class ford_sidi_algorithm : public series_acceleration<T, K, series_templ> {
public:
    ford_sidi_algorithm(const series_templ &series) : series_acceleration<T, K, series_templ>(series) {
        shanks_trans = new shanks_transform<T, K, series_templ>(this->series);
    }

    ~ford_sidi_algorithm() { delete shanks_trans; }

    T operator()(const K n, const int k) const {
        if (n < 0)
            throw std::domain_error("negative integer in the input");

        one_series<T, K> *ones_seq = new one_series<T, K>();

        // Насколько мы коллективным разумом поняли, g это как бы ряд используемого преобразования(shanks_transform, richardson_extrapolation, G_transformation и тд)
        // пока я не реализовал выбор и оставил только shanks_transform, хотя в дальнейшем выбор конечно же надо прикрутить

        T T_n_k = Psi(1 + 1, k - 1, (this->series), shanks_trans) - Psi(1, k - 1, (this->series), shanks_trans);
        T_n_k /= (Psi(1 + 1, k - 1, ones_seq, shanks_trans) - Psi(1, k - 1, ones_seq, shanks_trans));

        for (K i = 2; i <= n; ++i) {
            T a = Psi(i + 1, k - 1, (this->series), shanks_trans) - Psi(i, k - 1, (this->series), shanks_trans);
            a /= (Psi(i + 1, k - 1, ones_seq, shanks_trans) - Psi(i, k - 1, ones_seq, shanks_trans));
            T_n_k += a;
        }
        return T_n_k;
    }

protected:
    const shanks_transform<T, K, series_templ> *shanks_trans;

    T Psi(const K n, const int k, const series_base<T, K> *u, const shanks_transform<T, K, series_templ> *g) const {
        if (k == 0)
            return (u->operator()(n)) / (g->operator()(n, 1));

        return (Psi(n + 1, k - 1, u, g) - Psi(n, k - 1, u, g)) / (
                   Psi(n + 1, k - 1, k + 1, g) - Psi(n, k - 1, k + 1, g));
    }

    T Psi(const K n, const int k, const int k_1, const shanks_transform<T, K, series_templ> *g) const {
        if (k == 0)
            return (g->operator()(n, k_1)) / (g->operator()(n, 1));

        return (Psi(n + 1, k - 1, k_1, g) - Psi(n, k - 1, k_1, g)) / (
                   Psi(n + 1, k - 1, k + 1, g) - Psi(n, k - 1, k + 1, g));
    }
};
