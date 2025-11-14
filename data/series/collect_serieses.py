import os
import json

project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

algorithm_folder = os.path.join(project_root, "series", "serieses")

output_file = os.path.join(project_root, "series", "serieses.json")

def collect_serieses():
    algorithms = []

    for filename in os.listdir(algorithm_folder):
        if filename.endswith(".json"):
            file_path = os.path.join(algorithm_folder, filename)
            with open(file_path, "r", encoding="utf-8") as file:
                try:
                    data = json.load(file)
                    algorithms.append(data)
                except json.JSONDecodeError:
                    print(f"Error decoding JSON in file {filename}")

    with open(output_file, "w", encoding="utf-8") as output:
        json.dump(algorithms, output, ensure_ascii=False, indent=4)

    print(f"All serieses have been collected into {output_file}")

if __name__ == "__main__":
    collect_serieses()
