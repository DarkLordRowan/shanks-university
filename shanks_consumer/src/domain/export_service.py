from typing import Any, Protocol, Sequence


class ExportService(Protocol):

    def export(self, dicts: Sequence[dict], **kwargs: Any) -> None: ...
