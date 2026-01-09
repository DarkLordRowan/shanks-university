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
COPY ./lib ./lib/
COPY ./src ./src/
COPY ./consumer ./consumer/
WORKDIR /build/consumer

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

COPY consumer/ ./

COPY --from=shanks-builder /build/output/*.so ./consumer/pyshanks/pyshanks.so
RUN mkdir -p /opt/consumer/pyshanks && cp ./consumer/pyshanks/pyshanks.so /opt/consumer/pyshanks/pyshanks.so

CMD ["/bin/bash"]
