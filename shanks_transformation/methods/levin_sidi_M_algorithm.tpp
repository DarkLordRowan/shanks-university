template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_M_algorithm<T, K, series_templ>::calculate(const K n, const K order) const {

	using std::isfinite;

	if (gamma - static_cast<T>(n - static_cast<K>(1)) <= static_cast<T>(0))
		throw std::domain_error("gamma cannot be lesser than n-1");

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T rest;

	T up = static_cast<T>(1), down = static_cast<T>(1);
	T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), static_cast<K>(0));
	T S_n = this->series->S_n(order);

	T down_coef = gamma + static_cast<T>(order + static_cast<K>(2));
	T   up_coef = down_coef - static_cast<T>(n);
	
	for (K m = static_cast<K>(0); m < n - static_cast<K>(1); ++m) {
		up   *= (up_coef   + static_cast<T>(m));
		down *= (down_coef + static_cast<T>(m));
	}
	up /= down;
	down_coef = gamma + static_cast<T>(order + static_cast<K>(1));
	up_coef   = down_coef - static_cast<T>(n + static_cast<K>(1));
	
	for (K j = static_cast<K>(0); j <= n; ++j) {

		rest  = this->series->minus_one_raised_to_power_n(j); 
		rest *= binomial_coef * static_cast<T>(n - j);
		rest *= up; 
		rest /= static_cast<T>(j + static_cast<K>(1));

		up /= (  up_coef + static_cast<T>(j));
		up *= (down_coef + static_cast<T>(j));

		rest *= remainder_func->operator()(
			order, 
			j, 
			this->series, 
			-gamma-static_cast<T>(n)
		);

		numerator += rest * S_n ;
		denominator += rest;

		S_n += this->series->operator()(order + j + static_cast<K>(1));

	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_sidi_M_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {
	return calculate(n, order);
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_sidi_M_algorithm<T, K, series_templ>::levin_sidi_M_algorithm(
	const series_templ& series, 
	const remainder_type variant, 
	const T gamma_
	) :
	series_acceleration<T, K, series_templ>(series),
	gamma(gamma_)
{

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
    }
}