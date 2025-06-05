import subprocess
import os
import logging
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from datetime import datetime
import math
from math import pi, e, log, sqrt, cos, sin, tan, atan, asin, acos, sinh, cosh, tanh, gamma, factorial, erf, erfc
import csv
import logging
from new_logger import (
    setup_loggers, log_execution, timed_algorithm,
    Timer, StatsContext, LogParser, log_selected_params_new
)

# Initialize loggers
loggers = setup_loggers(stats_file_path="output_testing.log")
debug_logger = loggers["debug_logger"]
timing_logger = loggers["timing_logger"]
stats_logger = loggers["stats_logger"]
# === End logger integration ===

algorithm_styles = {
    0: {'color': '#FF0000', 'marker': 'o', 'linestyle': '-', 'markersize': 4},  
    1: {'color': '#00FF00', 'marker': '^', 'linestyle': '--', 'markersize': 4},  
    2: {'color': '#0000FF', 'marker': 's', 'linestyle': '-.', 'markersize': 4}, 
    3: {'color': '#FFFF00', 'marker': 'D', 'linestyle': ':', 'markersize': 4},  
    4: {'color': '#FF00FF', 'marker': 'p', 'linestyle': '-', 'markersize': 5},  
    5: {'color': '#00FFFF', 'marker': 'H', 'linestyle': '--', 'markersize': 5}, 
    6: {'color': '#FF8000', 'marker': 'v', 'linestyle': '-.', 'markersize': 5},  
    7: {'color': '#8000FF', 'marker': '<', 'linestyle': ':', 'markersize': 5}, 
    8: {'color': '#008000', 'marker': '>', 'linestyle': '-', 'markersize': 5},  
    9: {'color': '#000080', 'marker': 'X', 'linestyle': '--', 'markersize': 6},  
    10: {'color': '#800000', 'marker': '*', 'linestyle': '-.', 'markersize': 6}, 
    11: {'color': '#808000', 'marker': 'd', 'linestyle': ':', 'markersize': 6}, 
    12: {'color': '#008080', 'marker': '|', 'linestyle': '-', 'markersize': 6}, 
    13: {'color': '#800080', 'marker': '_', 'linestyle': '--', 'markersize': 6},  
    14: {'color': '#FF0080', 'marker': '.', 'linestyle': '-.', 'markersize': 7},
    15: {'color': '#00FF80', 'marker': '1', 'linestyle': ':', 'markersize': 7}, 
    16: {'color': '#80FF00', 'marker': '2', 'linestyle': '-', 'markersize': 7},  
    17: {'color': '#0080FF', 'marker': '3', 'linestyle': '--', 'markersize': 7},  
    18: {'color': '#FF80FF', 'marker': '4', 'linestyle': '-.', 'markersize': 7}, 
    19: {'color': '#80FFFF', 'marker': '+', 'linestyle': ':', 'markersize': 8},  
    20: {'color': '#FF8040', 'marker': 'x', 'linestyle': '-', 'markersize': 8}, 
    21: {'color': '#40FF80', 'marker': 'P', 'linestyle': '--', 'markersize': 8}, 
    22: {'color': '#8040FF', 'marker': 'h', 'linestyle': '-.', 'markersize': 8},
    23: {'color': '#FFCC00', 'marker': '8', 'linestyle': ':', 'markersize': 8},  
    24: {'color': '#6600CC', 'marker': 'D', 'linestyle': '-', 'markersize': 8},  
    25: {'color': '#CC0066', 'marker': 'd', 'linestyle': '--', 'markersize': 8},  
    26: {'color': '#0066CC', 'marker': 'p', 'linestyle': '-.', 'markersize': 8},  
    27: {'color': '#66CC00', 'marker': 'H', 'linestyle': ':', 'markersize': 8}, 
    28: {'color': '#990033', 'marker': 'v', 'linestyle': '-', 'markersize': 8},  
    29: {'color': '#339900', 'marker': '^', 'linestyle': '--', 'markersize': 8},  
    30: {'color': '#003399', 'marker': 's', 'linestyle': '-.', 'markersize': 8},  
    31: {'color': '#993300', 'marker': '*', 'linestyle': ':', 'markersize': 9}, 
    32: {'color': '#330099', 'marker': 'o', 'linestyle': '-', 'markersize': 9}  
}


standart_algos = {
    "1": "beta_Levin_S_algorithm",
    "2": "gamma_Levin_M_algorithm", 
    "3": "gamma_rho_Wynn_algorithm",
    "4": "RHO_rho_Wynn_algorithm",
    "5": "beta_levin_recursion_algorithm",
    "6": "epsilon_algorithm_3" 
}

transformations = {
    "0": "null_transformation",
    "1": "shanks_transformation",
    "2": "epsilon_algorithm",
    "3": "levin_algorithm",
    "4": "epsilon_algorithm_2",
    "5": "S_algorithm",
    "6": "D_algorithm",
    "7": "chang_epsilon_algorithm",
    "8": "M_algorithm",
    "9": "weniger_transformation",
    "10": "rho_wynn_transformation",
    "11": "brezinski_theta_transformation",
    "12": "epsilon_algorithm_3",
    "13": "levin_recursion",
    "14": "W_algorithm",
    "15": "richardson_algorithm",
    "16": "Ford_Sidi_algorithm",
    "17": "Ford_Sidi_algorithm_two",
    "18": "Ford_Sidi_algorithm_three",
    "19": "Epsilon modified Algorithm",
    "20": "Theta modified Algorithm",
    "21": "Epsilon - Aitken - Theta Algorithm"
}

