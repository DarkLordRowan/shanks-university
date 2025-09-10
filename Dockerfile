# ---- builder: собираем нативное расширение ----
FROM python:3.13-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY shanks_transformation/ ./shanks_transformation/
RUN cmake -S shanks_transformation -B build -DCMAKE_BUILD_TYPE=Release \
      -DPython3_EXECUTABLE="$(which python)" \
 && cmake --build build --config Release --target pyshanks

# (на Linux это .so; добавил .pyd на всякий случай)
RUN mkdir -p /wheel && \
    (find build -name "pyshanks*.so"  -exec cp -v {} /wheel/ \; || true) && \
    (find build -name "pyshanks*.pyd" -exec cp -v {} /wheel/ \; || true)

# ---- runtime: минимальный образ для запуска FastAPI ----
FROM python:3.13-slim
ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1
WORKDIR /app
COPY backend/requirements.txt ./backend/requirements.txt
RUN python -m pip install --no-cache-dir -r backend/requirements.txt
COPY backend/ ./backend/
COPY --from=builder /wheel/* ./backend/
ENV PORT=8000
EXPOSE 8000
CMD ["uvicorn", "backend.main:app", "--host", "0.0.0.0", "--port", "8000"]
