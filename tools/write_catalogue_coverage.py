"""Generate the coverage table from a saved exploratory catalogue probe.

The independently verified entries are an explicit reviewed list. Probe agreement
alone must never promote an entry to verified/ported.
"""
import json
from pathlib import Path
import sys

ROOT=Path(__file__).resolve().parents[1]
VERIFIED={
    "exp","sin","cos","sinh","cosh","ln1mx","arctan_x","arctanh_x",
    "arcsin_x","arcsinh_x","sqrt_1plusx","inverse_sqrt_1m4x",
    "one_minus_sqrt_1minus4x_div_2x","x_div_1minx","x_div_1minx2",
    "x_1mx_squared","inverse_1mx","sin_x2","cos_x2","sinh_x2",
    "arcsin_x2","arctanh_x2","arctan_x2","arctan_x3","ln1px4",
    "ln_1_plus_x3","ln1_m_x2","sqrt_1plusx_min_1_min_x_div_2","riemann_zeta_func",
}

VERIFIED.update({
    "bin", "m_fact_1mx_mp1_inverse", "four_arctan", "arcsin_x_minus_x",
    "half_asin_two_x", "two_arcsin_square_x_halfed", "cos_sqrt_x",
    "one_div_sqrt2_sin_xdivsqrt2", "mean_sinh_sin", "two_degree_x",
    "ln_1plussqrt1plusxsquare_minus_ln_2", "sqrt_oneminussqrtoneminusx_div_x",
    "erf", "exp_squared_erf", "si_x", "ci_x", "ln2", "two_ln2", "euler_mascheroni",
})

VERIFIED.update({
    'abs_sin_x_minus_2_div_pi',
    'bessel_first_kind',
    'cos3xmin1_div_xsquare',
    'e_x',
    'eighth_pi_m_one_third',
    'exp_m_cos_x_sinsin_x',
    'five_pi_twelve',
    'four_ln2_m_3',
    'half_minus_sinx_multi_pi_4',
    'half_multi_ln_1div2multi1minuscosx',
    'incomplete_Gamma_func',
    'k_x',
    'lambert_W_func',
    'legendre1',
    'ln13_min_ln7_div_7',
    'ln_1plusx_div_1plusx2',
    'ln_cosx',
    'ln_sinx_minus_ln_x',
    'ln_x_plus_one_x_minus_one_halfed',
    'minus_3_div_4_or_x_minus_3_div_4',
    'minus_one_quarter',
    'minus_three_plus_ln3_three_devided_two_plus_two_ln2',
    'minus_x_minus_pi_4_or_minus_pi_4',
    'one',
    'one_div_two_minus_x_multi_three_plus_x',
    'one_third_pi_squared_m_nine',
    'one_twelfth',
    'one_twelfth_3x2_pi2',
    'one_twelfth_x2_pi2',
    'pi',
    'pi_3',
    'pi_4',
    'pi_8_cosx_square_minus_1_div_3_cosx',
    'pi_cubed_32',
    'pi_four_minus_ln2_halfed',
    'pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4',
    'pi_minus_x_2',
    'pi_six_min_half',
    'pi_squared_6_minus_one',
    'pi_squared_twelve',
    'pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square',
    'pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi',
    'riemann_zeta_func_xmin1_div_Riemann_zeta_func_x',
    'ten_minus_x',
    'three_minus_pi',
    'x',
    'x_min_sqrt_x',
    'x_two',
    'x_two_throught_squares',
    'xsquareplus3_div_xsquareplus2multix_minus_1',
})

VERIFIED.update({f'rump_seq{i}' for i in range(1,8)} | {
    'natural_series','integral1','minus_one_n_fact_n_in_n','minus_one_ned_in_n'})

