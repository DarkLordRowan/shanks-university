# Установка через Docker

## Обзор

Docker обеспечивает изолированное и воспроизводимое окружение для запуска фреймворка.

---

## Предварительные требования

- Docker 20.10+
- Docker Compose 2.0+

**Проверка:**
```bash
docker --version
docker compose version
```

---

## Структура Docker Compose

**Файл:** `docker-compose.yml` (в корне проекта)

```yaml
services:
  shanks-consumer:         # Backend consumer
  shanks-consumer-mongodb: # MongoDB
  shanks-client:           # FastAPI REST API
```

### Сервисы

| Сервис | Образ | Порт | Описание |
|--------|-------|------|----------|
| `shanks-consumer` | shanks-consumer | - | Запуск экспериментов |
| `shanks-consumer-mongodb` | mongo:latest | 27017 | Хранение результатов |
| `shanks-client` | shanks-client | 8080 | REST API |

---

## Шаг 1. Создание .env файла

**Файл:** `.env` (в корне проекта)

```bash
# MongoDB
MONGO_HOST=shanks-consumer-mongodb
MONGO_PORT=27017
MONGO_USERNAME=admin
MONGO_PASSWORD=secret_password

# База данных
MONGO_DATABASE=trials
MONGO_COLLECTION=results
MONGO_AUTH_SOURCE=admin
```

---

## Шаг 2. Сборка образов

```bash
docker compose build
```

**Собираемые образы:**
1. `shanks-consumer` — из `backend/Dockerfile`
2. `shanks-client` — из `frontend/client/Dockerfile`

---

## Шаг 3. Запуск контейнеров

### Все сервисы

```bash
docker compose up -d
```

### Отдельные сервисы

```bash
# Только MongoDB
docker compose up -d shanks-consumer-mongodb

# MongoDB + Consumer
docker compose up -d shanks-consumer-mongodb shanks-consumer

# MongoDB + Client
docker compose up -d shanks-consumer-mongodb shanks-client
```

---

## Шаг 4. Запуск эксперимента в контейнере

```bash
docker compose exec shanks-consumer python -m src run \
    --config /app/config/options.json
```

### Volume mounts

Конфигурационные файлы и результаты доступны на хост-машине:

| Хост | Контейнер | Описание |
|------|-----------|----------|
| `./backend/runner/config/` | `/app/config` | Конфигурации |
| `./backend/runner/output/` | `/app/output` | Результаты |

### Пример с выводом результатов

```bash
# Запуск
docker compose exec shanks-consumer python -m src run \
    --config /app/config/example.json

# Просмотр результатов
ls -la backend/runner/output/
```
