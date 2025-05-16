from test import *


def get_series_limits(series_num):
    """Возвращает ограничения для ряда по его номеру"""
    limits = {
        1: "x ∈ ℝ",
        2: "x ∈ ℝ",
        3: "x ∈ ℝ",
        4: "x ∈ ℝ",
        5: "x ∈ ℝ",
        6: "|x| < 1",
        7: "|x| ≤ 1",
        8: "|x| < 1, x ≠ 1",
        9: "x ∈ ℝ",
        10: "x ∈ ℝ",
        11: "x ∈ ℝ, b ∈ ℤ",
        12: "|x| ≤ 0.5",
        13: "|x| < 1",
        14: "|x| < 1, x ≠ 1",
        15: "x ∈ ℝ",
        16: "|x| < 1, m ∈ ℤ",
        17: "|x| ≤ 0.25",
        18: "|x| ≤ π",
        19: "|x| ≤ π",
        20: "x = 0 (числовой ряд)",
        21: "x = 0 (числовой ряд)",
        22: "x = 0 (числовой ряд)",
        23: "x = 0 (числовой ряд)",
        24: "x = 0 (числовой ряд)",
        25: "x = 0 (числовой ряд)",
        26: "x = 0 (числовой ряд)",
        27: "x = 0 (числовой ряд)",
        28: "x = 0 (числовой ряд)",
        29: "x = 0 (числовой ряд)",
        30: "x = 0 (числовой ряд)",
        31: "x ∈ ℝ",
        32: "x ∈ ℝ",
        33: "x ∈ ℝ",
        34: "x ∈ ℝ",
        35: "x ∈ ℝ",
        36: "x ∈ ℝ",
        37: "x ∈ ℝ",
        38: "x ∈ ℝ",
        39: "|x| < 1",
        40: "|x| ≤ 2",
        41: "x = 0 (числовой ряд)",
        42: "x = 0 (числовой ряд)",
        43: "x = 0 (числовой ряд)",
        44: "x = 0 (числовой ряд)",
        45: "x ≠ 0",
        46: "0 < x < 2π",
        47: "0 < x < 2π",
        48: "x ∈ ℝ",
        49: "x² ≤ 1",
        50: "|x| < π/2",
        51: "0 < x ≤ π",
        52: "|x| ≤ π/2",
        53: "|x| < 1, x ≠ 0",
        54: "|x| ≤ 0.25, x ≠ 0",
        55: "|x| ≤ 1",
        56: "0 < x < 2π",
        57: "0 ≤ x ≤ 2π",
        58: "-π < x ≤ π",
        59: "-3 < x < 3",
        60: "5 < x < 15",
        61: "-π ≤ x ≤ π",
        62: "-π < x ≤ π",
        63: "|x-1| > 3",
        64: "x ∈ ℝ",
        65: "x > 0",
        66: "x > 1",
        67: "x > 2",
        68: "|x-1| > 3",
        69: "|x| ≤ 1",
        70: "|x| ≤ 1",
        71: "x ∈ ℝ",
        72: "x ∈ ℝ",
        73: "x ≥ -1",
        74: "|x| < 1/e",
        75: "x > 0, s > 0",
        76: "x = 0 (числовой ряд)",
        77: "x = 0 (числовой ряд)",
        78: "x = 0 (числовой ряд)",
        79: "x ≥ 0",
        80: "x ∈ ℝ",
        81: "|x| < 1",
        82: "x ∈ ℝ",
        83: "x ∈ ℝ",
        84: "|x| ≤ 1",
        85: "|x| < 1",
        86: "|x| < 1",
        87: "|x| ≤ 1",
        88: "x ∈ ℝ",
        89: "x ∈ ℝ",
        90: "|x| < 1",
        91: "x ∈ ℝ",
        92: "x ∈ ℝ",
        93: "x ≥ -1",
        94: "x = 0 (числовой ряд)",
        95: "x ∈ ℝ, a > -1",
        96: "x ∈ ℝ",
        97: "x > -1",
        98: "x ≥ 0",
        99: "|x| < 1",
        100: "|x| < 1",
        101: "|x| < 1",
        102: "x > 0, t > 0"
    }
    return limits.get(series_num, "Неизвестный ряд")

