import subprocess
import os
import logging
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from datetime import datetime
from math import factorial


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
    "104": "ArraySeries",
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
    0: '#00FF00',  # green
    1: '#FF0000',  # red
    2: '#0000FF',  # blue
    3: '#800080',  # purple
    4: '#FFA500',  # orange
    5: '#00FFFF',  # cyan
    6: '#FF00FF',  # magenta
    7: '#00FF00',  # lime
    8: '#FFC0CB',  # pink
    9: '#008080',  # teal
    10: '#E6E6FA', # lavender
    11: '#A52A2A', # brown
    12: '#F5F5DC', # beige
    13: '#800000', # maroon
    14: '#808000', # olive
    15: '#000080', # navy
    16: '#000000', # black
    17: '#808080', # gray
    18: '#FFFF00', # yellow
    19: '#87CEEB', # skyblue
    20: '#FFD700', # gold
    21: '#EE82EE', # violet
    22: '#FF7F50', # coral
    23: '#4B0082', # indigo
    24: '#40E0D0', # turquoise
    25: '#FA8072', # salmon
    26: '#006400', # darkgreen
    27: '#F0E68C', # khaki
    28: '#DA70D6', # orchid
    29: '#4682B4',  # steelblue
    30: '#575499', 
    31: '#012940',  
    32: '#111234', 

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
            text=True,
            #bufsize=1, 
            #universal_newlines=True
        )

        for line in self.cl_arguments:
            process.stdin.write(line + "\n")
            process.stdin.flush()
            #print(process.stdout.readline())
        self.output, self.stderr = process.communicate()

    def get_output(self):
        return self.output

    def get_error(self):
        return self.stderr

class Logger:
    def __init__(self, log_file='output.log'):
        self.log_file = Path(log_file)
        self._setup_logger()
        
    def _setup_logger(self):
        self.logger = logging.getLogger('Logger')
        self.logger.setLevel(logging.INFO)
        
        formatter = logging.Formatter('%(asctime)s - %(message)s')
        
        file_handler = logging.FileHandler(self.log_file)
        file_handler.setFormatter(formatter)
        
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)
        
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        self.logger.info("------------------------------------------")
        self.logger.info("Logger initialized")


    def log(self, value):
        try:
            self.logger.info(str(value))
            return True
        except Exception as e:
            print(f"Logging error: {str(e)}")
            return False

    def log_error(self, value):
        try:
            self.logger.error(str(value))
            return True
        except Exception as e:
            print(f"Logging error: {str(e)}")
            return False                                                                                                                                                                                                                                                                                                                                    

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


def find_params(params):
    series = [params[0]]
    x = [params[1]]
    transformation = []
    function = []
    n = []
    order = []

    if params[0] == "103":
        series.append(params[2])
        if params[2] in ["6", "11", "16", "75", "95", "102"]:
            series.append(params[3])
    elif params[0] in ["6", "11", "16", "75", "95", "102"]:
        series.append(params[2])
    a = len(series) + 1
    transformation.append(params[a])
    if params[a] in ["5", "6", "8"]:
        transformation.append(params[a + 1])
        transformation.append(params[a + 2])
    elif params[a] in ["10"]:
        transformation.append(params[a + 1])
        if params[a+1] == "1":
            transformation.append(params[a + 2])
        if params[a+1] == "2":
            transformation.append(params[a + 2])
            transformation.append(params[a + 3])
    elif params[a] in ["13"]:
        transformation.append(params[a + 1])

    a += len(transformation)

    function.append(params[a])
    n.append(params[a+1])
    order.append(params[a+2])

    if params[a] == "4":
        function.append(params[a+3])
        if params[a+3] in ["5", "6", "8"]:
            function.append(params[a + 4])
            function.append(params[a + 5])
        elif params[a+3] in ["10"]:
            function.append(params[a + 4])
            if params[a+4] == "1":
                function.append(params[a + 5])
            if params[a+4] == "2":
                function.append(params[a + 5])
                function.append(params[a + 6])
        elif params[a+3] in ["13"]:
            function.append(params[a + 4])
    elif params[a] == "6":
        function.append(params[a+1])
        function.append(params[a+2])
        function.append(params[a+3])
        function.append(params[a+4])
        function.append(params[a+5])
        function.append(params[a+6])

    return [series, x, transformation, function, n, order]


