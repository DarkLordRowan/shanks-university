# ---- builder: собираем нативное расширение ----
FROM python:3.13-slim AS builder
RUN apt-get update && apt-get install -y --no-install-recommends build-essential cmake git \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY shanks_transformation/ ./shanks_transformation/
RUN cmake -S shanks_transformation -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --config Release
RUN mkdir -p /wheel && cp -v build/*.so /wheel/ 2>/dev/null || true

# ---- runtime: только то, что нужно для запуска ----
FROM python:3.13-slim
WORKDIR /app
COPY backend/requirements.txt ./backend/requirements.txt
RUN python -m pip install --no-cache-dir -r backend/requirements.txt
COPY backend/ ./backend/
COPY --from=builder /wheel/* ./backend/
ENV PORT=8000
EXPOSE 8000
CMD ["uvicorn", "backend.main:app", "--host", "0.0.0.0", "--port", "8000"]
