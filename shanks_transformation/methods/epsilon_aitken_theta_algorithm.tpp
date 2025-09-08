template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_aitken_theta_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;

    if (n < static_cast<K>(4))
        return static_cast<T>(0); //TODO: диагностический вывод

    std::vector<T> current(n);

    for (K i = static_cast<K>(0); i < n; ++i)
        current[i] = this->series->operator()(i);

    T delta, delta_next;
    T gamma, lambda;
    T ck = static_cast<T>(1);

    for (K k = static_cast<K>(1); k <= n; ++k) { 

        std::vector<T> step1(n, static_cast<T>(0));
        std::vector<T> step3(n, static_cast<T>(0));

        switch (order){
            case static_cast<K>(1):
                gamma = static_cast<T>(1);
                gamma/= static_cast<T>(2);
                gamma/= static_cast<T>(k);

                lambda = static_cast<T>(1) - gamma;

                break;
            case static_cast<K>(2):

                gamma = static_cast<T>(2);
                gamma/= static_cast<T>(3);
                gamma/= static_cast<T>(k);

                lambda = static_cast<T>(1) - gamma;

                break;
            default:
                gamma = static_cast<T>(1);
                gamma/=(k + static_cast<T>(1));

                lambda =static_cast<T>(k);
                lambda*=gamma;
        }

        switch (order){
            case static_cast<K>(1):
                ck+= static_cast<T>(order /(static_cast<K>(2) * k - static_cast<K>(1)));
                break;

            case static_cast<K>(2):
                ck+= static_cast<T>(static_cast<K>(1)/(static_cast<K>(3) * k - static_cast<K>(2)));
                break;

            default:
                ck+=static_cast<T>(order / k);
        }

        step1[0] = current[1] - ck * (current[1] - current[0]) / (current[2] - current[1]);

        K i1, i2, i3;

        for (K i = static_cast<K>(0); i < static_cast<K>(step3.size() - 3); ++i) {

            i1 = i + static_cast<K>(1);
            i2 = i + static_cast<K>(2);
            i3 = i + static_cast<K>(3);

            step1[i1] = current[i2];
            step1[i1]-= ck * (current[i2] - current[i1]) / (current[i3] - current[i2]);

            delta = step1[i1] - step1[i];

            delta_next = (i2 < static_cast<K>(step1.size()) ? 
                step1[i2] - step1[i1] 
                : 
                static_cast<T>(0)
            );

            step3[i] = gamma * step1[i] + lambda * (step1[i1] + (delta * delta_next) / (delta - delta_next));
        }

        current = step3;
    }


    if (!isfinite(current[n - static_cast<K>(4)]))
        throw std::overflow_error("division by zero");

    return current[n - static_cast<K>(4)];
}