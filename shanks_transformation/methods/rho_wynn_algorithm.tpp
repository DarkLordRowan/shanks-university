template <std::floating_point T, std::unsigned_integral K, typename series_templ>
rho_Wynn_algorithm<T, K, series_templ>::rho_Wynn_algorithm(
	const series_templ& series, 
	const numerator_type variant,
	const T gamma_, 
	const T RHO_
	) : 
	series_acceleration<T, K, series_templ>(series), 
	gamma(gamma_), 
	RHO(RHO_) 
{
	//TODO: проверить можно ли использовать стандартные значения, указанный в конструктуре, в других вариантах + унифицировать названия
	// есть ли какие то ограничения на кастомные значения?
	// classic (0), gamma (1), gamma-rho (2): 
	// 0 -> gamma = 1, RHO = 0
	// 1 -> gamma = 2, RHO = 0
	// 2 -> gamma = 2, RHO = 1

	switch(variant) {
		case numerator_type::rho_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		case numerator_type::generalized_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		case numerator_type::gamma_rho_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		default:
			numerator_func.reset(new rho_transform<T, K>());
	}
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T rho_Wynn_algorithm<T, K, series_templ>::calculate(const K n, K order) const { //const int order	

	using std::isfinite;

	if (order & static_cast<K>(1))// is order odd
		//++order; //why
		throw std::domain_error("order should be even number");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

	const T S_n = this->series->S_n(n);
	const K order1 = order - static_cast<K>(1);
	const T res = (
		recursive_calculate_body(
			n, 
			order1 - static_cast<T>(1),
			 S_n, 
			 static_cast<T>(1)
		) + 
		numerator_func->operator()(
			n, 
			order, 
			this->series, 
			gamma, 
			RHO)
		) / (
		recursive_calculate_body(
			n, 
			order1, 
			S_n, 
			static_cast<K>(1)
		) - 
		recursive_calculate_body(
			n, 
			order1,
			 S_n, 
			 static_cast<T>(0)
		)
	);

	if (!isfinite(res))
		throw std::overflow_error("division by zero");
	return res;	
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T rho_Wynn_algorithm<T, K, series_templ>::recursive_calculate_body(const K n, const K order, T S_n, const K j) const {

	using std::isfinite;
	/**
	* S_n - previous sum;
	* j - adjusts n: (n + j);
	*/

	S_n += (j == static_cast<K>(0)) ? static_cast<T>(0) : this->series->operator()(n + j);

	if (order == static_cast<K>(0))
		return S_n;

	if (order == static_cast<K>(-1))
		return static_cast<T>(0);

	const K order1 = order - static_cast<K>(1);
	const K nj = n + j;
	const T res = (
		recursive_calculate_body(
			nj, 
			order1 - static_cast<K>(1), 
			S_n, 
			static_cast<K>(1)
		) + 
		numerator_func->operator()(
			nj, 
			order, 
			this->series,
			gamma, 
			RHO)
		) / (
		recursive_calculate_body(
			nj, 
			order1,
			 S_n, 
			 static_cast<K>(1)
		) - 
		recursive_calculate_body(
			nj, 
			order1, 
			S_n, 
			static_cast<K>(0)
		)
	);

	if (!isfinite(res))
		throw std::overflow_error("division by zero");

	return res;
}