def check_series_value(series_num, x, param=None):
    """Проверяет, допустимо ли значение x для ряда"""
    try:
        if series_num == 1:  # exp_series
            return 1
        elif series_num == 2:  # cos_series
            return 1
        elif series_num == 3:  # sin_series
            return 1
        elif series_num == 4:  # cosh_series
            return 1
        elif series_num == 5:  # sinh_series
            return 1
        elif series_num == 6:  # bin_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 7:  # four_arctan_series
            return 1 if abs(x) <= 1 else 0
        elif series_num == 8:  # ln1mx_series
            return 1 if abs(x) < 1 and x != 1 else 0
        elif series_num == 9:  # mean_sinh_sin_series
            return 1
        elif series_num == 10:  # exp_squared_erf_series
            return 1
        elif series_num == 11:  # xmb_Jb_two_series
            return 1 if param is not None else 0  # param is b
        elif series_num == 12:  # half_asin_two_x_series
            return 1 if abs(x) <= 0.5 else 0
        elif series_num == 13:  # inverse_1mx_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 14:  # x_1mx_squared_series
            return 1 if abs(x) < 1 and x != 1 else 0
        elif series_num == 15:  # erf_series
            return 1
        elif series_num == 16:  # m_fact_1mx_mp1_inverse_series
            return 1 if abs(x) < 1 and param is not None else 0  # param is m
        elif series_num == 17:  # inverse_sqrt_1m4x_series
            return 1 if abs(x) <= 0.25 else 0
        elif series_num == 18:  # one_twelfth_3x2_pi2_series
            return 1 if abs(x) <= pi else 0
        elif series_num == 19:  # x_twelfth_x2_pi2_series
            return 1 if abs(x) <= pi else 0
        elif series_num == 20:  # ln2_series
            return 1 if x == 0 else 0
        elif series_num == 21:  # one_series
            return 1 if x == 0 else 0
        elif series_num == 22:  # minus_one_quarter_series
            return 1 if x == 0 else 0
        elif series_num == 23:  # pi_3_series
            return 1 if x == 0 else 0
        elif series_num == 24:  # pi_4_series
            return 1 if x == 0 else 0
        elif series_num == 25:  # pi_squared_6_minus_one_series
            return 1 if x == 0 else 0
        elif series_num == 26:  # three_minus_pi_series
            return 1 if x == 0 else 0
        elif series_num == 27:  # one_twelfth_series
            return 1 if x == 0 else 0
        elif series_num == 28:  # eighth_pi_m_one_third_series
            return 1 if x == 0 else 0
        elif series_num == 29:  # one_third_pi_squared_m_nine_series
            return 1 if x == 0 else 0
        elif series_num == 30:  # four_ln2_m_3_series
            return 1 if x == 0 else 0
        elif series_num == 31:  # exp_m_cos_x_sinsin_x_series
            return 1
        elif series_num == 32:  # pi_four_minus_ln2_halfed_series
            return 1
        elif series_num == 33:  # five_pi_twelve_series
            return 1
        elif series_num == 34:  # x_two_series
            return 1
        elif series_num == 35:  # pi_six_min_half_series
            return 1
        elif series_num == 36:  # x_two_throught_squares_series
            return 1
        elif series_num == 37:  # minus_one_ned_in_n_series
            return 1
        elif series_num == 38:  # minus_one_n_fact_n_in_n_series
            return 1
        elif series_num == 39:  # ln_x_plus_one_x_minus_one_halfed_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 40:  # two_arcsin_square_x_halfed_series
            return 1 if abs(x) <= 2 else 0
        elif series_num == 41:  # pi_squared_twelve_series
            return 1 if x == 0 else 0
        elif series_num == 42:  # pi_cubed_32_series
            return 1 if x == 0 else 0
        elif series_num == 43:  # minus_three_plus_ln3_three_devided_two_plus_two_ln2_series
            return 1 if x == 0 else 0
        elif series_num == 44:  # two_ln2_series
            return 1 if x == 0 else 0
        elif series_num == 45:  # pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series
            return 1 if x != 0 else 0
        elif series_num == 46:  # pi_minus_x_2
            return 1 if 0 < x < 2*pi else 0
        elif series_num == 47:  # half_multi_ln_1div2multi1minuscosx
            return 1 if 0 < x < 2*pi else 0
        elif series_num == 48:  # half_minus_sinx_multi_pi_4
            return 1
        elif series_num == 49:  # ln_1plussqrt1plusxsquare_minus_ln_2
            return 1 if x**2 <= 1 else 0
        elif series_num == 50:  # ln_cosx
            return 1 if abs(x) < pi/2 else 0
        elif series_num == 51:  # ln_sinx_minus_ln_x
            return 1 if 0 < x <= pi else 0
        elif series_num == 52:  # pi_8_cosx_square_minus_1_div_3_cosx
            return 1 if abs(x) <= pi/2 else 0
        elif series_num == 53:  # sqrt_oneminussqrtoneminusx_div_x
            return 1 if abs(x) < 1 and x != 0 else 0
        elif series_num == 54:  # one_minus_sqrt_1minus4x_div_2x
            return 1 if abs(x) <= 0.25 and x != 0 else 0
        elif series_num == 55:  # arcsin_x_minus_x_series
            return 1 if abs(x) <= 1 else 0
        elif series_num == 56:  # pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series
            return 1 if 0 < x < 2*pi else 0
        elif series_num == 57:  # abs_sin_x_minus_2_div_pi_series
            return 1 if 0 <= x <= 2*pi else 0
        elif series_num == 58:  # pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series
            return 1 if -pi < x <= pi else 0
        elif series_num == 59:  # minus_3_div_4_or_x_minus_3_div_4_series
            return 1 if -3 < x < 3 else 0
        elif series_num == 60:  # ten_minus_x_series
            return 1 if 5 < x < 15 else 0
        elif series_num == 61:  # x_series
            return 1 if -pi <= x <= pi else 0
        elif series_num == 62:  # minus_x_minus_pi_4_or_minus_pi_4_series
            return 1 if -pi < x <= pi else 0
        elif series_num == 63:  # one_div_two_minus_x_multi_three_plus_x_series
            return 1 if abs(x-1) > 3 else 0
        elif series_num == 64:  # Si_x_series
            return 1
        elif series_num == 65:  # Ci_x_series
            return 1 if x > 0 else 0
        elif series_num == 66:  # Riemann_zeta_func_series
            return 1 if x > 1 else 0
        elif series_num == 67:  # Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series
            return 1 if x > 2 else 0
        elif series_num == 68:  # xsquareplus3_div_xsquareplus2multix_minus_1_series
            return 1 if abs(x-1) > 3 else 0
        elif series_num == 69:  # arcsin_x_series
            return 1 if abs(x) <= 1 else 0
        elif series_num == 70:  # arctg_x_series
            return 1 if abs(x) <= 1 else 0
        elif series_num == 71:  # K_x_series
            return 1
        elif series_num == 72:  # E_x_series
            return 1
        elif series_num == 73:  # sqrt_1plusx_series
            return 1 if x >= -1 else 0
        elif series_num == 74:  # Lambert_W_func_series
            return 1 if abs(x) < 1/e else 0
        elif series_num == 75:  # Incomplete_Gamma_func_series
            return 1 if x > 0 and (param is not None and param > 0) else 0  # param is s
        elif series_num == 76:  # Series_with_ln_number1
            return 1 if x == 0 else 0
        elif series_num == 77:  # Series_with_ln_number2
            return 1 if x == 0 else 0
        elif series_num == 78:  # pi_series
            return 1 if x == 0 else 0
        elif series_num == 79:  # x_min_sqrt_x_series
            return 1 if x >= 0 else 0
        elif series_num == 80:  # arctan_x2_series
            return 1
        elif series_num == 81:  # ln1px4_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 82:  # sin_x2_series
            return 1
        elif series_num == 83:  # arctan_x3_series
            return 1
        elif series_num == 84:  # arcsin_x2_series
            return 1 if abs(x) <= 1 else 0
        elif series_num == 85:  # ln1_m_x2_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 86:  # artanh_x_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 87:  # arcsinh_x_series
            return 1 if abs(x) <= 1 else 0
        elif series_num == 88:  # cos_x2_series
            return 1
        elif series_num == 89:  # sinh_x2_series
            return 1
        elif series_num == 90:  # arctanh_x2_series
            return 1 if abs(x) < 1 else 0
        elif series_num == 91:  # cos3xmin1_div_xsqare_series
            return 1
        elif series_num == 92:  # two_degree_x_series
            return 1
        elif series_num == 93:  # sqrt_1plusx_min_1_min_x_div_2_series
            return 1 if x >= -1 else 0
        elif series_num == 94:  # ln13_min_ln7_div_7_series
            return 1 if x == 0 else 0
        elif series_num == 95:  # Ja_x_series
            return 1 if param is not None and param > -1 else 0  # param is a
        elif series_num == 96:  # one_div_sqrt2_sin_xdivsqrt2_series
            return 1
        elif series_num == 97:  # ln_1_plus_x_div_1plusx2
            return 1 if x > -1 else 0
        elif series_num == 98:  # cos_sqrt_x
            return 1 if x >= 0 else 0
        elif series_num == 99:  # ln_1_plus_x3
            return 1 if abs(x) < 1 else 0
        elif series_num == 100:  # x_div_1minx
            return 1 if abs(x) < 1 else 0
        elif series_num == 101:  # x_div_1minx2
            return 1 if abs(x) < 1 else 0
        elif series_num == 102:  # gamma_series
            return 1 if x > 0 and (param is not None and param > 0) else 0  # param is t
        else:
            return 0
    except Exception as e:
        print(f"Error checking series {series_num}: {e}")
        return 0

