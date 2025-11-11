FROM python:3.13-slim AS runtime
ENV PYTHONUNBUFFERED=1 PYTHONDONTWRITEBYTECODE=1

WORKDIR /app

COPY apps/gateway/ /app/gateway/
RUN if [ -f /app/gateway/requirements.txt ]; then \
      pip install --no-cache-dir -r /app/gateway/requirements.txt; \
    fi

WORKDIR /app/gateway
ENV PORT=8000
EXPOSE 8000

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
