# Установка на Linux

## Обзор

Пошаговая инструкция установки фреймворка на Linux.

---

## Шаг 1. Установка системных зависимостей

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    g++ \
    git \
    libgmp-dev \
    libmpfr-dev \
    libeigen3-dev \
    pkg-config
```

### Fedora/RHEL

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    gmp-devel \
    mpfr-devel \
    eigen3-devel \
    pkg-config
```

### Arch Linux

```bash
sudo pacman -S --needed \
    gcc \
    cmake \
    git \
    gmp \
    mpfr \
    eigen3 \
    pkg-config
```

---

## Шаг 2. Клонирование репозитория

```bash
git clone git@github.com:DarkLordRowan/shanks-university.git
cd shanks-university
```

---

## Шаг 3. Сборка pyshanks (C++ extension)

```bash
cd backend/runner
./install_pyshanks.sh
```

**Скрипт выполняет:**
1. Компиляцию pyshanks через CMake
2. Создание директории `pyshanks/`
3. Копирование скомпилированного `.so` файла

**Ожидаемый вывод:**
```
BUILDING PYSHANKS...
PREPARING FOLDER FOR PYTHON...
CLEANING...
```

### Ручная сборка (если скрипт не работает)

```bash
cd backend/bindings
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

Скопируйте `*.so` в `backend/runner/pyshanks/`:

```bash
mkdir -p ../../runner/pyshanks
cp *.so ../../runner/pyshanks/
```

---

## Шаг 4. Проверка установки

```bash
cd backend/runner
python -c "import pyshanks as ps; print(ps.__version__)"
```

Успешная установка не выдаёт ошибок.

---

## Шаг 5. Установка опциональных зависимостей

### Для CSV экспорта

```bash
pip install pandas
```

### Для Parquet экспорта

```bash
pip install pyarrow
```

### Для MongoDB

```bash
pip install pymongo
```

### Для REST API (Frontend Client)

```bash
cd frontend/client
pip install fastapi uvicorn pymongo
```

### Для Vizr

```bash
pip install PyQt6 pyqtgraph polars
```
