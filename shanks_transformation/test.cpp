/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 */

#include <memory>
#include <string> 
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <random>
#include <set>

#include "wynn_numerators.h"
#include "remainders.h"
#include "shanks_transformation.h"
#include "epsilon_algorithm.h"
#include "levin_algorithm.h"
#include "levin_sidi_S_algorithm.h"
#include "drummond_D_algorithm.h"
#include "epsilon_algorithm_two.h"
#include "chang_whynn_algorithm.h"
#include "test_functions.h"
#include "levin_sidi_M_algorithm.h"
#include "weniger_algorithm.h"
#include "rho_wynn_algorithm.h"
#include "brezinski_theta_algorithm.h"
#include "epsilon_algorithm_three.h"
#include "levin_recursion_algorithm.h"
#include "lubkin_W_algorithm.h"
#include "richardson_algorithm.h"
#include "FSA.h"
#include "FSA_two.h"
#include "FSA_3.h"

/**
 * @brief Enum of transformation IDs
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G.
 */
enum transformation_id_t {
    null_transformation_id,
    shanks_transformation_id,
    epsilon_algorithm_id,
    levin_algorithm_id,
    epsilon_algorithm_2_id,
    S_algorithm,
    D_algorithm,
    chang_epsilon_algorithm,
    M_algorithm,
    weniger_transformation,
    rho_wynn_transformation_id,
    brezinski_theta_transformation_id,
    epsilon_algorithm_3_id,
    levin_recursion_id,
    W_algorithm_id,
    richardson_algorithm_id,
    Ford_Sidi_algorithm_id,
    Ford_Sidi_algorithm_two_id,
    Ford_Sidi_algorithm_three_id
};
/**
 * @brief Enum of series IDs
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G.
 */
enum series_id_t {
    null_series_id, 
    exp_series_id, 
    cos_series_id, 
    sin_series_id, 
    cosh_series_id,
    sinh_series_id, 
    bin_series_id, 
    four_arctan_series_id, 
    ln1mx_series_id, 
    mean_sinh_sin_series_id,
    exp_squared_erf_series_id, 
    xmb_Jb_two_series_id, 
    half_asin_two_x_series_id,
    inverse_1mx_series_id,
    x_1mx_squared_series_id,
    erf_series_id,
    m_fact_1mx_mp1_inverse_series_id,
    inverse_sqrt_1m4x_series_id,
    one_twelfth_3x2_pi2_series_id,
    x_twelfth_x2_pi2_series_id,
    ln2_series_id,
    one_series_id,
    minus_one_quarter_series_id,
    pi_3_series_id,
    pi_4_series_id,
    pi_squared_6_minus_one_series_id,
    three_minus_pi_series_id,
    one_twelfth_series_id,
    eighth_pi_m_one_third_series_id,
    one_third_pi_squared_m_nine_series_id,
    four_ln2_m_3_series_id,
    exp_m_cos_x_sinsin_x_series_id,
    pi_four_minus_ln2_halfed_series_id,
    five_pi_twelve_series_id,
    x_two_series_id,
    pi_six_min_half_series_id,
    x_two_throught_squares_id,
    minus_one_ned_in_n_series_id,
    minus_one_n_fact_n_in_n_series_id,
    ln_x_plus_one_x_minus_one_halfed_series_id,
    two_arcsin_square_x_halfed_series_id,
    pi_squared_twelve_series_id,
    pi_cubed_32_series_id,
    minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id,
    two_ln2_series_id,
    pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id,
    pi_minus_x_2_id,
    half_multi_ln_1div2multi1minuscosx_id,
    half_minus_sinx_multi_pi_4_id,
    ln_1plussqrt1plusxsquare_minus_ln_2_id,
    ln_cosx_id,
    ln_sinx_minus_ln_x_id,
    pi_8_cosx_square_minus_1_div_3_cosx_id,
    sqrt_oneminussqrtoneminusx_div_x_id,
    one_minus_sqrt_1minus4x_div_2x_id,
    arcsin_x_minus_x_series_id, 
    pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id,
    abs_sin_x_minus_2_div_pi_series_id,
    pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id,
    minus_3_div_4_or_x_minus_3_div_4_series_id,
    ten_minus_x_series_id,
    x_series_id,
    minus_x_minus_pi_4_or_minus_pi_4_series_id,
    one_div_two_minus_x_multi_three_plus_x_series_id,
    Si_x_series_id,
    Ci_x_series_id,
    Riemann_zeta_func_series_id,
    Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id,
    xsquareplus3_div_xsquareplus2multix_minus_1_series_id,
    arcsin_x_series_id,
    arctg_x_series_id,
    K_x_series_id,
    E_x_series_id,
    sqrt_1plusx_series_id,
    Lambert_W_func_series_id,
    Incomplete_Gamma_func_series_id,
    Series_with_ln_number1_id,
    Series_with_ln_number2_id,
    pi_series_id,
    x_min_sqrt_x_series_id,
    arctan_x2_series_id,
    ln1px4_series_id,
    sin_x2_series_id,
    arctan_x3_series_id,
    arcsin_x2_series_id,
    ln1_m_x2_series_id,
    artanh_x_series_id,
    arcsinh_x_series_id,
    cos_x2_series_id,
    sinh_x2_series_id,
    arctanh_x2_series_id,
    cos3xmin1_div_xsqare_series_id,
    two_degree_x_series_id,
    sqrt_1plusx_min_1_min_x_div_2_series_id,
    ln13_min_ln7_div_7_series_id,
    Ja_x_series_id,
    one_div_sqrt2_sin_xdivsqrt2_series_id,
    ln_1plusx_div_1plusx2_id,
    cos_sqrt_x_id,
    ln_1_plus_x3_id,
    x_div_1minx_id,
    x_div_1minx2_id,
    gamma_series_id,
    ModifiedSeries_id,
    ArraySeries_id
};

/**
 * @brief Enum of testing functions IDs
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G.
 */
