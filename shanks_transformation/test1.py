from test import *

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
    for num_series in range(len(series_id)-2):
        if num_series + 1 in [6, 11, 16, 75, 95, 102]:
            parameters=[str(num_series), consts[k], *params]
        else:
            parameters=[str(num_series), *params]
        print(parameters)
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
    
    print(lim_array)
    return



def main():
    current_dir = os.path.dirname(os.path.abspath(__file__))
    log_file = os.path.join(current_dir, "output.log")

    logger = Logger(log_file)

    testing_data_dir = 'output_testing_data'
    if not os.path.exists(testing_data_dir):
        os.makedirs(testing_data_dir)
        logger.log(f"New directory has been created - {testing_data_dir}")
    plot_dir = os.path.join(current_dir, testing_data_dir)


    exec_file = ""
    if (os.name == "nt"):
        exec_file = "/build/test.exe" # Путь к exe файлу
    else:
        exec_file = "./build/test"

    function_txt = '''
        Choose function:\n
        1)test_all_series(cmp_sum_and_transform)\n
        2)
        3)
        4)
    '''

    print(function_txt)
    function_id = int(input())
    if function_id == 1:
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
        pass
    else:
        print("wrong function_id")

    
    logger.log("------------------------------------------")




if __name__ == "__main__":
    main()