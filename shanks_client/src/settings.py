import os

from pydantic import BaseModel, Field


class MongoConfig(BaseModel):
    host: str = Field(default_factory=lambda: os.getenv("MONGO_HOST", "localhost"))
    port: int = Field(default_factory=lambda: int(os.getenv("MONGO_PORT", "27017")))
    username: str | None = Field(default_factory=lambda: os.getenv("MONGO_USERNAME"))
    password: str | None = Field(default_factory=lambda: os.getenv("MONGO_PASSWORD"))
    database: str = "trial_db"
    collection: str = "trial_results"
    auth_source: str = "admin"

    @property
    def mongo_url(self):
        return (
            f"mongodb://{self.username}:{self.password}@{self.host}:{self.port}/?authSource={self.auth_source}"
            if self.username and self.password
            else f"mongodb://{self.host}:{self.port}"
        )


settings = MongoConfig()
