if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import zlib
import hashlib
import script.builder.utils as utils

def file_sha3_512(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'sha3_512').hexdigest().upper()
  except:
    return None
  
def file_blake2b(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'blake2b').hexdigest().upper()
  except:
    return None

def file_crc32(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      crc = zlib.crc32(f.read())  
      return '{:08x}'.format(crc & 0xFFFFFFFF).upper()
  except:
    return None

def calculate_checksums(env):
  info = {}
  info['exe_crc32'] = file_crc32(env['executable_path'])
  info['exe_sha3_512'] = file_sha3_512(env['executable_path'])
  info['data_crc32'] = file_crc32(env['data_archive_path'])
  info['data_sha3_512'] = file_sha3_512(env['data_archive_path'])

  print('\n' + ':'*30 + ' КОНТРОЛЬНЫЕ СУММЫ ' + ':'*30)
  print(f'- CRC32 EXE: {  utils.in_env(info, 'exe_crc32')}')
  print(f'- CRC32 DATA: { utils.in_env(info, 'data_crc32')}')
  #print(f'- SHA512 EXE: { in_env(info, 'exe_sha512')}')
  #print(f'- SHA512 DATA: {in_env(info, 'data_sha512')}')
  return info
