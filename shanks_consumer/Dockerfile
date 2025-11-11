FROM python:3.13-slim-bookworm as shanks-builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    make \
    gcc \
    git \
    g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY shanks_transformation/ ./shanks_transformation

RUN cmake -S shanks_transformation

RUN make pyshanks

FROM python:3.13-slim-bookworm as prod

RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    libgl1 \
    libglib2.0-0 \
    libxcb-* \
    libxkbcommon-x11-0 \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

ENV PYTHONUNBUFFERED=1

WORKDIR /app

COPY shanks_consumer/requirements.txt ./

RUN --mount=type=cache,target=/root/.cache/pip \
    pip install --no-cache-dir -r requirements.txt

COPY shanks_consumer/ ./

COPY --from=shanks-builder /build/*.so ./pyshanks/

CMD ["/bin/bash"]