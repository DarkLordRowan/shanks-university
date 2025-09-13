FROM python:3.13-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build git \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY shanks_transformation/ ./shanks_transformation/
RUN cmake -S shanks_transformation -B build -DCMAKE_BUILD_TYPE=Release -DPython3_EXECUTABLE="$(which python)" \
 && cmake --build build --config Release --target pyshanks

RUN mkdir -p /artifacts/pyshanks \
 && (find build -type f -name "pyshanks*.so"  -exec cp -v {} /artifacts/pyshanks/ \; || true) \
 && (find build -type f -name "pyshanks*.pyd" -exec cp -v {} /artifacts/pyshanks/ \; || true)

COPY shanks_consumer/pyproject.toml shanks_consumer/README.md ./shanks_consumer/
COPY shanks_consumer/src ./shanks_consumer/src
RUN python -m pip install --upgrade pip build \
 && python -m build --wheel --outdir /artifacts/wheels ./shanks_consumer


# --------------------
FROM python:3.13-slim AS runtime
ENV PYTHONUNBUFFERED=1 PYTHONDONTWRITEBYTECODE=1

RUN apt-get update && apt-get install -y --no-install-recommends libstdc++6 \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /artifacts/pyshanks/ /tmp/pyshanks/
RUN pysp="$(python -c "import sysconfig; print(sysconfig.get_paths()['purelib'])")" \
 && mkdir -p "$pysp" \
 && cp -v /tmp/pyshanks/pyshanks*.* "$pysp/"

COPY --from=builder /artifacts/wheels/ /tmp/wheels/
RUN pip install --no-cache-dir /tmp/wheels/*.whl \
 && rm -rf /tmp/*

COPY apps/worker/ /app/worker/
RUN if [ -f /app/worker/requirements.txt ]; then \
      pip install --no-cache-dir -r /app/worker/requirements.txt; \
    fi

WORKDIR /app/worker
ENV PORT=8000
EXPOSE 8000
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
