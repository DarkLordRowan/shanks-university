FROM python:3.13-slim-bookworm AS shanks-builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    cmake \
    ninja-build \
    gcc \
    libeigen3-dev  \
    libmpfrc++-dev \
    libgsl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY ./cmake ./cmake/
COPY ./bindings ./bindings/
COPY ./core ./core/

RUN cmake -S ./bindings -B ./build -DSHANKS_ENABLE_PROFILING=ON -G Ninja
RUN mkdir -p pyshanks
RUN touch pyshanks/__init__.py && echo 'from .pyshanks import *' > pyshanks/__init__.py
RUN cmake --build build --target pyshanks -j$(nproc)
RUN cp build/*.so pyshanks

FROM python:3.13-slim-bookworm

ENV PYTHONUNBUFFERED=1

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgslcblas0 \
    libmpfr6 \
    libstdc++6 \
    libgsl27 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY runner/config/               ./config
COPY runner/src/                  ./src
COPY runner/main.py               ./
COPY runner/verify_parquet_fix.py ./

COPY --from=shanks-builder /pyshanks/ ./pyshanks/

CMD ["/bin/bash"]