def print_transformation_info():
    print("Which transformation would you like to test?")
    print("List of currently available transformations:\n")
    
    transformations = [
        "1 - Shanks Transformation",
        "2 - Epsilon Algorithm",
        "3 - Levin Algorithm",
        "4 - Epsilon Algorithm V-2",
        "5 - S-transformation",
        "6 - D-transformation",
        "7 - Chang - Wynn - Epsilon Algorithm",
        "8 - M-transformation",
        "9 - Weniger transformation",
        "10 - Rho - Wynn transformation",
        "11 - Theta Brezinski transformation",
        "12 - Epsilon Algorithm V-3",
        "13 - Levin - Recursion Algorithm",
        "14 - Lubkin W-transformation",
        "15 - Richardson Algorithm",
        "16 - Ford-Sidi Algorithm",
        "17 - Ford-Sidi Algorithm V-2",
        "18 - Ford-Sidi Algorithm V-3",
        "19 - Epsilon modified Algorithm",
        "20 - Theta modified Algorithm",
        "21 - Epsilon - Aitken - Theta Algorithm"
    ]
    
    print('\n'.join(transformations))


def test_all_series_for_1_function(logger, exec_file, params, consts, plot_dir, output_file):
    S_n_array = []
    T_n_array = []
    lim_array = []
    k = 0
    parameters = []
    for num_series in range(1, len(series_id)-1):
        if (not check_series_value(num_series, float(params[0])) or num_series == 55 or num_series == 79): 
            logger.log(f"Cant build plot and csv file for {series_id[str(num_series)]}, because {get_series_limits(num_series)}, given x = {params[0]}")
            continue
        if num_series in [6, 11, 16, 75, 95, 102]:
            parameters=[str(num_series), params[0], consts[k], *params[1:]]
        else:
            parameters=[str(num_series), *params]
        program = Program(exec_file, parameters)
        program.run()
        S_n = []
        T_n = []
        lim = 0
        try:
            logger.log(f"Values of file:")
            with open(output_file, 'r') as f:
                i = 1
                lim = float(f.readline().strip())
                logger.log(f"Sum of series: {lim}")
                while True:
                    line = f.readline().strip()
                    if not line:
                        break
                    try:
                        S_n.append(float(line))
                        logger.log(f"S_{i}: {S_n[i-1]}")
                    except:
                        S_n.append(None)
                        logger.log_error(f"S_{i}: {line}(replaced by None)")

                    line = f.readline().strip()

                    try:
                        T_n.append(float(line))
                        logger.log(f"T_{i}: {T_n[i-1]}")
                    except:
                        T_n.append(None)
                        logger.log(f"T_{i}: {line}(replaced by None)")
                    i += 1

        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")

        plot_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_{transformations[params[1][0]]}.png")

        x = list(range(len(S_n)))
        lim_arr = [lim for i in range(len(S_n))]

        plt.figure(figsize=(15, 15))
        
        plt.plot(x, S_n, linestyle="--", color="Blue", label="S_n")
        plt.plot(x, T_n, linestyle="--", color="Green", label = "T_n")
        if (lim != 0):
            plt.plot(x, lim_arr, linestyle=":", color="Red", label="sum")

        plt.tight_layout(pad = 2.0)
        plt.legend()
        plt.grid()
        plt.savefig(plot_file)
        plt.close()
        
        logger.log(f"Constructed graph for a {series_id[str(num_series+1)]} and saved to {plot_file}")
    
        S_n_array.append(S_n)
        T_n_array.append(T_n)
        lim_array.append(lim_arr)

        csv_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_{transformations[params[1][0]]}.csv")
        with open(csv_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['S_n', transformations[params[1][0]]])

            for row in zip(S_n, T_n, lim_arr):
                writer.writerow(row)

