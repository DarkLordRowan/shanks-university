from test import *


def test_all_series_for_1_function(exec_file, params):
    program = Program(exec_file, params)
    program.run()



def main():
    exec_file = ""
    if (os.name == "nt"):
        exec_file = "/build/test.exe" # Путь к exe файлу
    else:
        exec_file = "./build/test"
    params = ["1", "1", "17", "6", "10", "2", "1", "1", "1", "1", "1", "1"]

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




if __name__ == "__main__":
    main()