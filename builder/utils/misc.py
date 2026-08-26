def prepare_obj_name(raw: str) -> str:
  raw = raw.replace('.', '_')
  raw = raw.replace(' ', '-')
  raw = raw.replace('/', '_')
  raw = raw.replace('\\', '_')
  raw = raw.replace('.cpp', '')
  raw = raw.replace('.c', '')
  raw = raw.replace('.cxx', '')
  raw = raw.replace('.xx', '')
  raw = raw.replace('.pp', '')
  raw += '.o'
  return raw