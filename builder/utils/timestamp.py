''' Тут всё, что связанно с временными метками '''

from datetime import datetime, timezone

def utc_time():
  return str(datetime.now(timezone.utc))
