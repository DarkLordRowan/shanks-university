# Catalogue coverage

See [mathematical audit](mathematical-audit.md) for interpretation and reproduction.

`ported` means an independent Python mathematical oracle exists. `pending` is not a defect verdict.
`sample_agrees` compares a finite double sum or sequence sample with its own C++ reference at one point.
`reference_unavailable` includes divergent ordinary sums and exact Rump recurrences with poles.

109 C++ catalogue entries have Python counterparts; Python also adds the base log1p series.
0 C++ entries remain pending for independent verification and porting.

| C++ series | Python status | Probe N=256 | Relative residual |
|---|---|---|---|
| `abs_sin_x_minus_2_div_pi` | ported | review_residual | 2.05e-05 |
| `arcsin_x2` | ported | sample_agrees | 4.34e-19 |
| `arcsin_x` | ported | sample_agrees | 6.94e-18 |
| `arcsin_x_minus_x` | ported | sample_agrees | 1.81e-18 |
| `arcsinh_x` | ported | sample_agrees | 0 |
| `arctan_x2` | ported | sample_agrees | 4.34e-19 |
| `arctan_x3` | ported | sample_agrees | 0 |
| `arctan_x` | ported | sample_agrees | 0 |
| `arctanh_x2` | ported | sample_agrees | 0 |
| `arctanh_x` | ported | sample_agrees | 0 |
| `bessel_first_kind` | ported | sample_agrees | 0 |
| `bin` | ported | sample_agrees | 0 |
| `ci_x` | ported | sample_agrees | 1.84e-16 |
| `cos3xmin1_div_xsquare` | ported | sample_agrees | 2.15e-16 |
| `cos` | ported | sample_agrees | 0 |
| `cos_sqrt_x` | ported | sample_agrees | 1.11e-16 |
| `cos_x2` | ported | sample_agrees | 0 |
| `cosh` | ported | sample_agrees | 2.22e-16 |
| `e_x` | ported | sample_agrees | 1.41e-16 |
| `eighth_pi_m_one_third` | ported | sample_agrees | 1.84e-10 |
| `erf` | ported | sample_agrees | 6.94e-18 |
| `euler_mascheroni` | ported | review_residual | 9.73e-05 |
| `exp` | ported | sample_agrees | 2.11e-16 |
| `exp_m_cos_x_sinsin_x` | ported | sample_agrees | 6.94e-18 |
| `exp_squared_erf` | ported | sample_agrees | 0 |
| `five_pi_twelve` | ported | review_residual | 8.14e-05 |
| `four_arctan` | ported | sample_agrees | 0 |
| `four_ln2_m_3` | ported | sample_agrees | 5.73e-12 |
| `half_asin_two_x` | ported | sample_agrees | 0 |
| `half_minus_sinx_multi_pi_4` | ported | review_residual | 1.61e-05 |
| `half_multi_ln_1div2multi1minuscosx` | ported | review_residual | 0.00468 |
| `incomplete_Gamma_func` | ported | sample_agrees | 2.22e-16 |
| `integral1` | ported | sample_agrees | 5.71e-08 |
| `inverse_1mx` | ported | sample_agrees | 0 |
| `inverse_sqrt_1m4x` | ported | sample_agrees | 0 |
| `k_x` | ported | sample_agrees | 1.41e-16 |
| `lambert_W_func` | ported | sample_agrees | 0 |
| `legendre1` | ported | sample_agrees | 1.18e-07 |
| `ln13_min_ln7_div_7` | ported | sample_agrees | 8.67e-19 |
| `ln1_m_x2` | ported | sample_agrees | 5.38e-17 |
| `ln1mx` | ported | sample_agrees | 4.16e-17 |
| `ln1px4` | ported | sample_agrees | 1.46e-17 |
| `ln2` | ported | review_residual | 9.75e-05 |
| `ln_1_plus_x3` | ported | sample_agrees | 6.93e-17 |
| `ln_1plussqrt1plusxsquare_minus_ln_2` | ported | sample_agrees | 1.1e-16 |
| `ln_1plusx_div_1plusx2` | ported | sample_agrees | 4.86e-17 |
| `ln_cosx` | ported | sample_agrees | 3.62e-17 |
| `ln_sinx_minus_ln_x` | ported | sample_agrees | 9.88e-07 |
| `ln_x_plus_one_x_minus_one_halfed` | ported | sample_agrees | 3.62e-17 |
| `m_fact_1mx_mp1_inverse` | ported | sample_agrees | 1.24e-15 |
| `mean_sinh_sin` | ported | sample_agrees | 0 |
| `minus_3_div_4_or_x_minus_3_div_4` | ported | review_residual | 0.00144 |
| `minus_one_n_fact_n_in_n` | ported | sample_agrees | 2.08e-17 |
| `minus_one_ned_in_n` | ported | sample_agrees | 0 |
| `minus_one_quarter` | ported | sample_agrees | 3.77e-07 |
| `minus_three_plus_ln3_three_devided_two_plus_two_ln2` | ported | sample_agrees | 1.06e-08 |
| `minus_x_minus_pi_4_or_minus_pi_4` | ported | review_residual | 0.000413 |
| `natural_series` | ported | reference_unavailable | n/a |
| `one_div_sqrt2_sin_xdivsqrt2` | ported | sample_agrees | 6.94e-18 |
| `one_div_two_minus_x_multi_three_plus_x` | ported | sample_agrees | 2.78e-17 |
| `one` | ported | review_residual | 0.000195 |
| `one_minus_sqrt_1minus4x_div_2x` | ported | sample_agrees | 4.21e-16 |
| `one_third_pi_squared_m_nine` | ported | sample_agrees | 9.82e-10 |
| `one_twelfth_3x2_pi2` | ported | review_residual | 7.35e-06 |
| `one_twelfth` | ported | sample_agrees | 4.73e-08 |
| `one_twelfth_x2_pi2` | ported | sample_agrees | 7.58e-09 |
| `pi_3` | ported | sample_agrees | 4.77e-08 |
| `pi_4` | ported | review_residual | 4.88e-05 |
| `pi_8_cosx_square_minus_1_div_3_cosx` | ported | sample_agrees | 3.11e-09 |
| `pi_cubed_32` | ported | sample_agrees | 1.86e-10 |
| `pi_four_minus_ln2_halfed` | ported | sample_agrees | 9.52e-08 |
| `pi` | ported | sample_agrees | 2.78e-17 |
| `pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4` | ported | review_residual | 0.000507 |
| `pi_minus_x_2` | ported | review_residual | 0.0492 |
| `pi_six_min_half` | ported | sample_agrees | 1.06e-08 |
| `pi_squared_6_minus_one` | ported | sample_agrees | 3.79e-07 |
| `pi_squared_twelve` | ported | sample_agrees | 3.8e-07 |
| `pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square` | ported | sample_agrees | 1.76e-07 |
| `pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi` | ported | review_residual | 1.95e-05 |
| `riemann_zeta_func` | ported | review_residual | 6.32e-06 |
| `riemann_zeta_func_xmin1_div_Riemann_zeta_func_x` | ported | review_residual | 0.00173 |
| `rump_seq1` | ported | reference_unavailable | n/a |
| `rump_seq2` | ported | reference_unavailable | n/a |
| `rump_seq3` | ported | review_residual | 15.7 |
| `rump_seq4` | ported | reference_unavailable | n/a |
| `rump_seq5` | ported | review_residual | 19 |
| `rump_seq6` | ported | review_residual | 0.164 |
| `rump_seq7` | ported | sample_agrees | 0 |
| `si_x` | ported | sample_agrees | 0 |
| `sin` | ported | sample_agrees | 6.94e-18 |
| `sin_x2` | ported | sample_agrees | 0 |
| `sinh` | ported | sample_agrees | 0 |
| `sinh_x2` | ported | sample_agrees | 4.34e-19 |
| `sqrt_1plusx` | ported | sample_agrees | 0 |
| `sqrt_1plusx_min_1_min_x_div_2` | ported | sample_agrees | 9.1e-17 |
| `sqrt_oneminussqrtoneminusx_div_x` | ported | sample_agrees | 8.88e-16 |
| `ten_minus_x` | ported | sample_agrees | 4.49e-15 |
| `three_minus_pi` | ported | sample_agrees | 7.36e-10 |
| `two_arcsin_square_x_halfed` | ported | sample_agrees | 2.17e-19 |
| `two_degree_x` | ported | sample_agrees | 0 |
| `two_ln2` | ported | sample_agrees | 2.85e-07 |
| `x_1mx_squared` | ported | sample_agrees | 0 |
| `x_div_1minx2` | ported | sample_agrees | 0 |
| `x_div_1minx` | ported | sample_agrees | 6.94e-18 |
| `x` | ported | review_residual | 0.000998 |
| `x_min_sqrt_x` | ported | sample_agrees | 2.48e-09 |
| `x_two` | ported | review_residual | 4.87e-05 |
| `x_two_throught_squares` | ported | review_residual | 9.75e-05 |
| `xsquareplus3_div_xsquareplus2multix_minus_1` | ported | review_residual | 2.1e-06 |

