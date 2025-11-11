FROM python:3.13-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build git gcc-12 g++-12 \
 && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100 \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY shanks_transformation/ ./shanks_transformation/

RUN sed -E -i \
  -e 's/0\s*==\s*dig\[i\]/!static_cast<bool>(dig[i])/g' \
  -e 's/1\s*==\s*dig\[i\]/static_cast<bool>(dig[i])/g' \
  shanks_transformation/libs/arbitrary_arithmetics/precisioncore.cpp || true

RUN cmake -S shanks_transformation -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --config Release --target pyshanks

RUN mkdir -p /artifacts/pyshanks \
 && (find build -type f -name "pyshanks*.so"  -exec cp -v {} /artifacts/pyshanks/ \; || true) \
 && (find build -type f -name "pyshanks*.pyd" -exec cp -v {} /artifacts/pyshanks/ \; || true)

RUN mkdir -p /tmp/fpnan /artifacts/lib \
 && printf '%s\n' \
   '#include <limits>' \
   'extern "C" double FP_QUIET_NAN = std::numeric_limits<double>::quiet_NaN();' \
   > /tmp/fpnan/fp_nan_shim.cpp \
 && g++ -shared -fPIC -O2 -std=gnu++20 -o /artifacts/lib/libfpnan.so /tmp/fpnan/fp_nan_shim.cpp

RUN mkdir -p /artifacts/lib \
 && echo "Build libarb.so from libs/arbitrary_arithmetics/*.cpp" \
 && objlist="" && for f in $(find shanks_transformation/libs/arbitrary_arithmetics -maxdepth 1 -type f -name '*.cpp' | sort); do \
      echo "  -> $f"; \
      g++ -fPIC -O2 -std=gnu++20 -I shanks_transformation -c "$f" -o "/tmp/$(basename "$f").o"; \
      objlist="$objlist /tmp/$(basename "$f").o"; \
    done \
 && g++ -shared -o /artifacts/lib/libarb.so $objlist

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

COPY --from=builder /artifacts/lib/libfpnan.so /usr/local/lib/libfpnan.so
COPY --from=builder /artifacts/lib/libarb.so   /usr/local/lib/libarb.so
ENV LD_PRELOAD=/usr/local/lib/libfpnan.so:/usr/local/lib/libarb.so

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
