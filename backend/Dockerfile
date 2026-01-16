FROM python:3.13-slim-bookworm AS shanks-builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    make \
    g++ \
    git \
    libgsl-dev \
    libeigen3-dev \
    libmpfr-dev \
    libgmp-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY ./cmake ./cmake/
COPY ./bindings ./bindings/
COPY ./core ./core/
COPY ./runner ./runner/
WORKDIR /build/runner

RUN chmod +x install_pyshanks.sh \
    && ./install_pyshanks.sh \
    && mkdir -p /build/output \
    && cp -v pyshanks/*.so /build/output/ || true

FROM python:3.13-slim-bookworm

ENV PYTHONUNBUFFERED=1

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgslcblas0 \
    libmpfr6 \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY runner/ ./

COPY --from=shanks-builder /build/output/*.so ./pyshanks/pyshanks.so
RUN mkdir -p /opt/runner/pyshanks && cp ./pyshanks/pyshanks.so /opt/runner/pyshanks/pyshanks.so

CMD ["/bin/bash"]