def write_table(report):
    rows=report['series']
    lines=[
        '# Catalogue coverage','',
        'See [mathematical audit](mathematical-audit.md) for interpretation and reproduction.','',
        '`ported` means an independent Python mathematical oracle exists. `pending` is not a defect verdict.',
        '`sample_agrees` compares a finite double sum or sequence sample with its own C++ reference at one point.',
        '`reference_unavailable` includes divergent ordinary sums and exact Rump recurrences with poles.','',
        f'{len(VERIFIED)} C++ catalogue entries have Python counterparts; Python also adds the base log1p series.',
        f'{len(rows)-len(VERIFIED)} C++ entries remain pending for independent verification and porting.','',
        '| C++ series | Python status | Probe N=256 | Relative residual |',
        '|---|---|---|---|',
    ]
    for row in rows:
        name=row['name']
        status='ported' if name in VERIFIED else 'pending'
        residual=row.get('relative_residual')
        residual=f'{residual:.3g}' if residual is not None else 'n/a'
        lines.append(f'| `{name}` | {status} | {row["status"]} | {residual} |')
    lines += ['', '## Algorithm families','',
              'Full C++ variants (remainder estimates, recursive forms, numerators) are not interchangeable.',
              'Python Levin/Weniger/Drummond support next, t, u, v, v_shift remainders and beta>0. C++ Levin and Sidi-S direct/recursive forms are checked against those Python formulas.','',
              '| C++ header | Python status |', '|---|---|']
    ported={'shanks_algorithm':'shanks; aitken_delta2 is order 1',
            'wynn_epsilon_1_algorithm':'wynn_epsilon',
            'richardson_algorithm':'richardson (dyadic error model)',
            'levin_algorithm':'levin (five remainders, beta>0; C++ direct/recursive checked)',
            'weniger_algorithm':'weniger (Python general remainders; C++ fixed beta=1,next checked)',
            'wynn_rho_algorithm':'wynn_rho (classic nodes x_n=n+1; C++ parameterized numerators pending)',
            'brezinski_theta_algorithm':'brezinski_theta',
             'drummond_d_algorithm':'drummond (five remainders; C++ direct/recursive checked)',
            'levin_sidi_s_algorithm':'Python weniger is the S factorial transform (C++ direct/recursive checked)',
            'overholt_algorithm':'overholt',
            'lubkin_w_algorithm':'lubkin_w',
            'levin_sidi_m_algorithm':'levin_sidi_m (five remainders, gamma>=k-1; independent model fit)',
            'wynn_epsilon_2_algorithm':'wynn_epsilon_fixed (canonical epsilon with moving start)',
            'shanks_alternating_algorithm':'iterated_aitken (legacy C++ name; corrected forward window)',
            'f_algorithm':'homeier_f (increasing real power grid; independent divided differences)',
            'h_algorithm':'homeier_h (2k+1 window; independent polynomial oracle)',
            'pj_algorithm':'homeier_pj (positive integer p; independent generalized differences)',
            'anderson_acceleration_algorithm':'anderson (scalar observed pairs; Type-II ridge/minimum-norm LS)',
            'ford_sidi_2_algorithm':'aitken_backtracking (legacy misnomer; not general Ford-Sidi)',
            'ford_sidi_3_algorithm':'ford_sidi (recursive elimination for Levin basis; n=start, order=k)',
            'chang_wynn_algorithm':'chang_wynn (Eq. 3.20; 2k+2 samples; n=start, order=k)',
            'j_transformation_algorithm':'homeier_j (general Delta/delta; constant/pJ/power choices)',
            'wynn_epsilon_3_algorithm':'wynn_epsilon_adaptive (QELG-based; 50-cell compaction; restart/refill policy)'  }
    headers=sorted((ROOT/'backend/core/include/transformations').glob('*_algorithm.hpp'))
    lines[-2:-2]=[f'{sum(path.stem in ported for path in headers)}/{len(headers)} header families have audited Python counterparts for the documented variants.',
                 'Additional parameterized modes and specialized W variants are not all covered.','']
    for path in headers:
        lines.append(f'| `{path.name}` | {ported.get(path.stem,"pending")} |')
    (ROOT/'docs/catalogue-coverage.md').write_text('\n'.join(lines)+'\n')


if __name__=='__main__':
    source=Path(sys.argv[1]) if len(sys.argv)>1 else ROOT/'docs/catalogue-sample.json'
    write_table(json.loads(source.read_text()))
