"""
Parquet export service implementation.
"""

import json
from pathlib import Path
from typing import Any, Sequence

import pyarrow as pa
import pyarrow.dataset as ds

from src.config.model import TrialConfig
from src.domain.export_service import ExportService
from src.domain.params import BaseSeriesParam


class ParquetExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def _sanitize_complex_value(self, value: Any) -> dict[str, str] | None:
        """Convert value to {real: str, imag: str} format."""
        if value is None:
            return None
        
        if isinstance(value, dict) and "real" in value and "imag" in value:
             return {"real": str(value["real"]), "imag": str(value["imag"])}
        
        return {"real": str(value), "imag": "0.0"}

    def _format_noise(self, noise: dict) -> str:
        if not noise:
            return "None"
        method = noise.get("method", "Unknown")
        params = []
        for k in sorted(noise.keys()):
            v = noise[k]
            if k == "method" or v is None:
                continue
            params.append(f"{k}={v}")
        if not params:
            return method
        return f"{method}({', '.join(params)})"

    def _get_accel_record(self, result: dict, series_id: int) -> dict[str, Any]:
        accel = result["accel"]
        computed = result["computed"]
        error = result.get("error")
        filtered = result.get("filtered")
        noise = result.get("noise")
        
        comp_list = []
        events_list = []
        for step in computed:
             n = step["n"]
             val = self._sanitize_complex_value(step.get("accel_value"))
             dev = str(step.get("accel_value_deviation", ""))
             
             comp_list.append({
                 "value": val,
                 "deviation": dev
             })
             
             if "events" in step and step["events"]:
                 for evt in step["events"]:
                     events_list.append({
                         "n": n,
                         "name": str(evt.get("name")),
                         "description": str(evt.get("description"))
                     })

        errors_list = []
        if error:
             desc = error.get("description", "Unknown error")
             errors_list.append({"n": 0, "message": str(desc)})

        noise_str = "None"
        if noise:
             noise_dict = {
                 "type": str(noise.get("type", "")),
                 "method": str(noise.get("method", "")),
                 "param1": str(noise.get("param1", "")),
                 "param2": str(noise.get("param2", "")),
                 "seed": int(noise.get("seed", 0))
             }
             noise_str = self._format_noise(noise_dict)

        filtered_data = None
        if filtered:
             # filtered is a dict: {start_n, segment_length, methods: {name: {values: [], average: ...}}}
             methods_data = []
             methods_dict = filtered.get("methods", {})
             for m_name, m_res in methods_dict.items():
                 # Process values list
                 raw_vals = m_res.get("values", [])
                 sanitized_vals = [self._sanitize_complex_value(v) for v in raw_vals]
                 
                 # Process average
                 raw_avg = m_res.get("average")
                 sanitized_avg = self._sanitize_complex_value(raw_avg)
                 
                 methods_data.append((
                     m_name,
                     {
                         "values": sanitized_vals,
                         "average": sanitized_avg
                     }
                 ))
             
             filtered_data = {
                 "start_n": filtered.get("start_n", 0),
                 "segment_length": filtered.get("segment_length", 0),
                 "methods": methods_data
             }

        return {
            "series_id": series_id,
            "accel_name": str(accel.get("name")),
            "m_value": int(accel.get("m_value")),
            "additional_args": {k: str(v) for k, v in accel.get("additional_args", {}).items()},
            "computed": comp_list,
            "errors": errors_list,
            "events": events_list,
            "noise_str": noise_str,
            "filtered": filtered_data
        }

    def export(self, dicts: Sequence[dict], config: TrialConfig, series: list[BaseSeriesParam], **kwargs):
        self.location.mkdir(parents=True, exist_ok=True)
        file_options = ds.ParquetFileFormat().make_write_options(compression="zstd")

        series_records = {}
        accel_records = []
        
        for result in dicts:
            s_res = result["series"]
            s_id = s_res["id"]
            
            if s_id not in series_records:
                 comp_list = []
                 for step in result["computed"]:
                     val = self._sanitize_complex_value(step.get("partial_sum"))
                     dev = str(step.get("partial_sum_deviation", ""))
                     comp_list.append({
                         "n": step["n"],
                         "value": val,
                         "deviation": dev
                     })
                 
                 prec_str = "f64"
                 found_param = next((p for p in series if getattr(p, "id", -1) == s_id), None)
                 if found_param:
                     prec_str = str(found_param.precision.value)
                
                 series_records[s_id] = {
                     "series_name": s_res["name"],
                     "series_id": s_id,
                     "precision": prec_str,
                     "arguments": {k: str(v) for k, v in s_res["arguments"].items()},
                     "series_limit": self._sanitize_complex_value(s_res["lim"]),
                     "computed": comp_list
                 }
            
            accel_records.append(self._get_accel_record(result, s_id))

        if series_records:
            all_arg_keys = set()
            for r in series_records.values():
                all_arg_keys.update(r["arguments"].keys())
            sorted_arg_keys = sorted(list(all_arg_keys))
            
            arg_fields = [(k, pa.string()) for k in sorted_arg_keys]
            
            series_schema = pa.schema([
                ("series_name", pa.string()),
                ("series_id", pa.int64()),
                ("precision", pa.string()),
                ("arguments", pa.struct(arg_fields)),
                ("series_limit", pa.struct([("real", pa.string()), ("imag", pa.string())])),
                ("computed", pa.list_(pa.struct([
                    ("n", pa.int64()),
                    ("value", pa.struct([("real", pa.string()), ("imag", pa.string())])),
                    ("deviation", pa.string())
                ])))
            ])
            
            series_data = list(series_records.values())
            series_table = pa.Table.from_pylist(series_data, schema=series_schema)
            
            ds.write_dataset(
                series_table,
                base_dir=self.location / "series",
                format="parquet",
                partitioning=ds.partitioning(pa.schema([("precision", pa.string()), ("series_name", pa.string())]), flavor="hive"),
                file_options=file_options,
                existing_data_behavior="overwrite_or_ignore"
            )

        if accel_records:
            all_add_keys = set()
            for r in accel_records:
                all_add_keys.update(r["additional_args"].keys())
            sorted_add_keys = sorted(list(all_add_keys))
            
            add_arg_fields = [(k, pa.string()) for k in sorted_add_keys]
            
            accel_schema = pa.schema([
                ("series_id", pa.int64()),
                ("accel_name", pa.string()),
                ("m_value", pa.int64()),
                ("additional_args", pa.struct(add_arg_fields)),
                ("computed", pa.list_(
                    pa.struct([
                        ("value", pa.struct([("real", pa.string()), ("imag", pa.string())])),
                        ("deviation", pa.string())
                    ])
                )),
                ("errors", pa.list_(pa.struct([("n", pa.int64()), ("message", pa.string())]))),
                ("events", pa.list_(pa.struct([("n", pa.int64()), ("name", pa.string()), ("description", pa.string())]))),
                ("noise_str", pa.string()),
                ("filtered", pa.struct([
                    ("start_n", pa.int64()),
                    ("segment_length", pa.int64()),
                    ("methods", pa.map_(
                        pa.string(), # Key: Method Name
                        pa.struct([  # Value: Method Result
                            ("values", pa.list_(
                                pa.struct([("real", pa.string()), ("imag", pa.string())])
                            )),
                            ("average", pa.struct([("real", pa.string()), ("imag", pa.string())]))
                        ])
                    ))
                ]))
            ])

            accel_table = pa.Table.from_pylist(accel_records, schema=accel_schema)
            
            ds.write_dataset(
                accel_table,
                base_dir=self.location / "accelerations",
                format="parquet",
                partitioning=ds.partitioning(pa.schema([("series_id", pa.int64())]), flavor="hive"),
                file_options=file_options,
                existing_data_behavior="overwrite_or_ignore"
            )
