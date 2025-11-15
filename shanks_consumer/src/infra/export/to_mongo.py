from typing import Sequence

from pymongo.database import Database as MongoDatabase
from tqdm import tqdm

from src.domain.export_service import ExportService
from src.domain.trial_result import TrialResult
from src.infra.export.serializer import TrialResultSerializer


class MongoExportService(ExportService):
    def __init__(
        self,
        mongo_database: MongoDatabase,
        collection_name: str,
        batch_size: int = 1000,
    ):
        self.batch_size = batch_size
        self.mongo_database = mongo_database
        self.collection_name = collection_name

    def export(self, results: Sequence[TrialResult], **kwargs):
        collection = self.mongo_database.get_collection(self.collection_name)
        dicts = TrialResultSerializer.to_dict(results)

        with tqdm(
            total=len(dicts),
            desc=f"Exporting to MongoDB '{self.collection_name}'",
        ) as pbar:
            for i in range(0, len(dicts), self.batch_size):
                batch = dicts[i : i + self.batch_size]
                collection.insert_many(batch)
                pbar.update(len(batch))