enum test_function_id_t {
    null_test_function_id, 
    cmp_sum_and_transform_id, 
    cmp_a_n_and_transform_id, 
    transformation_remainder_id, 
    cmp_transformations_id,
    eval_transform_time_id,
    test_all_transforms_id
};

/**
 * @brief initialize LevinType transformations, usable for S,D,M
 * @authors Naumov A.
 * @edited by Yurov P.
 */
template<typename T, typename K, typename series_templ>
inline void init_levin(transformation_id_t id, std::unique_ptr<series_base<T,K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{
    bool recursive = false;
    char type;

    std::cout << std::endl;
    std::cout << "|--------------------------------------|" << std::endl;
    std::cout << "| choose what type of transformation u,t,d or v: "; std::cin >> type; std::cout << "|" << std::endl;
    if (id != transformation_id_t::M_algorithm)
    {
        std::cout << "| Use recurrence formula? 1<-true or 0<-false : "; std::cin >> recursive; std::cout << "|" << std::endl;
    }
    std::cout << "|--------------------------------------|" << std::endl;

    T beta = T(0);		//parameter for LevinType transformations algorithm
    T gamma = T(10);	//parameter for LevinType transformations algorithm

    //transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get(), beta));

    transform_base<T, K>* ptr = NULL;

    if (type == 'u') ptr = new u_transform<T, K>{};
    if (type == 't') ptr = new t_transform<T, K>{}; 
    if (type == 'v') { 
        if (id != transformation_id_t::M_algorithm)
            ptr = new v_transform<T, K>{};
        else
            ptr = new v_transform_2<T, K>{};
    }
    if (type == 'd') ptr = new d_transform<T, K>{};

    if (ptr == NULL) throw std::domain_error("chosen wrong type of transformation");

    switch (id) {
        case transformation_id_t::S_algorithm:

            std::cout << std::endl;
            std::cout << "|------------------------------------------|" << std::endl;
            std::cout << "| Enter parameter beta: "; std::cin >> beta;
            std::cout << "|------------------------------------------|" << std::endl;

            transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), ptr, recursive, beta));
            return;
        case transformation_id_t::D_algorithm:
            transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), ptr, recursive));
            return;
        case transformation_id_t::M_algorithm:

            std::cout << std::endl;
            std::cout << "|------------------------------------------|" << std::endl;
            std::cout << "| Enter parameter beta: "; std::cin >> gamma;
            std::cout << "|------------------------------------------|" << std::endl;

            transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), ptr, gamma));
            return;
        default:
            throw std::domain_error("wrong id was given");
    }	
}

/**
 * @brief initialize rho-WynnType transformations, usable for basic, Gamma, Gamma-Rho
 * @authors Yurov P.
 */
template<typename T, typename K, typename series_templ>
inline void init_wynn(std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{

    int type;
    T gamma = T{};	//parameter for gamma modification
    T RHO = T{};	//parameter for gamma-rho modification

    std::cout << std::endl;
    std::cout << "|------------------------------------------|" << std::endl;
    std::cout << "| choose transformation variant:           |" << std::endl;
    std::cout << "| classic (0), gamma (1), gamma-rho (2): "; std::cin >> type;
    std::cout << "|------------------------------------------|" << std::endl;

    switch (type) {
    case 0:
        transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new rho_transform<T, K>{}));
        break;
    case 1:
        std::cout << std::endl;
        std::cout << "|------------------------------------------|" << std::endl;
        std::cout << "| Enter parameter gamma: "; std::cin >> gamma;
        std::cout << "|------------------------------------------|" << std::endl;

        transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new generilized_transform<T, K>{}, gamma));
        break;
    case 2:
        std::cout << std::endl;
        std::cout << "|------------------------------------------|" << std::endl;
        std::cout << "| Enter parameter gamma: "; std::cin >> gamma;
        std::cout << "| Enter parameter RHO: "; std::cin >> RHO;
        std::cout << "|------------------------------------------|" << std::endl;

        transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new gamma_rho_transform<T, K>{}, gamma, RHO));
        break;
    default:
        throw std::domain_error("wrong transform variant");
        break;
    }
}

/**
 * @brief initialize levin_recursion transformation
 * @authors Maximov A.K.
 */
template<typename T, typename K, typename series_templ>
inline void init_levin_recursion(std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{

    T beta = T{};	//parameter for levin_recursion algorithm

    std::cout << std::endl;
    std::cout << "|------------------------------------------|" << std::endl;
    std::cout << "| Enter parameter beta: "; std::cin >> beta;
    std::cout << "|------------------------------------------|" << std::endl;

    transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get(), beta));
}


 /**
 * @brief A function that produces a random value from -100 to 100 and matches when i is repeated
 * @authors Sharonov K.S.
 * @param i The number of term
 */
int get_cached_random(int i) {
    static auto cache = std::unordered_map<int, int>{};
    
    static auto gen = std::mt19937(std::random_device{}());
    static auto dist = std::uniform_int_distribution<int>(-100, 100);
    
    auto it = cache.find(i);
    if (it != cache.end()) {
        return it->second;
    }

    int value = dist(gen);
    cache[i] = value;
    return value;
}

