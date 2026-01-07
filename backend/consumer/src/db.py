"""
MongoDB database setup utility.
Author: Yadrentsev I. M.
"""

import logging

from pymongo import MongoClient
from pymongo.database import Database as MongoDatabase
from pymongo.errors import PyMongoError

from src.config.model import MongoConfig


def setup_mongo_db(config: MongoConfig) -> MongoDatabase | None:
    """Setups MongoDB connection.

    :param config: MongoDB configuration
    :type config: MongoConfig
    :return: MongoDB database instance or None if connection failed
    :rtype: MongoDatabase | None
    """
    mongo_database = None
    logging.info("MongoDB export specified, attempting to connect...")
    try:
        mongo_database = MongoClient(
            host=config.host,
            port=config.port,
            serverSelectionTimeoutMS=5000,
            connectTimeoutMS=5000,
            username=config.username,
            password=config.password,
            authSource=config.auth_source,
        ).get_database(config.database)
        if mongo_database is not None:
            mongo_database.client.admin.command("ping")
    except PyMongoError:
        logging.error("Connection to MongoDB failed, skipping this option")
        return None
    else:
        logging.info("Connection to MongoDB successful")
    return mongo_database
