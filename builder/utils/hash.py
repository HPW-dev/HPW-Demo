'''хэши файлов'''

import hashlib
import zlib


def sha3_512(path: str):
  """
  Хэш файла в SHA3-512
  Returns:
    None, если фйла нет
  """
  try:
    with open(path, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'sha3_512').hexdigest().upper()
  except OSError:
    return None
  
def blake2b(path: str):
  """
  Хэш файла в Blake2b
  Returns:
    None, если фйла нет
  """
  try:
    with open(path, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'blake2b').hexdigest().upper()
  except OSError:
    return None

def crc32(path: str, chunk_size=1024*64):
  """
  Хэш файла в CRC32
  Returns:
    None, если фйла нет
  """
  try:
    crc = 0
    with open(path, 'rb') as f:
      # читаем файл частями, чтобы не нагрузить
      while True:
        chunk = f.read(chunk_size)
        if not chunk:
          break
        crc = zlib.crc32(chunk, crc)
    return f"{crc & 0xFFFFFFFF:08X}"
  except OSError:
    return None