template <typename T, typename K>
inline static void main_testing_function(){

    //choosing series
    std::unique_ptr<series_base<T, K>> series, series1;
    int series_id = 0, count_arr = 0;
    std::vector<T> arr;
    std::cin >> series_id;
    T x = 0;
    std::set<int> alternating_series = { 2, 3, 7, 11, 15, 18, 19, 20, 21, 24, 26, 28, 30, 31};

    if(series_id != -1){
        //choosing x
        std::cin >> x;

        //choosing series (cont.)
        switch (series_id)
        {
        case series_id_t::exp_series_id:
            series.reset(new exp_series<T, K>(x));
            break;
        case series_id_t::cos_series_id:
            series.reset(new cos_series<T, K>(x));
            break;
        case series_id_t::sin_series_id:
            series.reset(new sin_series<T, K>(x));
            break;
        case series_id_t::cosh_series_id:
            series.reset(new cosh_series<T, K>(x));
            break;
        case series_id_t::sinh_series_id:
            series.reset(new sinh_series<T, K>(x));
            break;
        case series_id_t::bin_series_id:
            T alpha;
            std::cout << "Enter the value for constant alpha for the series" << std::endl;
            std::cin >> alpha;
            series.reset(new bin_series<T, K>(x, alpha));
            break;
        case series_id_t::four_arctan_series_id:
            series.reset(new four_arctan_series<T, K>(x));
            break;
        case series_id_t::ln1mx_series_id:
            series.reset(new ln1mx_series<T, K>(x));
            break;
        case series_id_t::mean_sinh_sin_series_id:
            series.reset(new mean_sinh_sin_series<T, K>(x));
            break;
        case series_id_t::exp_squared_erf_series_id:
            series.reset(new exp_squared_erf_series<T, K>(x));
            break;
        case series_id_t::xmb_Jb_two_series_id:
            K  b;
            std::cout << "Enter the value for constant b for the series" << std::endl;
            std::cin >> b;
            series.reset(new xmb_Jb_two_series<T, K>(x, b));
            break; 
        case series_id_t::half_asin_two_x_series_id:
            series.reset(new half_asin_two_x_series<T, K>(x));
            break;
        case series_id_t::inverse_1mx_series_id:
            series.reset(new inverse_1mx_series<T, K>(x));
            break;
        case series_id_t::x_1mx_squared_series_id:
            series.reset(new x_1mx_squared_series<T, K>(x));
            break;
        case series_id_t::erf_series_id:
            series.reset(new erf_series<T, K>(x));
            break;
        case series_id_t::m_fact_1mx_mp1_inverse_series_id:
            K m;
            std::cout << "Enter the value for constant m for the series" << std::endl;
            std::cin >> m;
            series.reset(new m_fact_1mx_mp1_inverse_series<T, K>(x, m));
            break; 
        case series_id_t::inverse_sqrt_1m4x_series_id:
            series.reset(new inverse_sqrt_1m4x_series<T, K>(x));
            break;
        case series_id_t::one_twelfth_3x2_pi2_series_id:
            series.reset(new one_twelfth_3x2_pi2_series<T, K>(x));
            break; 
        case series_id_t::x_twelfth_x2_pi2_series_id:
            series.reset(new x_twelfth_x2_pi2_series<T, K>(x));
            break;
        case series_id_t::ln2_series_id:
            series.reset(new ln2_series<T, K>());
            break;
        case series_id_t::one_series_id:
            series.reset(new one_series<T, K>());
            break;
        case series_id_t::minus_one_quarter_series_id:
            series.reset(new minus_one_quarter_series<T, K>());
            break;
        case series_id_t::pi_3_series_id:
            series.reset(new pi_3_series<T, K>());
            break;
        case series_id_t::pi_4_series_id:
            series.reset(new pi_4_series<T, K>());
            break;
        case series_id_t::pi_squared_6_minus_one_series_id:
            series.reset(new pi_squared_6_minus_one_series<T, K>());
            break;
        case series_id_t::three_minus_pi_series_id:
            series.reset(new three_minus_pi_series<T, K>());
            break;
        case series_id_t::one_twelfth_series_id:
            series.reset(new one_twelfth_series<T, K>());
            break;
        case series_id_t::eighth_pi_m_one_third_series_id:
            series.reset(new eighth_pi_m_one_third_series<T, K>());
            break;
        case series_id_t::one_third_pi_squared_m_nine_series_id:
            series.reset(new one_third_pi_squared_m_nine_series<T, K>());
            break;
        case series_id_t::four_ln2_m_3_series_id:
            series.reset(new four_ln2_m_3_series<T, K>());
            break;
        case series_id_t::exp_m_cos_x_sinsin_x_series_id:
            series.reset(new exp_m_cos_x_sinsin_x_series<T, K>(x));
            break;
        case series_id_t::pi_four_minus_ln2_halfed_series_id:
            series.reset(new pi_four_minus_ln2_halfed_series<T, K>(x));
            break;
        case series_id_t::five_pi_twelve_series_id:
            series.reset(new five_pi_twelve_series<T, K>(x));
            break;
        case series_id_t::x_two_series_id:
            series.reset(new x_two_series<T, K>(x));
            break;
        case series_id_t::pi_six_min_half_series_id:
            series.reset(new pi_six_min_half_series<T, K>(x));
            break;
        case series_id_t::x_two_throught_squares_id:
            series.reset(new x_two_throught_squares_series<T, K>(x));
            break;
        case series_id_t::minus_one_ned_in_n_series_id:
            series.reset(new minus_one_ned_in_n_series<T, K>(x));
            break;
        case series_id_t::minus_one_n_fact_n_in_n_series_id:
            series.reset(new minus_one_n_fact_n_in_n_series<T, K>(x));
            break;
        case series_id_t::ln_x_plus_one_x_minus_one_halfed_series_id:
            series.reset(new ln_x_plus_one_x_minus_one_halfed_series<T, K>(x));
            break;
        case series_id_t::two_arcsin_square_x_halfed_series_id:
            series.reset(new two_arcsin_square_x_halfed_series<T, K>(x));
            break;
        case series_id_t::pi_squared_twelve_series_id:
            series.reset(new pi_squared_twelve_series<T, K>());
            break;
        case series_id_t::pi_cubed_32_series_id:
            series.reset(new pi_cubed_32_series<T, K>());
            break; 
        case series_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id:
            series.reset(new minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>());
            break; 
        case series_id_t::two_ln2_series_id:
            series.reset(new two_ln2_series<T, K>());
            break; 
        case series_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id:
            series.reset(new pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>(x));
            break; 
        case series_id_t::pi_minus_x_2_id:
            series.reset(new pi_minus_x_2<T, K>(x));
            break; 
        case series_id_t::half_multi_ln_1div2multi1minuscosx_id:
            series.reset(new half_multi_ln_1div2multi1minuscosx<T, K>(x));
            break; 
        case series_id_t::half_minus_sinx_multi_pi_4_id:
            series.reset(new half_minus_sinx_multi_pi_4<T, K>(x));
            break; 
        case series_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_id:
            series.reset(new ln_1plussqrt1plusxsquare_minus_ln_2<T, K>(x));
            break; 
        case series_id_t::ln_cosx_id:
            series.reset(new ln_cosx<T, K>(x));
            break; 
        case series_id_t::ln_sinx_minus_ln_x_id:
            series.reset(new ln_sinx_minus_ln_x<T, K>(x));
            break; 
        case series_id_t::pi_8_cosx_square_minus_1_div_3_cosx_id:
            series.reset(new pi_8_cosx_square_minus_1_div_3_cosx<T, K>(x));
            break; 
        case series_id_t::sqrt_oneminussqrtoneminusx_div_x_id:
            series.reset(new sqrt_oneminussqrtoneminusx_div_x<T, K>(x));
            break; 
        case series_id_t::one_minus_sqrt_1minus4x_div_2x_id:
            series.reset(new one_minus_sqrt_1minus4x_div_2x<T, K>(x));
            break; 
        case series_id_t::arcsin_x_minus_x_series_id:
            series.reset(new arcsin_x_minus_x_series<T, K>(x));
            break; 
        case series_id_t::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id:
            series.reset(new pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>(x));
            break; 
        case series_id_t::abs_sin_x_minus_2_div_pi_series_id:
            series.reset(new abs_sin_x_minus_2_div_pi_series<T, K>(x));
            break; 
        case series_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id:
            series.reset(new pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>(x));
            break; 
        case series_id_t::minus_3_div_4_or_x_minus_3_div_4_series_id:
            series.reset(new minus_3_div_4_or_x_minus_3_div_4_series<T, K>(x));
            break; 
        case series_id_t::ten_minus_x_series_id:
            series.reset(new ten_minus_x_series<T, K>(x));
            break; 
        case series_id_t::x_series_id:
            series.reset(new x_series<T, K>(x));
            break; 
        case series_id_t::minus_x_minus_pi_4_or_minus_pi_4_series_id:
            series.reset(new minus_x_minus_pi_4_or_minus_pi_4_series<T, K>(x));
            break; 
        case series_id_t::one_div_two_minus_x_multi_three_plus_x_series_id:
            series.reset(new one_div_two_minus_x_multi_three_plus_x_series<T, K>(x));
            break; 
        case series_id_t::Si_x_series_id:
            series.reset(new Si_x_series<T, K>(x));
            break; 
        case series_id_t::Ci_x_series_id:
            series.reset(new Ci_x_series<T, K>(x));
            break;
        case series_id_t::Riemann_zeta_func_series_id:
            series.reset(new Riemann_zeta_func_series<T, K>(x));
            break; 
        case series_id_t::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id:
            series.reset(new Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>(x));
            break; 
        case series_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_series_id:
            series.reset(new xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>(x));
            break; 
        case series_id_t::arcsin_x_series_id:
            series.reset(new arcsin_x_series<T, K>(x));
            break; 
        case series_id_t::arctg_x_series_id:
            series.reset(new arctg_x_series<T, K>(x));
            break; 
        case series_id_t::K_x_series_id:
            series.reset(new K_x_series<T, K>(x));
            break; 
        case series_id_t::E_x_series_id:
            series.reset(new E_x_series<T, K>(x));
            break; 
        case series_id_t::sqrt_1plusx_series_id:
            series.reset(new sqrt_1plusx_series<T, K>(x));
            break; 
        case series_id_t::Lambert_W_func_series_id:
            series.reset(new Lambert_W_func_series<T, K>(x));
            break; 
        case series_id_t::Incomplete_Gamma_func_series_id:
            T s;
            std::cout << "Enter the value for constant s for the series" << std::endl;
            std::cin >> s;
            series.reset(new Incomplete_Gamma_func_series<T, K>(x, s));
            break; 
        case series_id_t::Series_with_ln_number1_id:
            series.reset(new Series_with_ln_number1<T, K>());
            break; 
        case series_id_t::Series_with_ln_number2_id:
            series.reset(new Series_with_ln_number2<T, K>());
            break;
        case series_id_t::pi_series_id:
            series.reset(new pi_series<T, K>());
            break; 
        case series_id_t::x_min_sqrt_x_series_id:
            series.reset(new x_min_sqrt_x_series<T, K>(x));
            break; 
        case series_id_t::arctan_x2_series_id:
            series.reset(new arctan_x2_series<T, K>(x));
            break;
        case series_id_t::ln1px4_series_id:
            series.reset(new ln1px4_series<T, K>(x));
            break;
        case series_id_t::sin_x2_series_id:
            series.reset(new sin_x2_series<T, K>(x));
            break;
        case series_id_t::arctan_x3_series_id:
            series.reset(new arctan_x3_series<T, K>(x));
            break;
        case series_id_t::arcsin_x2_series_id:
            series.reset(new arcsin_x2_series<T, K>(x));
            break;
        case series_id_t::ln1_m_x2_series_id:
            series.reset(new ln1_m_x2_series<T, K>(x));
            break;
        case series_id_t::artanh_x_series_id:
            series.reset(new artanh_x_series<T, K>(x));
            break;
        case series_id_t::arcsinh_x_series_id:
            series.reset(new arcsinh_x_series<T, K>(x));
            break;
        case series_id_t::cos_x2_series_id:
            series.reset(new cos_x2_series<T, K>(x));
            break;
        case series_id_t::sinh_x2_series_id:
            series.reset(new sinh_x2_series<T, K>(x));
            break;
        case series_id_t::arctanh_x2_series_id:
            series.reset(new arctanh_x2_series<T, K>(x));
            break;
        case series_id_t::cos3xmin1_div_xsqare_series_id:
            series.reset(new cos3xmin1_div_xsqare_series<T, K>(x));
            break; 
        case series_id_t::two_degree_x_series_id:
            series.reset(new two_degree_x_series<T, K>(x));
            break; 
        case series_id_t::sqrt_1plusx_min_1_min_x_div_2_series_id:
            series.reset(new sqrt_1plusx_min_1_min_x_div_2_series<T, K>(x));
            break; 
        case series_id_t::ln13_min_ln7_div_7_series_id:
            series.reset(new ln13_min_ln7_div_7_series<T, K>());
            break; 
        case series_id_t::Ja_x_series_id:
            T a;
            std::cout << "Enter the value for constant a for the series" << std::endl;
            std::cin >> a;
            series.reset(new Ja_x_series<T, K>(x, a));
            break; 
        case series_id_t::one_div_sqrt2_sin_xdivsqrt2_series_id:
            series.reset(new one_div_sqrt2_sin_xdivsqrt2_series<T, K>(x));
            break; 
        case series_id_t::ln_1plusx_div_1plusx2_id:
            series.reset(new ln_1plusx_div_1plusx2<T, K>(x));
            break;
        case series_id_t::cos_sqrt_x_id:
            series.reset(new cos_sqrt_x<T, K>(x));
            break;
        case series_id_t::ln_1_plus_x3_id:
            series.reset(new ln_1_plus_x3<T, K>(x));
            break;
        case series_id_t::x_div_1minx_id:
            series.reset(new x_div_1minx<T, K>(x));
            break;
        case series_id_t::x_div_1minx2_id:
            series.reset(new x_div_1minx2<T, K>(x));
            break;
        case series_id_t::gamma_series_id:
            T t;
            std::cout << "Enter the parameter t in the gamma series" << std::endl;
            std::cin >> t;
            series.reset(new gamma_series<T, K>(t, x));
            break;
        case series_id_t::ModifiedSeries_id:
        {
            int series_id_1 = 0;
            std::cout << "Choose the series, that will be modified";
            //print_series_info();
            std::cin >> series_id_1;
    
            switch (series_id_1)
            {
            case series_id_t::exp_series_id:
                series1.reset(new exp_series<T, K>(x));
                break;
            case series_id_t::cos_series_id:
                series1.reset(new cos_series<T, K>(x));
                break;
            case series_id_t::sin_series_id:
                series1.reset(new sin_series<T, K>(x));
                break;
            case series_id_t::cosh_series_id:
                series1.reset(new cosh_series<T, K>(x));
                break;
            case series_id_t::sinh_series_id:
                series1.reset(new sinh_series<T, K>(x));
                break;
            case series_id_t::bin_series_id:
                T alpha;
                std::cout << "Enter the value for constant alpha for the series" << std::endl;
                std::cin >> alpha;
                series1.reset(new bin_series<T, K>(x, alpha));
                break;
            case series_id_t::four_arctan_series_id:
                series1.reset(new four_arctan_series<T, K>(x));
                break;
            case series_id_t::ln1mx_series_id:
                series1.reset(new ln1mx_series<T, K>(x));
                break;
            case series_id_t::mean_sinh_sin_series_id:
                series1.reset(new mean_sinh_sin_series<T, K>(x));
                break;
            case series_id_t::exp_squared_erf_series_id:
                series1.reset(new exp_squared_erf_series<T, K>(x));
                break;
            case series_id_t::xmb_Jb_two_series_id:
                K  b;
                std::cout << "Enter the value for constant b for the series" << std::endl;
                std::cin >> b;
                series1.reset(new xmb_Jb_two_series<T, K>(x, b));
                break; 
            case series_id_t::half_asin_two_x_series_id:
                series1.reset(new half_asin_two_x_series<T, K>(x));
                break;
            case series_id_t::inverse_1mx_series_id:
                series1.reset(new inverse_1mx_series<T, K>(x));
                break;
            case series_id_t::x_1mx_squared_series_id:
                series1.reset(new x_1mx_squared_series<T, K>(x));
                break;
            case series_id_t::erf_series_id:
                series1.reset(new erf_series<T, K>(x));
                break;
            case series_id_t::m_fact_1mx_mp1_inverse_series_id:
                K m;
                std::cout << "Enter the value for constant m for the series" << std::endl;
                std::cin >> m;
                series1.reset(new m_fact_1mx_mp1_inverse_series<T, K>(x, m));
                break; 
            case series_id_t::inverse_sqrt_1m4x_series_id:
                series1.reset(new inverse_sqrt_1m4x_series<T, K>(x));
                break;
            case series_id_t::one_twelfth_3x2_pi2_series_id:
                series1.reset(new one_twelfth_3x2_pi2_series<T, K>(x));
                break; 
            case series_id_t::x_twelfth_x2_pi2_series_id:
                series1.reset(new x_twelfth_x2_pi2_series<T, K>(x));
                break;
            case series_id_t::ln2_series_id:
                series1.reset(new ln2_series<T, K>());
                break;
            case series_id_t::one_series_id:
                series1.reset(new one_series<T, K>());
                break;
            case series_id_t::minus_one_quarter_series_id:
                series1.reset(new minus_one_quarter_series<T, K>());
                break;
            case series_id_t::pi_3_series_id:
                series1.reset(new pi_3_series<T, K>());
                break;
            case series_id_t::pi_4_series_id:
                series1.reset(new pi_4_series<T, K>());
                break;
            case series_id_t::pi_squared_6_minus_one_series_id:
                series1.reset(new pi_squared_6_minus_one_series<T, K>());
                break;
            case series_id_t::three_minus_pi_series_id:
                series1.reset(new three_minus_pi_series<T, K>());
                break;
            case series_id_t::one_twelfth_series_id:
                series1.reset(new one_twelfth_series<T, K>());
                break;
            case series_id_t::eighth_pi_m_one_third_series_id:
                series1.reset(new eighth_pi_m_one_third_series<T, K>());
                break;
            case series_id_t::one_third_pi_squared_m_nine_series_id:
                series1.reset(new one_third_pi_squared_m_nine_series<T, K>());
                break;
            case series_id_t::four_ln2_m_3_series_id:
                series1.reset(new four_ln2_m_3_series<T, K>());
                break;
            case series_id_t::exp_m_cos_x_sinsin_x_series_id:
                series1.reset(new exp_m_cos_x_sinsin_x_series<T, K>(x));
                break;
            case series_id_t::pi_four_minus_ln2_halfed_series_id:
                series1.reset(new pi_four_minus_ln2_halfed_series<T, K>(x));
                break;
            case series_id_t::five_pi_twelve_series_id:
                series1.reset(new five_pi_twelve_series<T, K>(x));
                break;
            case series_id_t::x_two_series_id:
                series1.reset(new x_two_series<T, K>(x));
                break;
            case series_id_t::pi_six_min_half_series_id:
                series1.reset(new pi_six_min_half_series<T, K>(x));
                break;
            case series_id_t::x_two_throught_squares_id:
                series1.reset(new x_two_throught_squares_series<T, K>(x));
                break;
            case series_id_t::minus_one_ned_in_n_series_id:
                series1.reset(new minus_one_ned_in_n_series<T, K>(x));
                break;
            case series_id_t::minus_one_n_fact_n_in_n_series_id:
                series1.reset(new minus_one_n_fact_n_in_n_series<T, K>(x));
                break;
            case series_id_t::ln_x_plus_one_x_minus_one_halfed_series_id:
                series1.reset(new ln_x_plus_one_x_minus_one_halfed_series<T, K>(x));
                break;
            case series_id_t::two_arcsin_square_x_halfed_series_id:
                series1.reset(new two_arcsin_square_x_halfed_series<T, K>(x));
                break;
            case series_id_t::pi_squared_twelve_series_id:
                series1.reset(new pi_squared_twelve_series<T, K>());
                break;
            case series_id_t::pi_cubed_32_series_id:
                series1.reset(new pi_cubed_32_series<T, K>());
                break; 
            case series_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id:
                series1.reset(new minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>());
                break; 
            case series_id_t::two_ln2_series_id:
                series1.reset(new two_ln2_series<T, K>());
                break; 
            case series_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id:
                series1.reset(new pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>(x));
                break; 
            case series_id_t::pi_minus_x_2_id:
                series1.reset(new pi_minus_x_2<T, K>(x));
                break; 
            case series_id_t::half_multi_ln_1div2multi1minuscosx_id:
                series1.reset(new half_multi_ln_1div2multi1minuscosx<T, K>(x));
                break; 
            case series_id_t::half_minus_sinx_multi_pi_4_id:
                series1.reset(new half_minus_sinx_multi_pi_4<T, K>(x));
                break; 
            case series_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_id:
                series1.reset(new ln_1plussqrt1plusxsquare_minus_ln_2<T, K>(x));
                break; 
            case series_id_t::ln_cosx_id:
                series1.reset(new ln_cosx<T, K>(x));
                break; 
            case series_id_t::ln_sinx_minus_ln_x_id:
                series1.reset(new ln_sinx_minus_ln_x<T, K>(x));
                break; 
            case series_id_t::pi_8_cosx_square_minus_1_div_3_cosx_id:
                series1.reset(new pi_8_cosx_square_minus_1_div_3_cosx<T, K>(x));
                break; 
            case series_id_t::sqrt_oneminussqrtoneminusx_div_x_id:
                series1.reset(new sqrt_oneminussqrtoneminusx_div_x<T, K>(x));
                break; 
            case series_id_t::one_minus_sqrt_1minus4x_div_2x_id:
                series1.reset(new one_minus_sqrt_1minus4x_div_2x<T, K>(x));
                break; 
            case series_id_t::arcsin_x_minus_x_series_id:
                series1.reset(new arcsin_x_minus_x_series<T, K>(x));
                break; 
            case series_id_t::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id:
                series1.reset(new pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>(x));
                break; 
            case series_id_t::abs_sin_x_minus_2_div_pi_series_id:
                series1.reset(new abs_sin_x_minus_2_div_pi_series<T, K>(x));
                break; 
            case series_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id:
                series1.reset(new pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>(x));
                break; 
            case series_id_t::minus_3_div_4_or_x_minus_3_div_4_series_id:
                series1.reset(new minus_3_div_4_or_x_minus_3_div_4_series<T, K>(x));
                break; 
            case series_id_t::ten_minus_x_series_id:
                series1.reset(new ten_minus_x_series<T, K>(x));
                break; 
            case series_id_t::x_series_id:
                series1.reset(new x_series<T, K>(x));
                break; 
            case series_id_t::minus_x_minus_pi_4_or_minus_pi_4_series_id:
                series1.reset(new minus_x_minus_pi_4_or_minus_pi_4_series<T, K>(x));
                break; 
            case series_id_t::one_div_two_minus_x_multi_three_plus_x_series_id:
                series1.reset(new one_div_two_minus_x_multi_three_plus_x_series<T, K>(x));
                break; 
            case series_id_t::Si_x_series_id:
                series1.reset(new Si_x_series<T, K>(x));
                break; 
            case series_id_t::Ci_x_series_id:
                series1.reset(new Ci_x_series<T, K>(x));
                break;
            case series_id_t::Riemann_zeta_func_series_id:
                series1.reset(new Riemann_zeta_func_series<T, K>(x));
                break; 
            case series_id_t::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id:
                series1.reset(new Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>(x));
                break; 
            case series_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_series_id:
                series1.reset(new xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>(x));
                break; 
            case series_id_t::arcsin_x_series_id:
                series1.reset(new arcsin_x_series<T, K>(x));
                break; 
            case series_id_t::arctg_x_series_id:
                series1.reset(new arctg_x_series<T, K>(x));
                break; 
            case series_id_t::K_x_series_id:
                series1.reset(new K_x_series<T, K>(x));
                break; 
            case series_id_t::E_x_series_id:
                series1.reset(new E_x_series<T, K>(x));
                break; 
            case series_id_t::sqrt_1plusx_series_id:
                series1.reset(new sqrt_1plusx_series<T, K>(x));
                break; 
            case series_id_t::Lambert_W_func_series_id:
                series1.reset(new Lambert_W_func_series<T, K>(x));
                break; 
            case series_id_t::Incomplete_Gamma_func_series_id:
                T s;
                std::cout << "Enter the value for constant s for the series" << std::endl;
                std::cin >> s;
                series1.reset(new Incomplete_Gamma_func_series<T, K>(x, s));
                break; 
            case series_id_t::Series_with_ln_number1_id:
                series1.reset(new Series_with_ln_number1<T, K>());
                break; 
            case series_id_t::Series_with_ln_number2_id:
                series1.reset(new Series_with_ln_number2<T, K>());
                break;
            case series_id_t::pi_series_id:
                series1.reset(new pi_series<T, K>());
                break; 
            case series_id_t::x_min_sqrt_x_series_id:
                series1.reset(new x_min_sqrt_x_series<T, K>(x));
                break; 
            case series_id_t::arctan_x2_series_id:
                series1.reset(new arctan_x2_series<T, K>(x));
                break;
            case series_id_t::ln1px4_series_id:
                series1.reset(new ln1px4_series<T, K>(x));
                break;
            case series_id_t::sin_x2_series_id:
                series1.reset(new sin_x2_series<T, K>(x));
                break;
            case series_id_t::arctan_x3_series_id:
                series1.reset(new arctan_x3_series<T, K>(x));
                break;
            case series_id_t::arcsin_x2_series_id:
                series1.reset(new arcsin_x2_series<T, K>(x));
                break;
            case series_id_t::ln1_m_x2_series_id:
                series1.reset(new ln1_m_x2_series<T, K>(x));
                break;
            case series_id_t::artanh_x_series_id:
                series1.reset(new artanh_x_series<T, K>(x));
                break;
            case series_id_t::arcsinh_x_series_id:
                series1.reset(new arcsinh_x_series<T, K>(x));
                break;
            case series_id_t::cos_x2_series_id:
                series1.reset(new cos_x2_series<T, K>(x));
                break;
            case series_id_t::sinh_x2_series_id:
                series1.reset(new sinh_x2_series<T, K>(x));
                break;
            case series_id_t::arctanh_x2_series_id:
                series1.reset(new arctanh_x2_series<T, K>(x));
                break;
            case series_id_t::cos3xmin1_div_xsqare_series_id:
                series1.reset(new cos3xmin1_div_xsqare_series<T, K>(x));
                break; 
            case series_id_t::two_degree_x_series_id:
                series1.reset(new two_degree_x_series<T, K>(x));
                break; 
            case series_id_t::sqrt_1plusx_min_1_min_x_div_2_series_id:
                series1.reset(new sqrt_1plusx_min_1_min_x_div_2_series<T, K>(x));
                break; 
            case series_id_t::ln13_min_ln7_div_7_series_id:
                series1.reset(new ln13_min_ln7_div_7_series<T, K>());
                break; 
            case series_id_t::Ja_x_series_id:
                T a;
                std::cout << "Enter the value for constant a for the series" << std::endl;
                std::cin >> a;
                series1.reset(new Ja_x_series<T, K>(x, a));
                break; 
            case series_id_t::one_div_sqrt2_sin_xdivsqrt2_series_id:
                series1.reset(new one_div_sqrt2_sin_xdivsqrt2_series<T, K>(x));
                break; 
            case series_id_t::ln_1plusx_div_1plusx2_id:
                series1.reset(new ln_1plusx_div_1plusx2<T, K>(x));
                break;
            case series_id_t::cos_sqrt_x_id:
                series1.reset(new cos_sqrt_x<T, K>(x));
                break;
            case series_id_t::ln_1_plus_x3_id:
                series1.reset(new ln_1_plus_x3<T, K>(x));
                break;
            case series_id_t::x_div_1minx_id:
                series1.reset(new x_div_1minx<T, K>(x));
                break;
            case series_id_t::x_div_1minx2_id:
                series1.reset(new x_div_1minx2<T, K>(x));
                break;
            case series_id_t::gamma_series_id:
                T t;
                std::cout << "Enter the parameter t in the gamma series" << std::endl;
                std::cin >> t;
                series1.reset(new gamma_series<T, K>(t, x));
                break;
            default:
                throw std::domain_error("wrong series_id");
            }
            series.reset(new ModifiedSeries<T, K, decltype(series1.get())>(series1.get(), get_cached_random));
            break;
        }
        default:
            throw std::domain_error("wrong series_id");
        }
    } else {
        T item;
        std::cin >> count_arr;
        for(int i = 0; i <= count_arr; i++){
            std::cin >> item;
            arr.push_back(item);
        }
    }

    if (series_id == -1){
        series.reset(new ArraySeries<T, K>(arr));
    }

    //choosing transformation
    //print_transformation_info();
    int transformation_id = 0;
    std::cin >> transformation_id;
    std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform;
    switch (transformation_id)
    {
    case transformation_id_t::shanks_transformation_id:
        if (alternating_series.contains(series_id))
            transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
        else
            transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::epsilon_algorithm_id:
        transform.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::levin_algorithm_id:
        transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::epsilon_algorithm_2_id:
        transform.reset(new epsilon_algorithm_two<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::S_algorithm:
        init_levin(transformation_id_t::S_algorithm, series, transform);
        break;
    case transformation_id_t::D_algorithm:
        init_levin(transformation_id_t::D_algorithm, series, transform);
        break;
    case transformation_id_t::chang_epsilon_algorithm:
        transform.reset(new chang_whynn_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::M_algorithm:
        init_levin(transformation_id_t::M_algorithm, series, transform);
        break;
    case transformation_id_t::weniger_transformation:
        transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::rho_wynn_transformation_id:
        init_wynn(series, transform);
        break;
    case transformation_id_t::brezinski_theta_transformation_id:
        transform.reset(new theta_brezinski_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::epsilon_algorithm_3_id:
        transform.reset(new epsilon_algorithm_three<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::levin_recursion_id:
        init_levin_recursion(series, transform);
        break;
    case transformation_id_t::W_algorithm_id:
        transform.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::richardson_algorithm_id:
        transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::Ford_Sidi_algorithm_id:
        transform.reset(new ford_sidi_algorithm<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::Ford_Sidi_algorithm_two_id:
        transform.reset(new ford_sidi_algorithm_two<T, K, decltype(series.get())>(series.get()));
        break;
    case transformation_id_t::Ford_Sidi_algorithm_three_id:
        transform.reset(new ford_sidi_algorithm_three<T, K, decltype(series.get())>(series.get(), 0));
        break;
    default:
        throw std::domain_error("wrong transformation_id");
    }

    //choosing testing function
    //print_test_function_info();
    int function_id = 0;
    std::cin >> function_id;
    int n = 0;
    int order = 0;
    if(series_id == -1){
        std::cin >> order;
        n = count_arr;
    }else {
        std::cout << "Enter n and order:" << std::endl;
        std::cin >> n >> order;
    }

    if(dynamic_cast<ford_sidi_algorithm_three<T, K, decltype(series.get())>*>(transform.get())) {
        transform.reset(new ford_sidi_algorithm_three<T, K, decltype(series.get())>(series.get(), n));
    }
    
    switch (function_id)
	{
	case test_function_id_t::cmp_sum_and_transform_id:
		cmp_sum_and_transform_f(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::cmp_a_n_and_transform_id:
		cmp_a_n_and_transform_f(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::transformation_remainder_id:
		transformation_remainders_f(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::cmp_transformations_id:
	{
		int cmop_transformation_id = 0;
		//print_transformation_info();
		std::cin >> cmop_transformation_id;

		std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform2;
		
		switch (cmop_transformation_id){
		case transformation_id_t::shanks_transformation_id:
			if (alternating_series.contains(series_id))
				transform2.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform2.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_id:
			transform2.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::levin_algorithm_id:
			transform2.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_2_id:
			transform2.reset(new epsilon_algorithm_two<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::S_algorithm:
			init_levin(transformation_id_t::S_algorithm, series, transform2);
			break;
		case transformation_id_t::D_algorithm:
			init_levin(transformation_id_t::D_algorithm, series, transform2);
			break;
		case transformation_id_t::chang_epsilon_algorithm:
			transform2.reset(new chang_whynn_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::M_algorithm:
			init_levin(transformation_id_t::M_algorithm, series, transform2);
			break;
		case transformation_id_t::weniger_transformation:
			transform2.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::rho_wynn_transformation_id:
			init_wynn(series, transform2);
			break;
		case transformation_id_t::brezinski_theta_transformation_id:
			transform2.reset(new theta_brezinski_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_3_id:
			transform2.reset(new epsilon_algorithm_three<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::levin_recursion_id:
			init_levin_recursion(series, transform);
			break;
		case transformation_id_t::W_algorithm_id:
			transform2.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::richardson_algorithm_id:
			transform2.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::Ford_Sidi_algorithm_id:
			transform2.reset(new ford_sidi_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::Ford_Sidi_algorithm_two_id:
			transform2.reset(new ford_sidi_algorithm_two<T, K, decltype(series.get())>(series.get()));
		case transformation_id_t::Ford_Sidi_algorithm_three_id:
			transform2.reset(new ford_sidi_algorithm_three<T, K, decltype(series.get())>(series.get(), n));
		default:
			throw std::domain_error("wrong algorithm id");
		}

		cmp_transformations_f(n, order, std::move(series.get()), std::move(transform.get()), std::move(transform2.get()));
	}
	case test_function_id_t::eval_transform_time_id:
		eval_transform_time_f(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::test_all_transforms_id:{ //Testing all functions for series
        std::ofstream f("output.txt");
        if (!f.is_open()) {
            throw std::runtime_error("Failed to open output file");
        }    
        std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform2;
        transform2.reset(new ford_sidi_algorithm_three<T, K, decltype(series.get())>(series.get(), n));

        f << std::fixed << std::setprecision(18);
        f.unsetf(std::ios::scientific);    
		for (int i = 1; i <= n; ++i) 
		{
			print_sum_f(i, std::move(series.get()), f);
			
			//shanks
			if (alternating_series.contains(series_id))
				transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//epsilon v-1
			transform.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//epsilon v-2
			transform.reset(new epsilon_algorithm_two<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//epsilon v-3
			transform.reset(new epsilon_algorithm_three<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new rho_transform<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new generilized_transform<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new gamma_rho_transform<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);

			//theta-brezinski
			transform.reset(new theta_brezinski_algorithm<T, K, decltype(series.get())>(series.get()));
            print_transform_f(i, order, std::move(transform.get()), f);


			//chang epsilon wynn
			transform.reset(new chang_whynn_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//levin standart
			transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//levin recurcive
			transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//levin-sidi S U
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);

			//levin-sidi S T
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi S D
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi S V
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);

			//levin-sidi D U
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi D T
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi D D
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi D V
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform<T, K>{}, false));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi M U
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi M T
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi M D
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);
			//

			//levin-sidi M V
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform_2<T, K>{}));
			print_transform_f(i, order, std::move(transform.get()), f);
			//
			
			//weniger
			transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//lubkin W
			transform.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);
			
			//Richardson
			transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);

			//Ford-Sidi
			transform.reset(new ford_sidi_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);


			//Ford-Sidi v-2
			transform.reset(new ford_sidi_algorithm_two<T, K, decltype(series.get())>(series.get()));
			print_transform_f(i, order, std::move(transform.get()), f);
			
			//Ford-Sidi V3
			print_transform_f(i, order, std::move(transform2.get()), f);

		}

		break;
    }
	default:
		throw std::domain_error("wrong function_id");
	}
}


int main(void)
{
	try
	{
		main_testing_function<long double, long long int>();
	}
	catch (std::domain_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (std::overflow_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}