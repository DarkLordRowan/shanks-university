template<typename T, typename K, typename series_templ>
class recursive_v_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    recursive_v_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, v_transform{});
    }
};
