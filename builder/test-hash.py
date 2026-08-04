'''
cd proj_root
py builder/test-hash.py
'''

from common.ui import *
from common.hash import *


print("=== Тест хэшей ===")

# тестовый файл
path = '.tmp/hash test.txt'
with open(path, mode='w', encoding='utf-8') as f:
  f.write('Игра H.P.W')

res_sha3_512 = sha3_512(path)
res_blake2b = blake2b(path)
res_crc32 = crc32(path)

print(f'SHA3-512 .. {to_yellow(res_sha3_512)}')
print(f'Blake2b ... {to_yellow(res_blake2b)}')
print(f'CRC32 ..... {to_yellow(res_crc32)}')

assert(res_sha3_512 == "543B361CA8AEDE6B220019F7D07CC19130810CC6829DADD27246279157FCFADAAB60FC020116AF5BA0DE17264F50DDB6230EA128A3E4444E714B527E09CE3A47")
assert(res_blake2b == "06E9D8B04928F72A0B740897DAC2992CB416BA97FC7DE241B939091C8A8F23DDCE89F9D56C1E0A66DE83930EC9B4C8D0B8A3C9225959527D6C6C7D85C5A95E96")
assert(res_crc32 == "ECE0D08A")

print(to_green('> тест пройден'))
