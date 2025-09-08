template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    // in the method we don't use order, it's only a stub 
    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    std::vector<std::vector<T>> e(
        2,
        std::vector<T>(
            n + static_cast<K>(1),
            static_cast<T>(0)
        )
    ); //2 vectors n + 1 length containing Richardson table next and previous 
    
    for (K i = static_cast<K>(0); i <= n; ++i)
        e[0][i] = this->series->S_n(i);

    // The Richardson method main function 
    T a, b;
    a = static_cast<T>(1);
    for (K l = static_cast<K>(1); l <= n; ++l) {
        a *= static_cast<T>(4);
        b = a - static_cast<T>(1);

        for (K m = l; m <= n; ++m){
            e[1][m] = fma(a, e[0][m], -e[0][m - static_cast<K>(1)]); 
            e[1][m]/= b;
        }

        std::swap(e[0], e[1]);
    }

    if (!isfinite(e[n & static_cast<K>(1)][n])) // get n & 1, cause if n is even, result is e[0][n], if n is odd, result is e[1][n]
        throw std::overflow_error("division by zero");

    return e[n & static_cast<K>(1)][n];
}