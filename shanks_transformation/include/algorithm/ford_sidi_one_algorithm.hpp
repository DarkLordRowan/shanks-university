#pragma once

template<typename T, typename K, typename series_templ>
class ford_sidi_one_algorithm final: public series_acceleration<T, K, series_templ> {
public:
    explicit ford_sidi_one_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;
protected:
    const shanks_transform<T, K, series_templ> *shanks_trans;


    T Psi(const K n, const int k, const series_base<T, K> *u, const shanks_transform<T, K, series_templ> *g) const;
    T Psi(const K n, const int k, const int k_1, const shanks_transform<T, K, series_templ> *g) const;
};

#include "ford_sidi_one_algorithm.tpp"