def test_all_series_for_all_function(logger, exec_file, params, consts, plot_dir, output_file):
    k = 0
    parameters = []

    names = ["S_n"]
    for i in range(len(algorithms)):
        names.append(algorithms[i])

    for num_series in range(1, len(series_id)-1):
        if (not check_series_value(num_series, float(params[0])) or num_series == 55 or num_series == 79): #55 too slow
            logger.log(f"Cant build plot and csv file for {series_id[str(num_series)]}, because {get_series_limits(num_series)}, given x = {params[0]}")
            continue
        if num_series in [6, 11, 16, 75, 95, 102]:
            parameters=[str(num_series), consts[k], "1", *params]
        else:
            parameters=[str(num_series), "1", *params]
        print(parameters)
        program = Program(exec_file, parameters)
        program.run()

        S_n = []
        T_n = [[] for i in range(34)]
        lim = 0
        try:
            logger.log(f"Values of file:")
            with open(output_file, 'r') as f:
                i = 0
                n = 33
                lim = float(f.readline().strip())
                for line in f:
                    if i % n == 0:
                        logger.log("---")
                        S_n.append(float(line))
                        logger.log(f"S_{i//n+1}: {S_n[-1]}")
                    else:
                        try:
                            T_n[(i)%n].append(float(line))
                            logger.log(f"{algorithms[(i-1)%n]}: {T_n[(i)%n][-1]}")
                        except:
                            T_n[(i)%n].append(None)
                            logger.log_error(f"{algorithms[(i-1)%n]}: {line.strip()} (replaced by None)")
                    i += 1
        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")

        plot_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_all_transformations.png")

        x = list(range(len(S_n)))
        lim_arr = [lim for i in range(len(S_n))]


        plt.figure(figsize=(15, 15))
        
        plt.plot(x, S_n, color = "Green", label = "S_n")
        for i in range(1, 33):
            if i not in [27, 30, 31, 32]: #TODO:fix
                plt.plot(x, T_n[i], color=color_dict_hex[i-1], label = f"{algorithms[i-1]}") 

        if (lim != 0):
            plt.plot(x, lim_arr, color="black", label = "Sum")

        plt.tight_layout(pad=2.0)
        plt.legend()
        plt.grid()
        
        plt.savefig(plot_file, bbox_inches='tight')
        plt.close()
        
        logger.log(f"Constructed graph for a {series_id[str(num_series+1)]} and saved to {plot_file}")
    
        csv_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_all_transformations.csv")

        with open(csv_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(names)
            for row in zip(S_n, *T_n[1:-1], lim_arr):
                writer.writerow(row)

def test_speed(logger, exec_file, params, consts, output_file):
    logger.log(f"Testing speed:")  
    i = 0
    for t in range(1, len(transformations)):
        parameters =[]
        if str(t) in ["5", "6", "8", "10", "12", "13"]:
            parameters = [*params[0], params[1], str(t), *consts[i], "5", params[2], params[3]]
            i+=1
        else:
            parameters = [*params[0], params[1], str(t), "5", params[2], params[3]]

        program = Program(exec_file, parameters)
        program.run()
        try:
            with open(output_file, 'r') as f:
                time = f.readline()
                logger.log(f"{transformations[str(t)]}: execution took {time} milliseconds")
        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")

def test_x_for_all_transformations(logger, exec_file, params, consts, plot_dir, output_file, iter):
    k = 0
    parameters = []

    names = ["S_n"]
    for i in range(len(algorithms)):
        names.append(algorithms[i])

    x_p = np.linspace(iter[0], iter[1], iter[2])
    for x in x_p:
        if (params[0][0] != "103" and not check_series_value(int(params[0][0]), x) or int(params[0][0]) == 55 or int(params[0][0] == 79)):
            logger.log(f"Cant build plot and csv file for {series_id[params[0][0]]}, because {get_series_limits(int(params[0][0]))}, given x = {x}")
            continue
        elif (params[0][0] == "103" and not check_series_value(int(params[0][1]), x)):
            logger.log(f"Cant build plot and csv file for {series_id[params[0][1]]}, because {get_series_limits(int(params[0][1]))}, given x = {x}")
            continue
        if len(params[0]) == 1:
            parameters=[params[0][0], str(x), "1", *params[1:]]
        else:
            parameters=[params[0][0], str(x), *params[0][1:], "1", *params[len(params[0]):]]
        program = Program(exec_file, parameters)
        program.run()

        S_n = []
        T_n = [[] for i in range(34)]
        lim = 0
        try:
            logger.log(f"Values of file:")
            with open(output_file, 'r') as f:
                i = 0
                n = 33
                lim = float(f.readline().strip())
                for line in f:
                    if i % n == 0:
                        logger.log("---")
                        S_n.append(float(line))
                        logger.log(f"S_{i//n+1}: {S_n[-1]}")
                    else:
                        try:
                            T_n[(i)%n].append(float(line))
                            logger.log(f"{algorithms[(i-1)%n]}: {T_n[(i)%n][-1]}")
                        except:
                            T_n[(i)%n].append(None)
                            logger.log_error(f"{algorithms[(i-1)%n]}: {line.strip()} (replaced by None)")
                    i += 1
        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")

        plot_file = os.path.join(plot_dir, f"{series_id[params[0][0]]}_x={x}_all_transformations.png")

        x_arr = list(range(len(S_n)))
        lim_arr = [lim for i in range(len(S_n))]


        plt.figure(figsize=(15, 15))
        
        plt.plot(x_arr, S_n, color = "Green", label = "S_n")
        for i in range(1, 33):
            if i not in [27, 30, 31, 32]:
                plt.plot(x_arr, T_n[i], color=color_dict_hex[i-1], label = f"{algorithms[i-1]}") 

        if (lim != 0):
            plt.plot(x_arr, lim_arr, color="black", label = "Sum")

        plt.tight_layout(pad=2.0)
        plt.legend()
        plt.grid()
        
        plt.savefig(plot_file, bbox_inches='tight')
        plt.close()
        
        logger.log(f"Constructed graph for a {series_id[params[0][0]]} and saved to {plot_file}")
    
        csv_file = os.path.join(plot_dir, f"{series_id[params[0][0]]}_x={x}_all_transformations.csv")

        with open(csv_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(names)
            for row in zip(S_n, *T_n[1:-1], lim_arr):
                writer.writerow(row)



def main():
    current_dir = os.path.dirname(os.path.abspath(__file__))
    log_file = os.path.join(current_dir, "output.log")

    logger = Logger(log_file)

    testing_data_dir = 'output_testing_data'
    if not os.path.exists(testing_data_dir):
        os.makedirs(testing_data_dir)
        logger.log(f"New directory has been created - {testing_data_dir}")
    
    testing_data_dir = os.path.join(current_dir, 'output_testing_data')
    for i in range(1, 5):
        if not os.path.exists(os.path.join(testing_data_dir, str(i))):
            os.makedirs(os.path.join(testing_data_dir, str(i)))
            logger.log(f"New directory has been created - {i}")

    plot_dir = os.path.join(current_dir, testing_data_dir)


    exec_file = ""
    if (os.name == "nt"):
        exec_file = "/build/test.exe" # Путь к exe файлу
    else:
        exec_file = "./build/test"

    function_txt = '''
        Choose function:\n
        1)test_all_series(cmp_sum_and_transform for all series)
        2)test_all_series_all_transformation(test_all_transforms)
        3)test_multi_x_for_1_series_and_for_all_transfomations
        4)read_parameters_from_file
        5)test_work_speed_of_transformation_for_series
    '''

    print(function_txt)
    function_id = int(input())
    if function_id == 1:
        plot_dir = os.path.join(plot_dir, "1")

        series_with_const = ["6", "11", "16", "75", "95", "102"]
        consts = []
        transform = []
        for i in range(len(series_with_const)):
            consts.append(input(f"Input const for a {series_id[series_with_const[i]]}:"))

        print_transformation_info()
        transformation = input()
        transform.append(transformation)
        if transformation in ["5"]:
            transform.append(input("Срoose what type of transformation u,t,d or v: "))
            transform.append(input("Use recurrence formula? 1<-true or 0<-false: "))
            transform.append(input("Use standart beta value? 1<-true or 0<-false: "))
            if transform[-1] == 0:
                transform.append(input("Enter parameter beta: "))
        elif transformation in ["6"]:
            transform.append(input("Срoose what type of transformation u,t,d or v: "))
            transform.append(input("Use recurrence formula? 1<-true or 0<-false: "))
        elif transformation in ["8"]:
            transform.append(input("Срoose what type of transformation u,t,d or v: "))
            transform.append(input("Use standart beta value? 1<-true or 0<-false: "))
            if transform[-1] == 0:
                transform.append(input("Enter parameter beta: "))
        elif transformation in ["10"]:
            transform.append(input("choose transformation variant:\nclassic (0), gamma (1), gamma-rho (2): "))
            if transform[-1] == "1":
                transform.append(input("Use standart gamma value? 1<-true or 0<-false:"))
                if transform[-1] == "0":
                    transform.append(input("Enter parameter gamma:"))
            if transform[-1] == "2":
                transform.append(input("Use standart gamma value? 1<-true or 0<-false:"))
                if transform[-1] == "0":
                    transform.append(input("Enter parameter gamma:"))
                transform.append(input("Use standart RHO value? 1<-true or 0<-false:"))
                if transform[-1] == "0":
                    transform.append(input("Enter parameter RHO:"))
        elif transformation in ["12"]:
            transform.append(input("Use standart epsilon value? 1<-true or 0<-false: "))
            if transform[-1] == 0:
                transform.append(input("Enter parameter epsilon: "))
        elif transformation in ["13"]:
            transform.append(input("Use standart beta value? 1<-true or 0<-false: "))
            if transform[-1] == 0:
                transform.append(input("Enter parameter beta: "))

        x = input("Input x: ")
        n = input("Input n: ")
        order = input("Input order: ")

        params = [x, *transform, "1", n, order]
        test_all_series_for_1_function(logger, exec_file, params, consts, plot_dir, "output.txt")

    elif function_id == 2:
        plot_dir = os.path.join(plot_dir, "2")

        parameters = []
        series_with_const = ["6", "11", "16", "75", "95", "102"]
        consts = []

        for i in range(len(series_with_const)):
            consts.append(input(f"Input const for a {series_id[series_with_const[i]]}:"))


        standart = input("| Use standart beta_Levin_S_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)
        if not int(standart):
            beta = input("| Enter parameter beta_Levin_S_algorithm: ")
            parameters.append(beta)

        standart = input("| Use standart gamma_Levin_M_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            gamma = input("| Enter parameter gamma_Levin_M_algorithm: ")
            parameters.append(gamma)

        standart = input("| Use standart gamma_rho_Wynn_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            gamma_rho = input("| Enter parameter gamma_rho_Wynn_algorithm: ")
            parameters.append(gamma_rho)

        standart = input("| Use standart RHO_rho_Wynn_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            rho = input("| Enter parameter RHO_rho_Wynn_algorithm: ")
            parameters.append(rho)

        standart = input("| Use standart beta_levin_recursion_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            beta_rec = input("| Enter parameter beta_levin_recursion_algorithm: ")
            parameters.append(beta_rec)

        standart = input("| Use standart epsilon_algorithm_3 value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            epsilon = input("| Enter parameter epsilon_algorithm_3: ")
            parameters.append(epsilon)

        x = input("Input x: ")
        n = input("Input n: ")
        order = input("Input order: ")

        params = [x, "6", n, order, *parameters]
        test_all_series_for_all_function(logger, exec_file, params, consts, plot_dir, "output.txt")
    elif function_id == 3:
        plot_dir = os.path.join(plot_dir, "3")

        series = []
        consts = []
        transform = []
        parameters = []

        series.append(input("Choose series:"))
        if series[-1] in ["6", "11", "16", "75", "95", "102"]:
            series.append(input("Enter parameter:"))
        elif series[-1] == "103":
            series.append(input("Choose series:"))
            if series[-1] in ["6", "11", "16", "75", "95", "102"]:
                series.append(input("Enter parameter:"))

        standart = input("| Use standart beta_Levin_S_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)
        if not int(standart):
            beta = input("| Enter parameter beta_Levin_S_algorithm: ")
            parameters.append(beta)

        standart = input("| Use standart gamma_Levin_M_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            gamma = input("| Enter parameter gamma_Levin_M_algorithm: ")
            parameters.append(gamma)

        standart = input("| Use standart gamma_rho_Wynn_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            gamma_rho = input("| Enter parameter gamma_rho_Wynn_algorithm: ")
            parameters.append(gamma_rho)

        standart = input("| Use standart RHO_rho_Wynn_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            rho = input("| Enter parameter RHO_rho_Wynn_algorithm: ")
            parameters.append(rho)

        standart = input("| Use standart beta_levin_recursion_algorithm value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            beta_rec = input("| Enter parameter beta_levin_recursion_algorithm: ")
            parameters.append(beta_rec)

        standart = input("| Use standart epsilon_algorithm_3 value? 1<-true or 0<-false : ")
        parameters.append(standart)

        if not int(standart):
            epsilon = input("| Enter parameter epsilon_algorithm_3: ")
            parameters.append(epsilon)

        if series[0] != "103":
            print(f"{get_series_limits(int(series[0]))}) for choosen series")
        else:
            print(f"({get_series_limits(int(series[1]))}) for choosen series")
        print("to set x we will use np.linspace(start, stop, count)")
        start = int(input("Input start: "))
        stop = int(input("Input stop: "))
        step = int(input("Input x count: "))
        n = input("Input n: ")
        order = input("Input order: ")

        params = [series, "6", n, order, *parameters]
        test_x_for_all_transformations(logger, exec_file, params, consts, plot_dir, "output.txt", [start, stop, step])
    elif function_id == 4:
        parameters = []
        plot_dir = os.path.join(plot_dir, "4")
        with open("testing_parameters", 'r') as f:
            for line in f:
                parameters.append(line.split(" "))
        
        for i in range(len(parameters)):
            program = Program(exec_file, parameters[i])
            program.run()

            plot_file = os.path.join(plot_dir, f"plot{i}.png")

            a = find_params(parameters[i])
            log_selected_params(logger, a)

            work_with_data(logger, a[3][0], "output.txt", plot_file)

    elif function_id == 5:
        series = []
        consts = [[] for i in range(6)]

        #TODO: print series info
        series.append(input("Choose series:"))
        if series[-1] in ["6", "11", "16", "75", "95", "102"]:
            series.append(input("Enter parameter:"))
        elif series[-1] == "103":
            series.append(input("Choose series:"))
            if series[-1] in ["6", "11", "16", "75", "95", "102"]:
                series.append(input("Enter parameter:"))

        print(f"\nChoose parameters for {transformations['5']}")
        consts[0].append(input("Сhoose what type of transformation u,t,d or v: "))
        consts[0].append(input("Use recurrence formula? 1<-true or 0<-false: "))
        consts[0].append(input("Use standart beta value? 1<-true or 0<-false: "))
        if consts[0][-1] == "0":
            consts[0].append(input("Enter parameter beta: "))
        print(f"\nChoose parameters for {transformations['6']}")
        consts[1].append(input("Срoose what type of transformation u,t,d or v: "))
        consts[1].append(input("Use recurrence formula? 1<-true or 0<-false: "))
        print(f"\nChoose parameters for {transformations['8']}")
        consts[2].append(input("Срoose what type of transformation u,t,d or v: "))
        consts[2].append(input("Use standart beta value? 1<-true or 0<-false: "))
        if consts[2][-1] == "0":
            consts[2].append(input("Enter parameter beta: "))
        print(f"\nChoose parameters for {transformations['10']}")
        consts[3].append(input("choose transformation variant:\nclassic (0), gamma (1), gamma-rho (2): "))
        if consts[3][-1] == "1":
            consts[3].append(input("Use standart gamma value? 1<-true or 0<-false:"))
            if consts[3][-1] == "0":
                consts[3].append(input("Enter parameter gamma:"))
        if consts[3][-1] == "2":
            consts[3].append(input("Use standart gamma value? 1<-true or 0<-false:"))
            if consts[3][-1] == "0":
                consts[3].append(input("Enter parameter gamma:"))
            consts[3].append(input("Use standart RHO value? 1<-true or 0<-false:"))
            if consts[3][-1] == "0":
                consts[3].append(input("Enter parameter RHO:"))
        print(f"Choose parameters for {transformations['12']}")
        consts[4].append(input("Use standart epsilon value? 1<-true or 0<-false: "))
        if consts[4][-1] == "0":
            consts[4].append(input("Enter parameter epsilon: "))
        print(f"Choose parameters for {transformations['13']}")
        consts[5].append(input("Use standart beta value? 1<-true or 0<-false: "))
        if consts[5][-1] == "0":
            consts[5].append(input("Enter parameter beta: "))
        x = ""

        if series[0] != "103":
            x = input(f"Input x({get_series_limits(int(series[0]))}): ")
        else:
            x = input(f"Input x({get_series_limits(int(series[1]))}): ")
        n = input("Input n: ")
        order = input("Input order: ")
        params = [series, x, n, order]
        test_speed(logger, exec_file, params, consts, "output.txt")

    
    logger.log("------------------------------------------")




if __name__ == "__main__":
    main()