def work_with_data(logger, function_id, file, plot_file):
    if function_id == "1":
        S_n = []
        T_n = []
        lim = 0
        try:
            logger.log(f"Values of file:")
            with open(file, 'r') as f:
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
        
        logger.log(f"plot saved to {plot_file}")

    if function_id == "2":
        a_n = []
        t_n = []
        diff = []
        try:
            logger.log(f"Values of file:")
            with open(file, 'r') as f:
                i = 1
                while True:
                    line = f.readline().strip()
                    if not line:
                        break
                    try:
                        a_n.append(float(line))
                        logger.log(f"a_{i}: {a_n[i-1]}")
                    except:
                        a_n.append(None)
                        logger.log(f"a_{i}: {line}(replaced by None)")

                    line = f.readline().strip()

                    try:
                        t_n.append(float(line))
                        logger.log(f"t_{i}: {t_n[i-1]}")
                    except:
                        t_n.append(None)
                        logger.log(f"t_{i}: {line}(replaced by None)")

                    line = f.readline().strip()

                    try:
                        diff.append(float(line))
                        logger.log(f"t_{i} - a{i}: {diff[i-1]}")
                    except:
                        diff.append(None)
                        logger.log(f"t_{i} - a{i}: {line}(replaced by None)")

                    i += 1
        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")

    if function_id == "3":
        diff = []
        try:
            logger.log(f"Values of file:")
            with open(file, 'r') as f:
                i = 1
                for line in f:
                    try:
                        diff.append(float(line.strip()))
                        logger.log(f"S-T_{i}: {diff[i-1]}")
                    except:
                        diff.append(None)
                        logger.log(f"S-T_{i}: {line.strip()}(replaced by None)")
                    i += 1
        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")

    if function_id == "4":
        first_transform = []
        second_transform = []
        try:
            with open(file, 'r') as f:
                i = 1
                while True:
                    line = f.readline().strip()
                    if not line:
                        break
                    try:
                        first_transform.append(float(line))
                        logger.log(f"first_transformation_{i}: {first_transform[i-1]}")
                    except:
                        first_transform.append(None)
                        logger.log_error(f"first_transformation_{i}: {line}(replaced by None)")

                    line = f.readline().strip()

                    try:
                        second_transform.append(float(line))
                        logger.log(f"second_transformation_{i}: {second_transform[i-1]}")
                    except:
                        second_transform.append(None)
                        logger.log(f"second_transformation_{i}: {line}(replaced by None)")
                    i += 1

                    try:
                        if first_transform[-1] > second_transform[-1]:
                            logger.log(f"First transformation faster")
                        else:
                            logger.log(f"Second transformation faster")
                    except:
                        logger.log(f"Can not compare first and second transformation")

        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")
            
    if function_id == "5":
        time = ''
        try:
            logger.log(f"Values of file:")
            with open(file, 'r') as f:
                time = f.readline()
                logger.log(f"execution took {time} milliseconds")
        except Exception as e:
            logger.log_error(f"Error reading from file: {str(e)}")
    
    if function_id == "6":
        S_n = []
        T_n = [[] for i in range(34)]
        lim = 0
        try:
            logger.log(f"Values of file:")
            with open(file, 'r') as f:
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
        
        logger.log(f"plot saved to {plot_file}")


def main():
    exec_file = ""
    if (os.name == "nt"):
        exec_file = "/test.exe" # Путь к exe файлу
    else:
        exec_file = "./build/test"
    params = ["1", "1", "4", "6", "10", "1", "1", "1", "1", "1", "1", "1"]
    #params = ["15", "3", "10", "0", "1", "10", "2"]
    #params = ["15", "3", "4", "4", "10", "2", "18"]


    program = Program(exec_file, params)
    program.run()

    error_message = program.get_error()
    if (error_message):
        logger.log_error(error_message)

    current_dir = os.path.dirname(os.path.abspath(__file__))

    plot_file = os.path.join(current_dir, "plot.png")
    log_file = os.path.join(current_dir, "output.log")

    logger = Logger(log_file)

    a = find_params(params)
    log_selected_params(logger, a)

    work_with_data(logger, a[3][0], "output.txt", plot_file)
    
    logger.log("------------------------------------------")


'''
Руководство по пользованию(Вводим так де как и на c++). Надо сделать параметры которые будут передаваться в программу.

1)Выбираем ряд(первое значение)
2)Вводим x(второе значение)
1.1)Если выбрали ряд под номерами ["6", "11", "16", "75", "95", "102"]. То для них надо ввести дополнительный параметр.
1.2)Если же выбрали ряд 103, то дальше нужно выбрать ряд, от который он будет модифицировать(в нашем случае добавлять шум). Смотрит пункт 3
4)Выбираем трансформацию.
4.1)Если выбрали трансформацию под номерами ["5", "6", "8"]. То надо выбрать какую трансформацию использовать(u,t,d,v) и выбрать является ли она рекурсивной(0, 1)
4.2)Если выбрана трансформация 10, то надо выбрать параметр Beta.
4.3)Если выбрана трансформация 13, то надо выбрать какую использовать 0 - classic(не требует доп параметров), 1 - gamma(требует доп параметр), 2 - gamma-rho(требует 2 доп параметра)
5)Выбираем функцию
6)Вводим n
7)Вводим order
5.1)Если выбрана функция 4, требуется написать трансформацию, с которой мы будем сравнивать(рассмотреть случаи 4.1-4.3)
5.2)Если выбрана функция 6, требуется написать 6 раз(для каждого алгоритма из [beta_Levin_S_algorithm, gamma_Levin_M_algorithm, gamma_rho_Wynn_algorithm, RHO_rho_Wynn_algorithm, beta_levin_recursion_algorithm, epsilon_algorithm_3] написать его параметр) следующий выбор: сли допишем 1, то автоматически выберется стандартное значение параметра, если 0, тонадо будет написать значение параметра
'''

if __name__ == "__main__":
    main()