## Algorithm families

Full C++ variants (remainder estimates, recursive forms, numerators) are not interchangeable.
Python Levin/Weniger/Drummond support next, t, u, v, v_shift remainders and beta>0. C++ Levin and Sidi-S direct/recursive forms are checked against those Python formulas.

23/23 header families have audited Python counterparts for the documented variants.
Additional parameterized modes and specialized W variants are not all covered.

| C++ header | Python status |
|---|---|
| `anderson_acceleration_algorithm.hpp` | anderson (scalar observed pairs; Type-II ridge/minimum-norm LS) |
| `brezinski_theta_algorithm.hpp` | brezinski_theta |
| `chang_wynn_algorithm.hpp` | chang_wynn (Eq. 3.20; 2k+2 samples; n=start, order=k) |
| `drummond_d_algorithm.hpp` | drummond (five remainders; C++ direct/recursive checked) |
| `f_algorithm.hpp` | homeier_f (increasing real power grid; independent divided differences) |
| `ford_sidi_2_algorithm.hpp` | aitken_backtracking (legacy misnomer; not general Ford-Sidi) |
| `ford_sidi_3_algorithm.hpp` | ford_sidi (recursive elimination for Levin basis; n=start, order=k) |
| `h_algorithm.hpp` | homeier_h (2k+1 window; independent polynomial oracle) |
| `j_transformation_algorithm.hpp` | homeier_j (general Delta/delta; constant/pJ/power choices) |
| `levin_algorithm.hpp` | levin (five remainders, beta>0; C++ direct/recursive checked) |
| `levin_sidi_m_algorithm.hpp` | levin_sidi_m (five remainders, gamma>=k-1; independent model fit) |
| `levin_sidi_s_algorithm.hpp` | Python weniger is the S factorial transform (C++ direct/recursive checked) |
| `lubkin_w_algorithm.hpp` | lubkin_w |
| `overholt_algorithm.hpp` | overholt |
| `pj_algorithm.hpp` | homeier_pj (positive integer p; independent generalized differences) |
| `richardson_algorithm.hpp` | richardson (dyadic error model) |
| `shanks_algorithm.hpp` | shanks; aitken_delta2 is order 1 |
| `shanks_alternating_algorithm.hpp` | iterated_aitken (legacy C++ name; corrected forward window) |
| `weniger_algorithm.hpp` | weniger (Python general remainders; C++ fixed beta=1,next checked) |
| `wynn_epsilon_1_algorithm.hpp` | wynn_epsilon |
| `wynn_epsilon_2_algorithm.hpp` | wynn_epsilon_fixed (canonical epsilon with moving start) |
| `wynn_epsilon_3_algorithm.hpp` | wynn_epsilon_adaptive (QELG-based; 50-cell compaction; restart/refill policy) |
| `wynn_rho_algorithm.hpp` | wynn_rho (classic nodes x_n=n+1; C++ parameterized numerators pending) |
