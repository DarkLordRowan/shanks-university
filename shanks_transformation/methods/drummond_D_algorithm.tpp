
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result(const K n, const K order) const {

    using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
	T rest;

	for (K j = static_cast<K>(0); j <= n; ++j) {

		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= this->series->binomial_coefficient(static_cast<T>(n), j);
		rest *= remainder_func->operator()(n,j, this->series);

		numerator   += rest * this->series->S_n(n+j);
		denominator += rest;
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result_rec(const K n, const K order) const {

    using std::isfinite;

	std::vector<T>   Num(order + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> Denom(order + static_cast<K>(1), static_cast<T>(0));

    //init the base values
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder_func->operator()(n, i, this->series);
		  Num[i] = this->series->S_n(n+i) * Denom[i];
	}

    //recurrence
	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {
			Denom[j] = Denom[j+static_cast<K>(1)] - Denom[j];
			  Num[j] =   Num[j+static_cast<K>(1)] -   Num[j];
		}

	Num[0] /= Denom[0];

	if (!isfinite(Num[0]))
		throw std::overflow_error("division by zero");
    
	return Num[0];
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
drummonds_D_algorithm<T,K,series_templ>::drummonds_D_algorithm(
	const series_templ& series, 
	const remainder_type variant, 
	bool useRecFormulas
	) : 
	series_acceleration<T, K, series_templ>(series),
	useRecFormulas(useRecFormulas)
{
    //check variant else default 'u'
    switch(variant){
        case remainder_type::u_variant :
            remainder_func.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder_func.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder_func.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder_func.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder_func.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
            break;
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T drummonds_D_algorithm<T,K,series_templ>::operator()(const K n, const K order) const { 

    using std::isfinite;

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}