series_id = {
    "0": "null_series",
    "1": "exp_series",
    "2": "cos_series",
    "3": "sin_series",
    "4": "cosh_series",
    "5": "sinh_series",
    "6": "bin_series",
    "7": "four_arctan_series",
    "8": "ln1mx_series",
    "9": "mean_sinh_sin_series",
    "10": "exp_squared_erf_series",
    "11": "xmb_Jb_two_series",
    "12": "half_asin_two_x_series",
    "13": "inverse_1mx_series",
    "14": "x_1mx_squared_series",
    "15": "erf_series",
    "16": "m_fact_1mx_mp1_inverse_series",
    "17": "inverse_sqrt_1m4x_series",
    "18": "one_twelfth_3x2_pi2_series",
    "19": "x_twelfth_x2_pi2_series",
    "20": "ln2_series",
    "21": "one_series",
    "22": "minus_one_quarter_series",
    "23": "pi_3_series",
    "24": "pi_4_series",
    "25": "pi_squared_6_minus_one_series",
    "26": "three_minus_pi_series",
    "27": "one_twelfth_series",
    "28": "eighth_pi_m_one_third_series",
    "29": "one_third_pi_squared_m_nine_series",
    "30": "four_ln2_m_3_series",
    "31": "exp_m_cos_x_sinsin_x_series",
    "32": "pi_four_minus_ln2_halfed_series",
    "33": "five_pi_twelve_series",
    "34": "x_two_series",
    "35": "pi_six_min_half_series",
    "36": "x_two_throught_squares",
    "37": "minus_one_ned_in_n_series",
    "38": "minus_one_n_fact_n_in_n_series",
    "39": "ln_x_plus_one_x_minus_one_halfed_series",
    "40": "two_arcsin_square_x_halfed_series",
    "41": "pi_squared_twelve_series",
    "42": "pi_cubed_32_series",
    "43": "minus_three_plus_ln3_three_devided_two_plus_two_ln2_series",
    "44": "two_ln2_series",
    "45": "pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series",
    "46": "pi_minus_x_2",
    "47": "half_multi_ln_1div2multi1minuscosx",
    "48": "half_minus_sinx_multi_pi_4",
    "49": "ln_1plussqrt1plusxsquare_minus_ln_2",
    "50": "ln_cosx",
    "51": "ln_sinx_minus_ln_x",
    "52": "pi_8_cosx_square_minus_1_div_3_cosx",
    "53": "sqrt_oneminussqrtoneminusx_div_x",
    "54": "one_minus_sqrt_1minus4x_div_2x",
    "55": "arcsin_x_minus_x_series",
    "56": "pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series",
    "57": "abs_sin_x_minus_2_div_pi_series",
    "58": "pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series",
    "59": "minus_3_div_4_or_x_minus_3_div_4_series",
    "60": "ten_minus_x_series",
    "61": "x_series",
    "62": "minus_x_minus_pi_4_or_minus_pi_4_series",
    "63": "one_div_two_minus_x_multi_three_plus_x_series",
    "64": "Si_x_series",
    "65": "Ci_x_series",
    "66": "Riemann_zeta_func_series",
    "67": "Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series",
    "68": "xsquareplus3_div_xsquareplus2multix_minus_1_series",
    "69": "arcsin_x_series",
    "70": "arctg_x_series",
    "71": "K_x_series",
    "72": "E_x_series",
    "73": "sqrt_1plusx_series",
    "74": "Lambert_W_func_series",
    "75": "Incomplete_Gamma_func_series",
    "76": "Series_with_ln_number1",
    "77": "Series_with_ln_number2",
    "78": "pi_series",
    "79": "x_min_sqrt_x_series",
    "80": "arctan_x2_series",
    "81": "ln1px4_series",
    "82": "sin_x2_series",
    "83": "arctan_x3_series",
    "84": "arcsin_x2_series",
    "85": "ln1_m_x2_series",
    "86": "artanh_x_series",
    "87": "arcsinh_x_series",
    "88": "cos_x2_series",
    "89": "sinh_x2_series",
    "90": "arctanh_x2_series",
    "91": "cos3xmin1_div_xsqare_series",
    "92": "two_degree_x_series",
    "93": "sqrt_1plusx_min_1_min_x_div_2_series",
    "94": "ln13_min_ln7_div_7_series",
    "95": "Ja_x_series",
    "96": "one_div_sqrt2_sin_xdivsqrt2_series",
    "97": "ln_1plusx_div_1plusx2",
    "98": "cos_sqrt_x",
    "99": "ln_1_plus_x3",
    "100": "x_div_1minx",
    "101": "x_div_1minx2",
    "102": "gamma_series",
    "103": "ModifiedSeries",
}

algorithms = {
    0: "shanks",
    1: "epsilon_v-1",
    2: "epsilon_v-2",
    3: "epsilon_v-3",
    4: "rho-wynn_rho_transform",
    5: "rho-wynn_generilized_transform",
    6: "rho-wynn_gamma_rho_transform",
    7: "theta-brezinski",
    8: "chang_epsilon_wynn",
    9: "levin_standart",
    10: "levin_recursive",
    11: "levin-sidi_S_U",
    12: "levin-sidi_S_T",
    13: "levin-sidi_S_D",
    14: "levin-sidi_S_V",
    15: "levin-sidi_D_U",
    16: "levin-sidi_D_T",
    17: "levin-sidi_D_D",
    18: "levin-sidi_D_V",
    19: "levin-sidi_M_U",
    20: "levin-sidi_M_T",
    21: "levin-sidi_M_D",
    22: "levin-sidi_M_V",
    23: "weniger",
    24: "lubkin_W",
    25: "Richardson",
    26: "Ford-Sidi",
    27: "Ford-Sidi_v-2",
    28: "Ford-Sidi_V3",
    29: "Epsilon modified Algorithm",
    30: "Theta modified Algorithm",
    31: "Epsilon - Aitken - Theta Algorithm"

}

