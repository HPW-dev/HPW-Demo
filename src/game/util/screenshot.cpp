#include "pch.hpp"
#include "screenshot.hpp"
#include "config.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/image/image-io.hpp"
#include "game/core/common.hpp"

void save_screenshot(cr<Image> image) {
  assert(image);

  auto t = std::time(nullptr);
#ifdef LINUX
  struct ::tm lt;
  ::localtime_r(&t, &lt);
#else // WINDOWS
  auto lt = *std::localtime(&t);
#endif

  auto screenshots_dir = hpw::cur_dir + hpw::screenshots_path + SEPARATOR;
  if (hpw::config.contains("path"))
    screenshots_dir = hpw::cur_dir + hpw::config["path"].value(
      "screenshots", hpw::screenshots_path) + SEPARATOR;
  std::ostringstream oss;
  oss << screenshots_dir;
  make_dir_if_not_exist(oss.str());
  oss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");
  oss << "-" + n2s(rndu_fast(100'000)) + ".png";
  save(image, oss.str());
}
