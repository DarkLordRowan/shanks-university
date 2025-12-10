import argparse
from pathlib import Path
from typing import Any, Union, get_args, get_origin

from pydantic import BaseModel

from src.config.model import TrialConfig


def is_pydantic_model_type(t: Any) -> bool:
    if t is None:
        return False

    if isinstance(t, type) and issubclass(t, BaseModel):
        return True

    origin = get_origin(t)

    if origin is Union:
        return any(is_pydantic_model_type(a) for a in get_args(t))

    if origin is not None:
        args = get_args(t)
        return any(is_pydantic_model_type(a) for a in args)

    return False


def unwrap_type(t: Any) -> Any:
    origin = get_origin(t)
    if origin is Union:
        return next((a for a in get_args(t) if a is not type(None)), t)
    if origin is not None:
        return unwrap_type(get_args(t)[0])
    return t


def _add_field_to_parser(parser: argparse.ArgumentParser, name: str, field: Any):
    raw_type = field.annotation
    field_type = unwrap_type(raw_type)
    default = field.default

    arg_name = f"--{name.replace('_', '-')}"
    help_text = f"(type={field_type}, default={default})"

    if field_type is bool:
        parser.add_argument(arg_name, action="store_true", help=help_text)
        return

    if field_type is Path:
        parser.add_argument(arg_name, type=Path, help=help_text)
        return

    if hasattr(field_type, "__members__"):
        parser.add_argument(
            arg_name,
            type=str,
            choices=list(field_type.__members__.keys()),
            help=help_text,
        )
        return

    if isinstance(field_type, type):
        parser.add_argument(arg_name, type=field_type, help=help_text)
        return

    parser.add_argument(arg_name, help=help_text)


def _add_model_to_parser(
    parser: argparse.ArgumentParser, model: type[BaseModel], prefix: str = ""
):
    for name, field in model.model_fields.items():
        full_name = f"{prefix}{name}"

        if is_pydantic_model_type(field.annotation):
            group = parser.add_argument_group(full_name)
            cls = unwrap_type(field.annotation)
            _add_model_to_parser(group, cls, prefix=f"{full_name}.")  # type: ignore
        else:
            _add_field_to_parser(parser, full_name, field)


def build_cli_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Trial executor")

    subparsers = parser.add_subparsers(dest="command", required=True)

    run_parser = subparsers.add_parser("run", help="Run trial executor")

    run_parser.add_argument("--config", type=Path, help="JSON/YAML config file")

    _add_model_to_parser(run_parser, TrialConfig)

    return parser


def _build_nested_dict_from_args(args: dict[str, Any]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for k, v in args.items():
        if k == "config" or v is None:
            continue
        parts = k.split(".")
        d = result
        for part in parts[:-1]:
            d = d.setdefault(part, {})
        d[parts[-1]] = v
    return result


def load_config_and_apply_argparse() -> tuple[TrialConfig, argparse.Namespace]:
    parser = build_cli_parser()
    args = parser.parse_args()

    cfg = TrialConfig.load(args.config)

    overrides = _build_nested_dict_from_args(vars(args))

    final_cfg = cfg.model_copy(update=overrides)
    return final_cfg, args