color_dict_hex = {
    0: '#FF0000',  
    1: '#00FF00',  
    2: '#0000FF',  
    3: '#FFFF00',  
    4: '#FF00FF', 
    5: '#00FFFF',  
    6: '#FF8000',
    7: '#8000FF', 
    8: '#008000', 
    9: '#000080',  
    10: '#800000', 
    11: '#808000', 
    12: '#008080', 
    13: '#800080', 
    14: '#FF0080',
    15: '#00FF80', 
    16: '#80FF00', 
    17: '#0080FF', 
    18: '#FF80FF', 
    19: '#80FFFF', 
    20: '#FF8040',
    21: '#40FF80',
    22: '#8040FF', 
    23: '#FFCC00', 
    24: '#6600CC', 
    25: '#CC0066', 
    26: '#0066CC', 
    27: '#66CC00', 
    28: '#990033', 
    29: '#339900', 
    30: '#003399', 
    31: '#993300', 
    32: '#330099', 
}

functions = {
    "0": "null_test_function_id", 
    "1": "cmp_sum_and_transform_id", 
    "2": "cmp_a_n_and_transform_id", 
    "3": "transformation_remainder_id", 
    "4": "cmp_transformations_id",
    "5": "eval_transform_time_id",
    "6": "test_all_transforms_id"
}



