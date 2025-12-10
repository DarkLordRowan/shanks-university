from typing import Any

from bson import ObjectId
from fastapi import APIRouter, Depends, HTTPException, Query
from fastapi.responses import JSONResponse

from src.dependency import get_collection
from src.enums import SortField, SortOrder
from src.models import ResultDocument

api_router = APIRouter(prefix="/api")


@api_router.get("/results", response_model=list[ResultDocument])
async def get_results(
    skip: int = Query(0, ge=0, description="Number of documents to skip"),
    limit: int = Query(
        10, ge=1, le=1000, description="Maximum number of documents to return"
    ),
    series_name: str | None = Query(
        None, description="Filter by series name (case-insensitive)"
    ),
    accel_name: str | None = Query(
        None, description="Filter by acceleration algorithm name"
    ),
    has_error: bool | None = Query(
        None, description="Filter by error presence"
    ),
    min_n: int | None = Query(
        None, ge=0, description="Minimum n value in computed items"
    ),
    max_n: int | None = Query(
        None, ge=0, description="Maximum n value in computed items"
    ),
    stack_id: str | None = Query(None, description="Filter by stack_id"),
    sort_by: SortField = Query(
        SortField.created_at, description="Field to sort by"
    ),
    sort_order: SortOrder = Query(SortOrder.desc, description="Sort order"),
    include_computed: bool = Query(
        True, description="Include computed items in response"
    ),
    collection=Depends(get_collection),
) -> list[ResultDocument]:
    pipeline = []

    match_stage = {}
    if series_name:
        match_stage["series.name"] = {
            "$regex": f"^{series_name}$",
            "$options": "i",
        }
    if accel_name:
        match_stage["accel.name"] = accel_name
    if has_error is not None:
        match_stage["has_error"] = has_error
    if stack_id:
        match_stage["stack_id"] = stack_id

    if min_n is not None or max_n is not None:
        computed_filter = {}
        if min_n is not None:
            computed_filter["$gte"] = min_n
        if max_n is not None:
            computed_filter["$lte"] = max_n
        match_stage["computed.n"] = computed_filter

    if match_stage:
        pipeline.append({"$match": match_stage})

    sort_direction = 1 if sort_order == SortOrder.asc else -1
    sort_field_map = {
        SortField.created_at: "created_at",
        SortField.series_name: "series.name",
        SortField.accel_name: "accel.name",
    }
    sort_field = sort_field_map.get(sort_by, "created_at")

    if sort_by == SortField.n_count:
        pipeline.append({"$unwind": "$computed"})
        pipeline.append({"$sort": {"computed.n": sort_direction}})
        pipeline.append(
            {
                "$group": {
                    "_id": "$_id",
                    "doc": {"$first": "$$ROOT"},
                    "computed_items": {"$push": "$computed"},
                }
            }
        )
        pipeline.append({"$addFields": {"doc.computed": "$computed_items"}})
        pipeline.append({"$replaceRoot": {"newRoot": "$doc"}})
    else:
        pipeline.append({"$sort": {sort_field: sort_direction}})

    pipeline.append({"$skip": skip})
    pipeline.append({"$limit": limit})

    if not include_computed:
        pipeline.append({"$project": {"computed": 0}})

    results = await collection.aggregate(pipeline).to_list(length=limit)
    return [ResultDocument(**result) for result in results]


@api_router.get("/results/{result_id}", response_model=ResultDocument)
async def get_result_by_id(
    result_id: str,
    include_computed: bool = Query(
        True, description="Include computed items in response"
    ),
    collection=Depends(get_collection),
) -> ResultDocument:
    if not ObjectId.is_valid(result_id):
        raise HTTPException(status_code=400, detail="Invalid result_id format")

    pipeline = [
        {"$match": {"_id": ObjectId(result_id)}},
    ]

    if not include_computed:
        pipeline.append({"$project": {"computed": ObjectId("0")}})

    results = await collection.aggregate(pipeline).to_list(length=1)

    if not results:
        raise HTTPException(status_code=404, detail="Result not found")

    return ResultDocument(**results[0])


@api_router.get("/filters", response_class=JSONResponse)
async def get_available_filters(
    collection=Depends(get_collection),
) -> dict[str, Any]:
    pipeline = [
        {
            "$facet": {
                "series_names": [
                    {"$group": {"_id": "$series.name"}},
                    {"$sort": {"_id": 1}},
                    {"$limit": 50},
                ],
                "accel_names": [
                    {"$group": {"_id": "$accel.name"}},
                    {"$sort": {"_id": 1}},
                    {"$limit": 50},
                ],
                "n_range": [
                    {"$unwind": "$computed"},
                    {
                        "$group": {
                            "_id": None,
                            "min_n": {"$min": "$computed.n"},
                            "max_n": {"$max": "$computed.n"},
                        }
                    },
                ],
                "stack_ids": [
                    {"$group": {"_id": "$stack_id"}},
                    {"$sort": {"_id": 1}},
                    {"$limit": 50},
                ],
            }
        }
    ]

    filters_result = await collection.aggregate(pipeline).to_list(length=1)
    filters = filters_result[0] if filters_result else {}

    return {
        "series_names": [
            item["_id"] for item in filters.get("series_names", [])
        ],
        "accel_names": [
            item["_id"] for item in filters.get("accel_names", [])
        ],
        "stack_ids": [item["_id"] for item in filters.get("stack_ids", [])],
        "n_range": filters.get("n_range", [{"min_n": 0, "max_n": 0}])[0],
    }
