#include "pch.hpp"

#ifdef WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else // LINUX
  #include <unistd.h>
  #include <climits>
#endif

void set_priority(Priority mode) {
  log_debug << "set process priority " << scast<int>(mode);

#ifdef WINDOWS
  static const std::unordered_map<Priority, ::DWORD> priority_classez {
    {Priority::low, BELOW_NORMAL_PRIORITY_CLASS},
    {Priority::normal, NORMAL_PRIORITY_CLASS},
    {Priority::high, HIGH_PRIORITY_CLASS},
    {Priority::realtime, REALTIME_PRIORITY_CLASS},
  };

  static const std::unordered_map<Priority, int> thread_priority_classez {
    {Priority::low, THREAD_PRIORITY_LOWEST},
    {Priority::normal, THREAD_PRIORITY_NORMAL},
    {Priority::high, THREAD_PRIORITY_HIGHEST},
    {Priority::realtime, THREAD_PRIORITY_TIME_CRITICAL},
  };

  auto ret = ::SetPriorityClass(::GetCurrentProcess(), priority_classez.at(mode));
  if (!ret)
    log_error << "ошибка при смене приоритета процесса";
    
  ret = ::SetThreadPriority(::GetCurrentThread(), thread_priority_classez.at(mode));
  if (!ret)
    log_error << "ошибка при смене приоритета потока";
#else // LINUX
  #pragma message("need impl for set_priority in Linux")
  log_error << "need impl for set_priority in Linux";
#endif
}

Str get_pc_name() {
#ifdef WINDOWS
  constexpr ::DWORD NAME_SZ = MAX_COMPUTERNAME_LENGTH + 1;
  char ret[NAME_SZ];
  ::DWORD sz = NAME_SZ;
  return_if (::GetComputerNameA(ret, &sz), Str(ret));
#else
  char hostname[1024];
  ::gethostname(hostname, 1024);
  return Str(hostname);
#endif

  return "localhost";
}

Str get_launch_dir() {
   Str fallback = Str(".") + SEPARATOR;

#ifdef WINDOWS
  std::vector<wchar_t> wbuf(MAX_PATH);
  DWORD wlen = 0;
  while (true) {
    wlen = ::GetModuleFileNameW(nullptr, wbuf.data(), scast<DWORD>(wbuf.size()));
    ret_if (wlen == 0, fallback);
    break_if (wlen < wbuf.size());
    wbuf.resize(wbuf.size() * 2);
  }

  const int utf8_len = ::WideCharToMultiByte(CP_UTF8, 0, wbuf.data(),
    scast<int>(wlen), nullptr, 0, nullptr, nullptr);
  ret_if (utf8_len <= 0, fallback);

  Str path(utf8_len, '\0');
  ::WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), scast<int>(wlen),
    path.data(), utf8_len, nullptr, nullptr);

  // В Windows/MSYS2 разделителем может быть как '\', так и '/'
  std::size_t sep_pos = path.find_last_of("\\/");
#else // Linux:
  std::vector<char> buf(PATH_MAX);
  ssize_t len = 0;
  while (true) {
    len = readlink("/proc/self/exe", buf.data(), buf.size());
    ret_if (len == -1, fallback);
    break_if (scast<size_t>(len) < buf.size());
    buf.resize(buf.size() * 2);
  }

  Str path(buf.data(), scast<size_t>(len));
  std::size_t sep_pos = path.find_last_of('/');
#endif

  ret_if (sep_pos == Str::npos, fallback);

  // Если бинарник лежит в корне диска (например, "C:\app.exe" или "/app")
  ret_if (sep_pos == 0, Str(1, SEPARATOR));

  return path.substr(0, sep_pos) + SEPARATOR;
}
