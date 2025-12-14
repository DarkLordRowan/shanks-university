#!/bin/bash
set -e

DB_NAME="trial_db"
COLLECTION_NAME="trial_results"

mongosh <<EOF
use $DB_NAME

if (db.getCollectionInfos({ name: "$COLLECTION_NAME" }).length === 0) {
    db.createCollection("$COLLECTION_NAME")
}

db["$COLLECTION_NAME"].createIndex({ "series.name": 1 }, { name: "series_name_idx", background: true })
db["$COLLECTION_NAME"].createIndex({ "accel.name": 1 }, { name: "accel_name_idx", background: true })
db["$COLLECTION_NAME"].createIndex({ "stack_id": 1 }, { name: "stack_id_idx", background: true })
db["$COLLECTION_NAME"].createIndex({ "error": 1 }, { name: "error_idx", background: true, sparse: true })
db["$COLLECTION_NAME"].createIndex({ "series.name": 1, "created_at": -1 }, { name: "series_name_created_at_idx", background: true })
db["$COLLECTION_NAME"].createIndex({ "accel.name": 1, "created_at": -1 }, { name: "accel_name_created_at_idx", background: true })
db["$COLLECTION_NAME"].createIndex({ "computed.n": 1 }, { name: "computed_n_idx", background: true })
db["$COLLECTION_NAME"].createIndex({ "computed": 1 }, { name: "computed_count_idx", background: true })

print("Indexes created successfully")
EOF
