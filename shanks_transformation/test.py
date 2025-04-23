import subprocess
import os
import logging
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from datetime import datetime
from math import factorial


class program:
    def __init__(self, path, params):
        self.file_path = path
        self.cl_arguments = params
        self.output = ""
        self.stderr = ""

    def info(self):
        print("Executable file: ", self.file_path)
        print("Arguments, that we inputing in command line: ", self.cl_arguments)

    def run(self):
        process = subprocess.Popen(
            [self.file_path],  
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

class TextFileLogger:
    def __init__(self, input_file='output.txt', log_file='data.log'):
        self.input_file = Path(input_file)
        self.log_file = Path(log_file)
        
        self.setup_logger()
        
    def setup_logger(self):
        self.logger = logging.getLogger('TextFileLogger')
        self.logger.setLevel(logging.INFO)
        
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        
        file_handler = logging.FileHandler(self.log_file)
        file_handler.setFormatter(formatter)
        
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)
        
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        self.logger.info("Logger initialized successfully")

    def read_input_file(self):
        try:
            with open(self.input_file, 'r') as f:
                data = [line.strip() for line in f if line.strip()]
            self.logger.info(f"Read {len(data)} lines from {self.input_file}")
            return data
        except FileNotFoundError:
            self.logger.error(f"Input file {self.input_file} not found")
            return []
        except Exception as e:
            self.logger.error(f"Error reading input file: {str(e)}")
            return []

    def log_message(self, message):
        if not message:
            self.logger.warning("No data to log")
            return False
        try:
            with open(self.log_file, 'a') as f:
                timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                f.write(f"{timestamp} - {message}\n")
                self.logger.info(f"Logged message to {self.log_file}")
                return True
        except Exception as e:
            self.logger.error(f"Error writing to log file: {str(e)}")
            return False

    def log_error(self, error_message):
        if not error_message:
            self.logger.warning("Attempted to log empty error message")
            return False
        
        try:
            self.logger.error(error_message)
            return True
        except Exception as e:
            print(f"Critical logging failure: {str(e)}")
            return False

    def log_data(self, data):
        if not data:
            self.logger.warning("No data to log")
            return False
        
        try:
            with open(self.log_file, 'a') as f:
                timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                for item in data:
                    f.write(f"{timestamp} - {item}\n")
            self.logger.info(f"Logged {len(data)} items to {self.log_file}")
            return True
        except Exception as e:
            self.logger.error(f"Error writing to log file: {str(e)}")
            return False

    def plot_data(self, data=None, output_plot='plot.png'):
        try:
            S_n = []
            Transformation_result = []
            for item in data:
                a, b = item.split()
                S_n.append(float(a))
                Transformation_result.append(float(b))
            x = list(range(1, len(S_n) + 1))
            
            if not S_n:
                self.logger.awarning("No numeric data to plot")
                return False
            
            plt.figure(figsize=(10, 5))
            plt.plot(x, S_n, marker='o', linestyle='-', color='blue', label='S_n')
            plt.plot(x, Transformation_result, marker='s', linestyle='--', color='red', label='Transformation_result')
            
            # Настройка оформления
            plt.title(f"Data Visualization - {datetime.now().strftime('%Y-%m-%d')}")
            plt.xlabel("Index (x)")
            plt.ylabel("Values")
            plt.grid(True)
            plt.legend()
            
            plt.savefig(output_plot)
            plt.close()
            self.logger.info(f"Plot saved to {output_plot}")
            return True
        except Exception as e:
            self.logger.error(f"Error plotting data: {str(e)}")
            return False
    

    def process(self):
        data = self.read_input_file()
        if data:
            self.log_data(data)
            self.plot_data(data)
        return data

def exp_series(x, n):
    arr = []
    for i in range(n):
        a_n = x**i / factorial(i)
        arr.append(f"{a_n}")
    return arr


def main():
    exec_program = ""
    if (os.name == "Windows"):
        exec_program = "/build/test.exe" # Путь к exe файлу
    else:
        exec_program = "./build/test"
    #a = program(exec_program, ["-1", "100"] + exp_series(1, 101) +  ["18"])
    a = program(exec_program, ["1", "1", "18", "10", "1"])
    a.run()
    output_message = a.get_output()
    error_message = a.get_error()
    if (error_message):
        logger.log_error(error_message)
    
    logger = TextFileLogger(input_file='output.txt', log_file='output.log')
    processed_data = logger.process()
    print(f"Processed {len(processed_data)} items")



if __name__ == "__main__":
    main()