''' Тут всё, что связанно с временными метками '''

from datetime import datetime, timezone

def utc_time() -> str:
  return datetime.now(timezone.utc)
