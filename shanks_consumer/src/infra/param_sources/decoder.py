import json

import pyshanks as ps
from src.domain.precision import PrecisionType, is_arb_precision


def decide_json_decoder(precision: PrecisionType):
    if not is_arb_precision(precision):
        return None

    class _Decoder(json.JSONDecoder):
        def __init__(self, *args, **kwargs):
            super().__init__(parse_float=ps.Arb, parse_int=ps.Arb, *args, **kwargs)

    return _Decoder
