import logging

from pymongo import MongoClient
from pymongo.database import Database as MongoDatabase
from pymongo.errors import PyMongoError

from src.config import MongoConfig


def setup_mongo_db(config: MongoConfig) -> MongoDatabase | None:
    mongo_database = None
    logging.info("MongoDB export specified, attempting to connect...")
    try:
        mongo_database = MongoClient(
            host=config.mongo_host,
            port=config.mongo_port,
            serverSelectionTimeoutMS=5000,
            connectTimeoutMS=5000,
            username=config.mongo_username,
            password=config.mongo_password,
            authSource=config.mongo_auth_source,
        ).get_database(config.mongo_database)
        if mongo_database is not None:
            mongo_database.client.admin.command("ping")
    except PyMongoError:
        logging.error("Connection to MongoDB failed, skipping this option")
        return None
    else:
        logging.info("Connection to MongoDB successful")
    return mongo_database