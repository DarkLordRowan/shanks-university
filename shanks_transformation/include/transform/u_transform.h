
class u_transform {
    const T beta;

public:
    u_transform(const T beta_ = T(1)) : beta(beta_) {
    };

    template<typename T, typename K>
    T operator()(const int n, const int j, const series_base<T, K> *series) const {
        return T(1) / ((beta + n) * series->operator()(n + j + 1));
    }
};

