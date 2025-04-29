
template<typename T, typename K, typename series_templ>
class t_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    t_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, t_transform{});
    }
};
