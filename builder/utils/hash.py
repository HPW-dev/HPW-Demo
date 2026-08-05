'''хэши файлов'''

import zlib
import hashlib


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

def crc32(path: str):
  """
  Хэш файла в CRC32
  Returns:
    None, если фйла нет
  """
  try:
    with open(path, 'rb', buffering=0) as f:
      crc = zlib.crc32(f.read())  
      return f'{crc & 0xFFFFFFFF:08x}'.upper()
  except OSError:
    return None
