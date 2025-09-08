template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm_two<T, K, series_templ>::epsilon_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm_two<T, K, series_templ>::operator()(const K n, const K order) const
{

    using std::isfinite;

    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    if (order == static_cast<K>(0))
        return this->series->S_n(n);

    K k = static_cast<K>(2);
    k *= order;
    k += n;
    k -= (n & static_cast<K>(1));

    std::vector<std::vector<T>> e(
        4, 
        std::vector<T>(
            k + static_cast<K>(3), 
            static_cast<T>(0)
        )
    ); //4 vectors k+3 length containing four Epsilon Table rows 

    K j = k;
    do { //Counting first row of Epsilon Table
        e[3][j] = this->series->S_n(j);
    } while (--j > static_cast<K>(0));

    T a = static_cast<T>(0);

    K i1, i2;

    while (k > static_cast<K>(-1)) {
        for (K i = static_cast<K>(0); i < k; ++i) {

            i1 = i + static_cast<K>(1);
            i2 = i + static_cast<K>(2);

            e[0][i] = e[2][i1];
            e[0][i]+= static_cast<T>(1) / (e[3][i1] - e[3][i]); //Standart Epsilon Wynn algorithm

            if (!isfinite(e[0][i]) && i2 <= k) { //This algorithm is used if new elliment is corrupted.
                a = e[2][i2] * static_cast<T>(1) / (static_cast<T>(1) - e[2][i2] / e[2][i1]);

                a += e[2][i] * static_cast<T>(1) / (static_cast<T>(1) - e[2][i] / e[2][i1]);

                a -= e[0][i2] * static_cast<T>(1)/ (static_cast<T>(1) - e[0][i2] / e[2][i1]);

                e[0][i] = a / (static_cast<T>(1) + a / e[2][i1]);
            }

            if (!isfinite(e[0][i])) //If new element is still corrupted we just copy prev. element, so we will get result
                e[0][i] = e[2][i];         
        }

        std::swap(e[0], e[1]); //Swapping rows of Epsilon Table. First ine will be overwriteen next turn
        std::swap(e[1], e[2]);
        std::swap(e[2], e[3]);

        --k;
    }
    
    if (!isfinite(e[0][0]))
        throw std::overflow_error("division by zero");

    return e[0][0];  //Only odd rows have mathmatical scense. Always returning e[0][0]
}