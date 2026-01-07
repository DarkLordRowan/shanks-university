from pydantic import BaseModel, Field
from pydantic_mongo import ObjectIdField


class SeriesArguments(BaseModel):
    x: str | None = None


class Series(BaseModel):
    name: str
    lim: str
    arguments: SeriesArguments | None = None


class AdditionalArgs(BaseModel):
    remainder: str | None = None
    useRecurrentFormula: str | None = None
    beta: str | None = None


class Accel(BaseModel):
    name: str
    m_value: int
    additional_args: AdditionalArgs | None = None


class Event(BaseModel):
    name: str | None = None
    description: str | None = None


class ComputedItem(BaseModel):
    n: int
    series_value: str | None = None
    partial_sum: str | None = None
    partial_sum_deviation: str | None = None
    accel_value: str | None = None
    accel_value_deviation: str | None = None
    events: list[Event] | None = None


class ResultDocument(BaseModel):
    id: ObjectIdField = Field(alias="_id")
    series: Series
    accel: Accel
    computed: list[ComputedItem] | None = None
    error: str | None = None
    stack_id: str
