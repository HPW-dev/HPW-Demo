#include <ctime>
#include <cassert>
#include <sstream>
#include "screenshot.hpp"
#include "config.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/image-io.hpp"
#include "game/core/common.hpp"
#include "util/platform.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"

void save_screenshot(cr<Image> image) {
  assert(image);
  assert(hpw::config);

  auto t = std::time(nullptr);
#ifdef LINUX
  struct ::tm lt;
  ::localtime_r(&t, &lt);
#else // WINDOWS
  auto lt = *std::localtime(&t);
#endif

  cauto screenshots_dir = hpw::cur_dir + 
    (*hpw::config)["path"].get_str("screenshots", hpw::screenshots_path) + SEPARATOR;
  std::ostringstream oss;
  oss << screenshots_dir;
  make_dir_if_not_exist(hpw::cur_dir + oss.str());
  oss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");
  oss << "-" + n2s(rndu_fast(100'000)) + ".png";
  save(image, oss.str());
}