class Program:
    def __init__(self, file, params):
        self.file = file
        self.cl_arguments = params
        self.output = ""
        self.stderr = ""

    def info(self):
        print("Executable file: ", self.file)
        print("Arguments, that we inputing in command line: ", self.cl_arguments)

    def run(self):
        process = subprocess.Popen(
            [self.file],  
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        for line in self.cl_arguments:
            process.stdin.write(line + "\n")
            process.stdin.flush()
        self.output, self.stderr = process.communicate()

    def get_output(self):
        return self.output

    def get_error(self):
        return self.stderr                                                                                                                                                                                                                                                                                                                

'''
def log_selected_params(logger, params):
    series, x, transformation, function, n, order = params
    logger.log(f"Testing parameters:")

    if (len(series) == 1):
        logger.log(f"Series: {series_id[series[0]]}")
    elif (series[0] == "103") and len(series) == 2:
        logger.log(f"Series: {series_id[series[0]]}")
        logger.log(f"ModifiedSeries: {series_id[series[1]]}")
    elif (series[0] == "103") and len(series) == 3:
        logger.log(f"Series: {series_id[series[0]]}")
        logger.log(f"ModifiedSeries: {series_id[series[1]]}")
        logger.log(f"Series constanta: {series[2]}")
    else:
        logger.log(f"Series: {series_id[series[0]]}")
        logger.log(f"Series constanta: {series[1]}")

    logger.log(f"X: {x[0]}")
    
    logger.log(f"Transformation: {transformations[transformation[0]]}")
    if transformation[0] in ["5", "6", "8"]:
        if transformation[2] == "0":
            logger.log(f"Type of transformation: {transformation[1]} not recurrence")
        else:
            logger.log(f"Type of transformation: {transformation[1]} recurrence")

    elif transformation[0] == "10":
        if transformation[1] == "0":
            logger.log(f"Type of transformation: classic")
        if transformation[1] == "1":
            logger.log(f"Type of transformation: gamma")
            logger.log(f"Gamma: {transformation[2]}")
        if transformation[1] == "2":
            logger.log(f"Type of transformation: gamma-rho")
            logger.log(f"Gamma: {transformation[2]}")
            logger.log(f"RHO: {transformation[3]}")
    
    elif transformation[0] == "13":
            logger.log(f"Parameter beta: {transformation[2]}")

    logger.log(f"Function: {functions[function[0]]}")
    if function[0] == "4":
        logger.log(f"Compare with: {transformations[function[1]]}")
        if function[1] in ["5", "6", "8"]:
            if function[3] == "0":
                logger.log(f"Type of transformation: {function[2]} not recurrence")
            else:
                logger.log(f"Type of transformation: {function[2]} recurrence")

        elif function[1] == "10":
            if function[2] == "0":
                logger.log(f"Type of transformation: classic")
            if function[2] == "1":
                logger.log(f"Type of transformation: gamma")
                logger.log(f"Gamma: {function[3]}")
            if function[2] == "2":
                logger.log(f"Type of transformation: gamma-rho")
                logger.log(f"Gamma: {function[3]}")
                logger.log(f"RHO: {function[4]}")
        
        elif function[1] == "13":
                logger.log(f"Parameter beta: {function[2]}")
    elif function[0] == "6":
        for i in range(0, 7):
            if function[i] == "1":
                logger.log(f"Use standart {i} algorithm")
            elif function[i] == "0":
                logger.log(f"Use not standart {standart_algos[i]} algorithm")


    logger.log(f"N: {n[0]}")
    logger.log(f"order: {order[0]}")
'''

def setup_directories(current_dir):
    testing_data_dir = os.path.join(current_dir, 'output_testing_data')
    if not os.path.exists(testing_data_dir):
        os.makedirs(testing_data_dir)
        debug_logger.info(f"New directory has been created - {testing_data_dir}")

    for i in range(1, 5):
        sub_dir = os.path.join(testing_data_dir, str(i))
        if not os.path.exists(sub_dir):
            os.makedirs(sub_dir)
            debug_logger.info(f"New directory has been created - {sub_dir}") # Corrected to sub_dir

    return testing_data_dir

def get_transformation_params(transformation_id):
    params = [transformation_id]
    
    if transformation_id in ["5", "6", "8"]:
        params.append(input("Choose what type of transformation u,t,d or v: "))
        params.append(input("Use recurrence formula? 1<-true or 0<-false: "))
        if transformation_id == "5":
            params.append(input("Use standart beta value? 1<-true or 0<-false: "))
            if params[-1] == "0":
                params.append(input("Enter parameter beta: "))
    
    elif transformation_id == "10":
        params.append(input("choose transformation variant:\nclassic (0), gamma (1), gamma-rho (2): "))
        if params[-1] == "1":
            params.append(input("Use standart gamma value? 1<-true or 0<-false:"))
            if params[-1] == "0":
                params.append(input("Enter parameter gamma:"))
        elif params[-1] == "2":
            params.append(input("Use standart gamma value? 1<-true or 0<-false:"))
            if params[-1] == "0":
                params.append(input("Enter parameter gamma:"))
            params.append(input("Use standart RHO value? 1<-true or 0<-false:"))
            if params[-1] == "0":
                params.append(input("Enter parameter RHO:"))
    
    elif transformation_id in ["12", "13"]:
        params.append(input("Use standart value? 1<-true or 0<-false: "))
        if params[-1] == "0":
            param_name = "epsilon" if transformation_id == "12" else "beta"
            params.append(input(f"Enter parameter {param_name}: "))
    
    return params

def get_series_params(series_id):
    params = [series_id]
    if series_id in ["6", "11", "16", "75", "95", "102"]:
        params.append(input(f"Input const for {series_id[series_id]}: "))
    elif series_id == "103":
        params.append(input("Choose series to modify: "))
        if params[-1] in ["6", "11", "16", "75", "95", "102"]:
            params.append(input(f"Input const for modified series: "))
    return params


def get_algorithm_params():
    params = []
    algo_configs = {
        "1": ("beta_Levin_S_algorithm", "beta"),
        "2": ("gamma_Levin_M_algorithm", "gamma"),
        "3": ("gamma_rho_Wynn_algorithm", "gamma_rho"),
        "4": ("RHO_rho_Wynn_algorithm", "RHO"),
        "5": ("beta_levin_recursion_algorithm", "beta_rec"),
        "6": ("epsilon_algorithm_3", "epsilon")
    }

    for algo_name, param_name in algorithms:
        standart = input(f"Use standart {algo_name} value? 1<-true or 0<-false: ")
        params.append(standart)
        if standart == "0":
            if key == "3": 
                params.append(input(f"Enter parameter gamma for {algo_name}: "))
                params.append(input(f"Enter parameter RHO for {algo_name}: "))
            else:
                params.append(input(f"Enter parameter {param_name_stem} for {algo_name}: "))
    return params

def generate_filename(params, ext=".png"):
    series_num = params[0]
    x_value = params[1]
    trans1_num = params[2]
    trans2_num = params[3]
    n_value = params[-2]
    order_value = params[-1]
    
    series_name = series_id.get(series_num, f"series_{series_num}")
    trans1_name = transformations.get(trans1_num, f"trans_{trans1_num}")
    trans2_name = transformations.get(trans2_num, f"trans_{trans2_num}")
    
    # Формируем имя файла
    filename = (
        f"cmp_{trans1_name}_vs_{trans2_name}_"
        f"series_{series_name}_"
        f"x{x_value}_"
        f"n{n_value}_"
        f"order{order_value}"
        f"{ext}"
    )
    
    filename = filename.replace(" ", "_").replace("/", "-").replace("\\", "-")
    return filename[:150] 

@log_execution(logger_name="debug_logger", level=logging.INFO)
@timed_algorithm(logger_name="timing_logger", level=logging.INFO)
def run_program_and_process_output(exec_file, params, output_file): # Removed logger argument
    program = Program(exec_file, params)
    program.run()

    try:
        with open(output_file, 'r') as f:
            return f.read().splitlines()
    except Exception as e:
        debug_logger.error(f"Error reading from file {output_file}: {str(e)}")
        return []

def save_to_csv(data, filename, headers=None):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        if headers:
            writer.writerow(headers)
        for row in data:
            writer.writerow(row)

@log_execution(logger_name="debug_logger", level=logging.INFO)
@timed_algorithm(logger_name="timing_logger", level=logging.INFO)
def plot_comparison(x, y1, y2, y3, labels, filename):
    plt.figure(figsize=(15, 15))
    plt.plot(x, y1, linestyle="--", color="Blue", label=labels[0])
    plt.plot(x, y2, linestyle="--", color="Green", label=labels[1])
    plt.plot(x, y3, linestyle=":", color="Red", label=labels[2]) # limit of series

    plt.tight_layout(pad=2.0)
    plt.legend()
    plt.grid()
    plt.savefig(filename)
    plt.close()

@log_execution(logger_name="debug_logger", level=logging.INFO)
@timed_algorithm(logger_name="timing_logger", level=logging.INFO)
def plot_multiple_algorithms(x, S_n, T_n, lim, filename):
    plt.figure(figsize=(18, 12))
    
    plt.plot(x, S_n, color='black', linewidth=2, linestyle='-', 
             marker='o', markersize=6, label='Partial sums (S_n)')
    
    for i in range(1, 33):
        if i not in [27, 30, 31, 32]:
            algo_name = algorithms[i] 
            if algo_name and i in T_n and T_n[i] and i in algorithm_styles:
                style = algorithm_styles[i-1]
                plt.plot(x, T_n[i], 
                        color=style['color'],
                        linestyle=style['linestyle'],
                        marker=style['marker'],
                        markersize=style['markersize'],
                        linewidth=1.5,
                        label=f'{algorithms[i-1]}')
            else:
                debug_logger.warning(f"Length mismatch for algorithm {algo_name} (data length {len(T_n[i])}, x length {len(x)}). Skipping plot.")	
    
    #limit
    plt.axhline(y=lim, color='red', linewidth=2, linestyle='--', label='Limit')
        
    y_min = lim - (5 / n * abs(lim))
    y_max = lim + (5 / n * abs(lim))
    plt.ylim(y_min, y_max)
    
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
    
    plt.grid(True, linestyle=':', alpha=0.7)
    plt.xlabel('Iteration', fontsize=12)
    plt.ylabel('Value', fontsize=12)
    plt.title('Comparison of Convergence Acceleration Algorithms', fontsize=14)
    
    plt.tight_layout()
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    plt.close()

def find_params(params):
    try:
        result = {
            'series': [],
            'x': None,
            'transformation': [],
            'function': [],
            'n': None,
            'order': None
        }
        
        param_index = 0
        
        result['series'].append(params[param_index])
        param_index += 1
        
        result['x'] = params[param_index]
        param_index += 1
        
        if result['series'][0] in ["6", "11", "16", "75", "95", "102"]:
            result['series'].append(params[param_index])
            param_index += 1
        elif result['series'][0] == "103":
            result['series'].append(params[param_index]) 
            param_index += 1
            if result['series'][1] in ["6", "11", "16", "75", "95", "102"]:
                result['series'].append(params[param_index])
                param_index += 1
        
        result['transformation'].append(params[param_index])
        param_index += 1
        
        if result['transformation'][0] in ["5", "6", "8"]:
            result['transformation'].append(params[param_index])
            param_index += 1
            result['transformation'].append(params[param_index])  
            param_index += 1
        
        elif result['transformation'][0] == "13":
            use_default_beta = params[param_index]
            result['transformation'].append(use_default_beta)
            param_index += 1
            
            if use_default_beta == "0":
                result['transformation'].append(params[param_index])
                param_index += 1
        
        elif result['transformation'][0] == "10":
            transform_type = params[param_index]
            result['transformation'].append(transform_type)
            param_index += 1
            
            if transform_type == "1": 
                use_default_gamma = params[param_index]
                result['transformation'].append(use_default_gamma)
                param_index += 1
                
                if use_default_gamma == "0":
                    result['transformation'].append(params[param_index])  
                    param_index += 1
                    
            elif transform_type == "2":  
                use_default_gamma = params[param_index]
                result['transformation'].append(use_default_gamma)
                param_index += 1
                
                if use_default_gamma == "0":
                    result['transformation'].append(params[param_index])  
                    param_index += 1
                
                use_default_rho = params[param_index]
                result['transformation'].append(use_default_rho)
                param_index += 1
                
                if use_default_rho == "0":
                    result['transformation'].append(params[param_index])
                    param_index += 1
        
        result['function'].append(params[param_index])
        param_index += 1
        
        result['n'] = params[param_index]
        param_index += 1
        
        result['order'] = params[param_index]
        param_index += 1
        
        if result['function'][0] == "4":
            result['function'].append(params[param_index])  
            param_index += 1
            
            if result['function'][1] in ["5", "6", "8"]:
                result['function'].append(params[param_index])  
                param_index += 1
                result['function'].append(params[param_index])  
                param_index += 1
                
            elif result['function'][1] == "10":
                use_default_beta = params[param_index]
                result['function'].append(use_default_beta)
                param_index += 1
                
                if use_default_beta == "0":
                    result['function'].append(params[param_index])  
                    param_index += 1
                    
            elif result['function'][1] == "13":
                transform_type = params[param_index]
                result['function'].append(transform_type)
                param_index += 1
                
                if transform_type == "1": 
                    use_default_gamma = params[param_index]
                    result['function'].append(use_default_gamma)
                    param_index += 1
                    
                    if use_default_gamma == "0":
                        result['function'].append(params[param_index])  
                        param_index += 1
                        
                elif transform_type == "2": 
                    use_default_gamma = params[param_index]
                    result['function'].append(use_default_gamma)
                    param_index += 1
                    
                    if use_default_gamma == "0":
                        result['function'].append(params[param_index]) 
                        param_index += 1
                    
                    use_default_rho = params[param_index]
                    result['function'].append(use_default_rho)
                    param_index += 1
                    
                    if use_default_rho == "0":
                        result['function'].append(params[param_index])  
                        param_index += 1
        
        elif result['function'][0] == "6":
            algorithms = [
                'beta_Levin_S_algorithm',
                'gamma_Levin_M_algorithm',
                'gamma_rho_Wynn_algorithm',
                'RHO_rho_Wynn_algorithm',
                'beta_levin_recursion_algorithm',
                'epsilon_algorithm_3'
            ]
            
            for algo in algorithms:
                use_default = params[param_index]
                result['function'].append(use_default)
                param_index += 1
                
                if use_default == "0":
                    result['function'].append(params[param_index]) 
                    param_index += 1
        
        return [result['series'], result['x'], result['transformation'], result['function'], result['n'], result['order']]

    except IndexError:
        debug_logger.error(f"Failed to parse parameters (IndexError): {params_list_str}")
        return [result['series'], result['x'], result['transformation'], result['function'], result['n'], result['order']]
    except Exception as e:
        debug_logger.error(f"Exception in find_params for {params_list_str}: {e}")
        return [result['series'], result['x'], result['transformation'], result['function'], result['n'], result['order']]


def get_series_limits(series_num):
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
        debug_logger.error(f"Error in check_series_value for series {series_num}, x={x}: {e}")
        return 0

def print_transformation_info():
    print("Which transformation would you like to test?")
    print("List of currently available transformations:\n")
    
    transformations_list = [ 
        f"{key} - {transformations[key]}" for key in sorted(transformations.keys(), key=int) if key != "0"
    ]
    print('\n'.join(transformations_list))

def process_function_1_data(output_file): # Removed logger argument
    S_n = []
    T_n = []
    lim = 0.0
    try:
        debug_logger.info(f"Reading S_n, T_n, lim from: {output_file}")
        with open(output_file, 'r') as f:
            i = 1
            try:
                lim_str = f.readline().strip()
                lim = float(lim_str)
                debug_logger.info(f"Sum of series (lim): {lim}")
            except ValueError:
                debug_logger.error(f"Could not parse limit value: {lim}. Defaulting to 0.0.")
                lim = 0.0

            while True:
                line_s = f.readline().strip()
                if not line_s: 
                    break 
                try:
                    S_n.append(float(line_s))
                    debug_logger.info(f"S_{i}: {S_n[-1]}") 
                except ValueError:
                    S_n.append(None)
                    debug_logger.warning(f"S_{i}: Could not parse '{line_s}', replaced by None")

                line_t = f.readline().strip()

                try:
                    T_n.append(float(line_t))
                    debug_logger.info(f"T_{i}: {T_n[-1]}") 
                except ValueError:
                    T_n.append(None)
                    debug_logger.warning(f"T_{i}: Could not parse '{line_t}', replaced by None")
                i += 1
            #debug_logger.info(f"Processed {i-1} pairs of S_n, T_n values.")
    except FileNotFoundError:
        debug_logger.error(f"File not found: {output_file}")
    except Exception as e:
        debug_logger.error(f"Error reading from file {output_file}: {str(e)}")

    return S_n, T_n, lim

def save_function_1_results(S_n, T_n, lim, plot_file, csv_file, transformation_name):
    x = list(range(len(S_n)))
    lim_arr = [lim for _ in range(len(S_n))]
    
    plot_comparison(
        x, S_n, T_n, lim_arr,
        ['S_n', f'T_n({transformation_name})', 'Sum'],
        plot_file
    )
    
    save_to_csv(
        zip(S_n, T_n, lim_arr),
        csv_file,
        ['S_n', f"T_n({transformation_name})", 'Sum']
    )
    
    debug_logger.info(f"Function 1 results saved to {plot_file} and {csv_file}")

def process_function_6_data(output_file):
    S_n = []
    T_n = [[] for _ in range(34)]
    lim = 0.0
    try:
        debug_logger.info(f"Reading S_n and multiple T_n from: {output_file}")
        with open(output_file, 'r') as f:
            i = 0
            n = 33
            try:
                lim_str = f.readline().strip()
                lim = float(lim_str)
                debug_logger.info(f"Sum of series (lim): {lim}")
            except:
                debug_logger.error(f"Could not parse limit value: {lim_str}. Defaulting to 0.0.")
                lim = 0.0
            for line in f:
                if i % n == 0:
                    logger.log("---")
                    try:
                        S_n.append(float(line))
                        debug_logger.info(f"S_{i//n+1}: {S_n[-1]}")
                    except:
                        S_n.append(None)
                        debug_logger.warning(f"S_{i//n+1}: Could not parse '{line}', replaced by None")
                        debug_logger.info(f"S_{i//n+1}: {S_n[-1]}")
                    S_n.append(float(line))
                else:
                    try:
                        T_n[(i)%n].append(float(line))
                        debug_logger.warning(f"{algorithms[(i-1)%n]}: {T_n[(i)%n][-1]}")
                    except:
                        T_n[(i)%n].append(None)
                        debug_logger.warning(f"{algorithms[(i-1)%n]}: '{line.strip()}' (replaced by None)")
                i += 1
    except Exception as e:
        debug_logger.warning(f"Error reading from file: {str(e)}")    
    return S_n, T_n, lim

def save_function_6_results(S_n, T_n, lim, plot_file, csv_file):
    x = list(range(len(S_n)))
    lim_arr = [lim for _ in range(len(S_n))]
    
    headers = ["S_n"] + [algorithms[i] for i in range(len(algorithms))] + ["Sum"]
    
    plot_multiple_algorithms(x, S_n, T_n, lim, plot_file)
    
    save_to_csv(
        zip(S_n, *T_n[1:-1], lim_arr),
        csv_file,
        headers
    )
    
    debug_logger.info(f"Results saved to {plot_file} and {csv_file}")



def process_speed_data(output_file, transformation_name): # Removed logger argument
    try:
        with open(output_file, 'r') as f:
            time = f.readline().strip()
            debug_logger.info(f"{transformation_name}: execution took {time} milliseconds")
            return time
    except FileNotFoundError:
        debug_logger.error(f"Speed data file not found: {output_file}")
    except Exception as e:
        debug_logger.error(f"Error reading speed data from file {output_file}: {str(e)}")
    return None

def save_speed_results(times, transformation_names, csv_file): 
    data = zip(transformation_names, times)
    save_to_csv(data, csv_file, ['Transformation', 'Time (ms)'])
    debug_logger.info(f"Speed results saved to {csv_file}")

def process_function_2_data(output_file):
    a_n = []
    t_n = []
    diff = []
    try:
        debug_logger.info(f"Reading a_n, t_n, diff from: {output_file}")
        with open(output_file, 'r') as f:
            i = 1
            while True:
                line = f.readline().strip()
                if not line:
                    break
                try:
                    a_n.append(float(line))
                    debug_logger.info(f"a_{i}: {a_n[i-1]}")
                except:
                    a_n.append(None)
                    debug_logger.warning(f"a_{i}: {line}(replaced by None)")

                line = f.readline().strip()
                try:
                    t_n.append(float(line))
                    debug_logger.info(f"t_{i}: {t_n[i-1]}")
                except:
                    t_n.append(None)
                    debug_logger.warning(f"t_{i}: {line}(replaced by None)")

                line = f.readline().strip()
                try:
                    diff.append(float(line))
                    debug_logger.info(f"t_{i} - a{i}: {diff[i-1]}")
                except:
                    diff.append(None)
                    debug_logger.warning(f"t_{i} - a{i}: {line}(replaced by None)")

                i += 1

    except FileNotFoundError: 
        debug_logger.error(f"File not found: {output_file}")
    except Exception as e: 
        debug_logger.error(f"Error reading file {output_file}: {e}")

    return a_n, t_n, diff

def process_function_3_data(output_file): # Removed logger argument
    diff = []
    try:
        debug_logger.info(f"Reading S-T_n diff from: {output_file}")
        with open(output_file, 'r') as f:
            i = 1
            for line in f:
                try: 
                    diff.append(float(line.strip()))
                    debug_logger.info(f"S-T_{i}: {diff[i-1]}")
                except ValueError: 
                    diff.append(None)
                    debug_logger.warning(f"S-T_{i} parse error: '{line.strip()}'")
                i += 1
    except FileNotFoundError: 
        debug_logger.error(f"File not found: {output_file}")
    except Exception as e: 
        debug_logger.error(f"Error reading file {output_file}: {e}")
    return diff

def process_function_4_data(output_file):
    first_transform = []
    second_transform = []
    try:
        debug_logger.info(f"Reading two transformations data from: {output_file}")
        with open(output_file, 'r') as f:
            i = 1
            while True:
                line = f.readline().strip()
                if not line:
                    break
                try:
                    first_transform.append(float(line))
                    debug_logger.info(f"first_transformation_{i}: {first_transform[i-1]}")
                except:
                    first_transform.append(None)
                    debug_logger.warning(f"first_transformation_{i}: {line}(replaced by None)")

                line = f.readline().strip()
                try:
                    second_transform.append(float(line))
                    debug_logger.info(f"second_transformation_{i}: {second_transform[i-1]}")
                except:
                    second_transform.append(None)
                    debug_logger.warning(f"second_transformation_{i}: {line}(replaced by None)")
                i += 1

                try:
                    if first_transform[-1] > second_transform[-1]:
                        debug_logger.info(f"First transformation faster")
                    else:
                        debug_logger.info(f"Second transformation faster")
                except:
                    debug_logger.warning(f"Can not compare first and second transformation")
    except FileNotFoundError: 
        debug_logger.error(f"File not found: {output_file}")
    except Exception as e: 
        debug_logger.error(f"Error reading file {output_file}: {e}")
    return first_transform, second_transform

def process_function_5_data(output_file, logger):
    try:
        logger.log(f"Values of file:")
        with open(output_file, 'r') as f:
            time = f.readline()
            logger.log(f"execution took {time} milliseconds")
            return time
    except Exception as e:
        logger.log_error(f"Error reading from file: {str(e)}")
        return None

def work_with_data(function_id, file, plot_file):
    csv_file = os.path.splitext(plot_file)[0] + ".csv"
    
    if function_id == "1":
        S_n, T_n, lim = process_function_1_data(file)
        save_function_1_results(S_n, T_n, lim, plot_file, csv_file, "")
    
    elif function_id == "2":
        a_n, t_n, diff = process_function_2_data(file)
        save_to_csv(
            zip(a_n, t_n, diff),
            csv_file,
            ['a_n', 't_n', 't_n - a_n']
        )
    
    elif function_id == "3":
        diff = process_function_3_data(file)
        save_to_csv(
            [[d] for d in diff],
            csv_file,
            ['S_n - T_n']
        )
    
    elif function_id == "4":
        first, second = process_function_4_data(file)
        save_to_csv(
            zip(first, second),
            csv_file,
            ['First transformation', 'Second transformation']
        )
    
    elif function_id == "5":
        time = process_function_5_data(file)
        save_to_csv(
            [[time]],
            csv_file,
            ['Execution time (ms)']
        )
    
    elif function_id == "6":
        S_n, T_n, lim = process_function_6_data(file)
        save_function_6_results(S_n, T_n, lim, plot_file, csv_file)


def test_all_series_for_1_function(exec_file, params, consts, plot_dir, output_file):
    S_n_array = []
    T_n_array = []
    lim_array = []
    k = 0
    
    for num_series in range(1, len(series_id)-1):
        if not check_series_value(num_series, float(params[0])) or num_series in [55, 79]:
            debug_logger.error(f"Cant build plot for {series_id[str(num_series)]}, x = {params[0]}")
            continue
        
        if num_series in [6, 11, 16, 75, 95, 102]:
            parameters = [str(num_series), params[0], consts[k], *params[1:]]
            k += 1
        else:
            parameters = [str(num_series), *params]
        
        run_program_and_process_output(exec_file, parameters, output_file)
        S_n, T_n, lim = process_function_1_data(output_file)
        
        plot_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_{transformations[params[1][0]]}.png")
        csv_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_{transformations[params[1][0]]}.csv")
        
        save_function_1_results(
            S_n, T_n, lim, 
            plot_file, csv_file,
            transformations[params[1][0]], 
        )
        
        S_n_array.append(S_n)
        T_n_array.append(T_n)
        lim_array.append([lim]*len(S_n))

def test_all_series_for_all_function(exec_file, params, consts, plot_dir, output_file):
    k = 0
    
    for num_series in range(1, len(series_id)-1):
        if not check_series_value(num_series, float(params[0])) or num_series in [55, 79]:
            debug_logger.error(f"Cant build plot for {series_id[str(num_series)]}, x = {params[0]}")
            continue
        
        if num_series in [6, 11, 16, 75, 95, 102]:
            parameters = [str(num_series), consts[k], "1", *params]
            k += 1
        else:
            parameters = [str(num_series), "1", *params]
        
        run_program_and_process_output(exec_file, parameters, output_file)
        S_n, T_n, lim = process_function_6_data(output_file)
        
        plot_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_all_transformations.png")
        csv_file = os.path.join(plot_dir, f"{series_id[str(num_series+1)]}_all_transformations.csv")
        
        save_function_6_results(S_n, T_n, lim, plot_file, csv_file)


def test_speed(exec_file, params, consts, output_file):
    times = []
    trans_names = []
    i=0
    for t in range(1, len(transformations)):
        parameters = []
        if str(t) in ["5", "6", "8", "10", "12", "13"]:
            parameters = [*params[0], params[1], str(t), *consts[i], "5", params[2], params[3]]
            i += 1
        else:
            parameters = [*params[0], params[1], str(t), "5", params[2], params[3]]
        
        run_program_and_process_output(exec_file, parameters, output_file)
        time = process_speed_data(output_file, transformations[str(t)])
        
        if time:
            times.append(time)
            trans_names.append(transformations[str(t)])
    
    csv_file = os.path.join("speed_results.csv")
    save_speed_results(times, trans_names, csv_file)

def test_x_for_all_transformations(exec_file, params, consts, plot_dir, output_file, iter):
    x_p = np.linspace(iter[0], iter[1], iter[2])
    
    for x in x_p:
        if params[0][0] != "103" and not check_series_value(int(params[0][0]), x):
            debug_logger.error(f"Cant build plot for {series_id[params[0][0]]}, x = {x}")
            continue
        elif (params[0][0] == "103" and not check_series_value(int(params[0][1]), x)):
            debug_logger.error(f"Cant build plot for {series_id[params[0][1]]}, x = {x}")
            continue
        
        if len(params[0]) == 1:
            parameters = [params[0][0], str(x), "1", *params[1:]]
        else:
            parameters = [params[0][0], str(x), *params[0][1:], "1", *params[len(params[0]):]]
        
        run_program_and_process_output(exec_file, parameters, output_file)
        S_n, T_n, lim = process_function_6_data(output_file)
        
        plot_file = os.path.join(plot_dir, f"{series_id[params[0][0]]}_x={x}_all_transformations.png")
        csv_file = os.path.join(plot_dir, f"{series_id[params[0][0]]}_x={x}_all_transformations.csv")
        
        save_function_6_results(S_n, T_n, lim, plot_file, csv_file)


def main():
    current_dir = os.path.dirname(os.path.abspath(__file__))    
    testing_data_dir = setup_directories(current_dir)
    
    exec_file = r"C:\Users\DLR_ACER\HW_Mirea\shanks-university\x64\Debug\shanks_transformation.exe" if os.name == "nt" else "./build/test"
    
    print('''
    Choose function:
    1) test_all_series(cmp_sum_and_transform for all series)
    2) test_all_series_all_transformation(test_all_transforms)
    3) test_multi_x_for_1_series_and_for_all_transfomations
    4) read_parameters_from_file
    5) test_work_speed_of_transformation_for_series
    ''')
    
    function_id = int(input())
    
    if function_id == 1:
        plot_dir = os.path.join(testing_data_dir, "1")
        series_with_const = ["6", "11", "16", "75", "95", "102"]
        consts = [input(f"Input const for {series_id[s]}:") for s in series_with_const]
        
        print_transformation_info()
        transform = get_transformation_params(input())
        
        x = input("Input x: ")
        n = input("Input n: ")
        order = input("Input order: ")
        
        params = [x, *transform, "1", n, order]
        test_all_series_for_1_function(exec_file, params, consts, plot_dir, "output.txt")
    
    elif function_id == 2:
        plot_dir = os.path.join(testing_data_dir, "2")
        series_with_const = ["6", "11", "16", "75", "95", "102"]
        consts = [input(f"Input const for {series_id[s]}:") for s in series_with_const]
        
        parameters = get_algorithm_params()
        
        x = input("Input x: ")
        n = input("Input n: ")
        order = input("Input order: ")
        
        params = [x, "6", n, order, *parameters]
        test_all_series_for_all_function(exec_file, params, consts, plot_dir, "output.txt")
    
    elif function_id == 3:
        plot_dir = os.path.join(testing_data_dir, "3")
        
        series = get_series_params(input("Choose series:"))
        parameters = get_algorithm_params()
        
        if series[0] != "103":
            print(f"{get_series_limits(int(series[0]))} for chosen series")
        else:
            print(f"{get_series_limits(int(series[1]))} for chosen series")
        
        print("to set x we will use np.linspace(start, stop, count)")
        start = int(input("Input start: "))
        stop = int(input("Input stop: "))
        step = int(input("Input x count: "))
        n = input("Input n: ")
        order = input("Input order: ")
        
        params = [series, "6", n, order, *parameters]
        test_x_for_all_transformations(exec_file, params, series, plot_dir, "output.txt", [start, stop, step])
    
    elif function_id == 4:
        plot_dir = os.path.join(testing_data_dir, "4")
        parameters = []
        
        with open("testing_parameters", 'r') as f:
            parameters = [line.split() for line in f]
        
        for i, params in enumerate(parameters):
            output = run_program_and_process_output(exec_file, params, "output.txt")
            
            parsed_params = find_params(params)
            log_selected_params(parsed_params)
            
            plot_file = os.path.join(plot_dir, generate_filename(params))
            work_with_data(parsed_params[3][0], "output.txt", plot_file)
    
    elif function_id == 5:
        series = get_series_params(input("Choose series:"))
        
        consts = []
        for trans_id in ["5", "6", "8", "10", "12", "13"]:
            print(f"\nChoose parameters for {transformations[trans_id]}")
            consts.append(get_transformation_params(trans_id))
        
        x = input(f"Input x: ")
        n = input("Input n: ")
        order = input("Input order: ")
        
        params = [series, x, n, order]
        test_speed(exec_file, params, consts, "output.txt")

selected_series = ['GDP', 'Inflation']
selected_transformations = ['log', 'diff']
selected_functions = ['ARIMA', 'LSTM']
x = 100
n = 20
order = 3

log_selected_params_new(
    selected_series=selected_series,
    transformations=selected_transformations,
    functions=selected_functions,
    x=x,
    n=n,
    order=order
)

if __name__ == '__main__':
    with Timer("Full Testing Pipeline Execution"):
        debug_logger.info("Main execution started")
    main()


    try:
        parser = LogParser("output_testing.log") 
        results = parser.get_results() 
        if results:
            debug_logger.info("Found logged Optimization Results in output_testing.log:")
            for res_idx, res_entry in enumerate(results):
                debug_logger.info(f"  Result {res_idx+1}: {res_entry}")
        else:
            debug_logger.info("No 'OPTIMIZATION_RESULTS' found by parser in output_testing.log.")

        all_stats = parser.get_all_statistics()
        if all_stats:
            debug_logger.info(f"Found {len(all_stats)} general statistics entries in output_testing.log.")
            for stat_entry in all_stats:
               debug_logger.debug(f"  Stat: {stat_entry}")
        
        critical = parser.get_critical_events()
        if critical:
            debug_logger.info(f"Found {len(critical)} critical event entries.")
            for crit_event in critical:
                debug_logger.warning(f"  Critical Event: {crit_event}")

    except FileNotFoundError:
        debug_logger.error("Log file 'output_testing.log' not found for parsing at the end.")
    except Exception as e:
        debug_logger.error(f"Error during final log parsing: {e}")