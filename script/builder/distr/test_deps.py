import script.builder.distr.cxx_compiler as cxx_compiler
import script.builder.io.fs as fs

def build_test_deps():
  '''собирает тестовую прогу'''
  print('\n' + ':'*30 + ' ТЕСТОВАЯ СБОРКА ' + ':'*30)

  deps_dir = "script/builder/dependency-test/"
  sources = []
  sources.extend(fs.find_mask(f"{deps_dir}*.cpp"))
  sources.extend(fs.find_mask(f"{deps_dir}utils/*.cpp"))
  sources.extend(fs.find_mask(f"{deps_dir}funcs/*.cpp"))
  compiler = cxx_compiler.Compiler(
    target = ".tmp/test.exe",
    sources = sources,
    cxx_opts = ["Os", "g0", "std=c++23", "pipe"],
    defines = ["NDEBUG"],
    ld_opts = ["static", 's'],
    temp_dir = ".tmp/",
    include = [deps_dir],
    log=True
  )
  compiler.compile()
