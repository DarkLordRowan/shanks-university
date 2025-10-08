# scripts/dev_setup.py
import os, sys, sysconfig, shutil, subprocess, glob, platform, pathlib

ROOT = pathlib.Path(__file__).resolve().parents[1]
CXX_SRC = ROOT / "shanks_transformation"
BUILD = ROOT / f"build-py{sys.version_info.major}{sys.version_info.minor}"
PYEXE = sys.executable
PURELIB = pathlib.Path(sysconfig.get_paths()["purelib"])

def run(cmd, cwd=None):
    print("+", " ".join(cmd))
    subprocess.check_call(cmd, cwd=cwd)

def ensure_tools():
    if shutil.which("cmake") is None:
        raise RuntimeError("Не найден cmake в PATH")

def decide_generator_args():
    # Явное переопределение через окружение
    gen_env = os.getenv("CMAKE_GENERATOR")
    if gen_env:
        # пример: set CMAKE_GENERATOR=Visual Studio 17 2022
        return ["-G", gen_env] + (["-A", "x64"] if "Visual Studio" in gen_env and platform.system()=="Windows" else [])
    # По умолчанию:
    if platform.system() == "Windows":
        # Надёжный путь — MSVC из VS2022
        return ["-G", "Visual Studio 17 2022", "-A", "x64"]
    # *nix: если есть ninja — используем его, иначе дефолтный генератор
    if shutil.which("ninja"):
        return ["-G", "Ninja"]
    return []

def cmake_gen():
    args = [
        "cmake",
        "-S", str(CXX_SRC),
        "-B", str(BUILD),
        f"-DPython3_EXECUTABLE={PYEXE}",
        "-DCMAKE_BUILD_TYPE=Release",
    ] + decide_generator_args()
    try:
        run(args)
    except subprocess.CalledProcessError as e:
        if platform.system() == "Windows":
            msg = (
                "CMake не смог найти C++ компилятор.\n"
                "Проверьте установку 'Build Tools for Visual Studio 2022' с компонентами MSVC x64 и CMake.\n"
                "Альтернатива: задайте CMAKE_GENERATOR=Visual Studio 17 2022.\n"
            )
            print(msg, file=sys.stderr)
        raise e

def cmake_build():
    # Для VS-генератора нужна конфигурация; для Ninja — игнорируется
    run(["cmake", "--build", str(BUILD), "--config", "Release", "--target", "pyshanks"])

def install_pyshanks():
    patterns = ["**/pyshanks*.so", "**/pyshanks*.pyd"]
    found = []
    for p in patterns:
        found += glob.glob(str(BUILD / p), recursive=True)
    if not found:
        raise RuntimeError("pyshanks*.so/.pyd не найден в build-дереве")
    lib = max(found, key=len)
    dst = PURELIB / pathlib.Path(lib).name
    shutil.copy2(lib, dst)
    print(f"Установлен: {dst}")

def install_consumer_editable():
    run([PYEXE, "-m", "pip", "install", "--upgrade", "pip", "build", "wheel", "setuptools"])
    run([PYEXE, "-m", "pip", "install", "-e", str(ROOT / "shanks_consumer")])

def verify():
    run([PYEXE, "-c", "import pyshanks, src; print('ok:', pyshanks.__name__, src.__name__)"])

if __name__ == "__main__":
    ensure_tools()
    BUILD.mkdir(parents=True, exist_ok=True)
    cmake_gen()
    cmake_build()
    install_pyshanks()
    install_consumer_editable()
    verify()
    print("DONE")
