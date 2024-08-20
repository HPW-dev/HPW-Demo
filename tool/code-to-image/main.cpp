#include <filesystem>
#include <fstream>
#include <chrono>
#include <iostream>
#include <algorithm>
#include "util/error.hpp"
#include "util/str.hpp"
#include "util/str-util.hpp"
#include "util/mem-types.hpp"
#include "util/path.hpp"
#include "util/platform.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/image-io.hpp"
#include "graphic/util/util-templ.hpp"

using Images = Vector<Image>;
constexpr static const Pal8 BG_COLOR = Pal8::black;
constexpr static const Pal8 TEXT_COLOR = Pal8::white;
//constexpr static const Pal8 BORDER_COLOR = Pal8::from_real(0.5, true);
constexpr static const Pal8 BORDER_COLOR = Pal8::black;
constexpr static const Pal8 COMENT_COLOR = Pal8::from_real(0.45);
//constexpr static const Pal8 BORDER_COLOR = Pal8::black;
//constexpr static const Pal8 COMENT_COLOR = TEXT_COLOR;

inline Str prepare_seach_dir(cr<Str> path) {
  Str ret = path;
  if (ret.empty())
    ret = "./";
  ret += '/';
  conv_sep(ret);
  return ret;
}

// true если имя файла подходит по маске
inline bool check_file_masks(cr<Str> fname, cr<Strs> masks) {
  for (crauto mask: masks)
    if (std::filesystem::path(fname).extension() == mask)
      return true;
  return false;
}

inline void recursive_find_helper(Strs& dst,
cr<std::filesystem::directory_iterator> dir, cr<Strs> masks) {
  for (crauto file: dir) {
    if (file.is_directory())
      recursive_find_helper(
        dst,
        std::filesystem::directory_iterator(file.path().string() + SEPARATOR),
        masks);

    cauto fname = file.path().string();
    if (check_file_masks(fname, masks))
      dst.push_back(fname);
  }
}

/* репурсивно ищет имена файлов подходящие по шаблону
в masks по всем папкам в search_dir */
Strs recursive_find(cr<Str> search_dir, cr<Strs> masks) {
  Strs ret;
  cauto dir = std::filesystem::directory_iterator(search_dir);
  recursive_find_helper(ret, dir, masks);
  return ret;
}

// убирает файлы соответствующие маске
void exclude_mask(Strs& dst, cr<Strs> masks) {
  std::erase_if(dst, [&](cr<Str> fname) {
    for (crauto mask: masks)
      if (fname.find(mask) != Str::npos)
        return true;
    return false;
  });
}

Image text_to_image(cr<Str> fname) {
  std::ifstream file(fname);
  iferror(!file.is_open(), "error while openin file \"" << fname << "\"");

  // узнать ширину и высоту текста
  int txt_w {}, txt_h {};
  while (!file.eof()) {
    Str line;
    std::getline(file, line);
    txt_w = std::max<int>(txt_w, line.size());
    ++txt_h;
  }

  constexpr int BORDER = 2;
  constexpr int SPACE_H = 2; // отступ строки по высоте
  static_assert(BORDER > 0);
  static_assert(SPACE_H > 0);
  Image ret(txt_w + BORDER * 2, txt_h * SPACE_H + BORDER * 2, Pal8::black);
  ret.set_path(fname);
  ret.set_generated(true);

  // рисовать символы как пиксели
  int y {};
  file = std::ifstream(fname);
  while (!file.eof()) {
    Str line;
    std::getline(file, line);
    bool is_comment {};
    Str::value_type prev_ch {};

    for (int x {}; crauto ch: line) {
      // раскрасить каждый символ
      Pal8 color {};
      if (ch != ' ')
        color = TEXT_COLOR;
      /*if (ch >= '0' && ch <= '9')
        color = Pal8::red;*/
      if (prev_ch == '/' && ch == '/')
        is_comment = true;
      if (is_comment)
        color = COMENT_COLOR;
      prev_ch = ch;

      ret.set(BORDER + x, BORDER + y * SPACE_H, color);
      ++x;
    }

    ++y;
  } // file !file.eof

  draw_rect(ret, Rect(0, 0, ret.X, ret.Y), BORDER_COLOR);
  return ret;
} // text_to_image

// фильтрует и сортирует картинки для save_all_images
inline void prepare_image_list(Images& image_list) {
  assert(!image_list.empty());
  // сортировка по размеру
  std::sort(image_list.begin(), image_list.end(), [](cr<Image> a, cr<Image> b) {
    // при одинаковых размерах сортировать по ширине
    if (a.size == b.size)
      return a.X > b.X;
    return a.size > b.size;
  });
}

using Rects = Vector<Rect>;

/* перебирать каждый пиксель, пока не найдётся
свободная область или не хватит места под картинку */
void inser_to_free_space(Image& dst, cr<Image> src, Rects& occupied) {
  cauto mx = dst.X;
  cauto my = dst.Y;

  cfor (y, my) {
    cont_if(y + src.Y >= my);

    cfor (x, mx) {
      cont_if(x + src.X >= mx);
      const Rect src_rect(Vec(x, y), Vec(src.X, src.Y));

      // проверить свободные области
      bool is_free {true};
      for (crauto rect: occupied) {
        if (intersect(rect, src_rect)) {
          is_free = false;
          break;
        }
      }
      if (is_free) {
        insert(dst, src, {x, y});
        occupied.push_back(src_rect);
        return;
      }
    } // for x
  } // for y
}

Image make_atlas(cr<Images> images, int w, int h) {
  assert(!images.empty());
  Rects occupied {}; // занятые области
  Image dst(w, h, {BG_COLOR});

  using Clock = std::chrono::steady_clock;
  auto time_point = Clock::now();
  using namespace std::chrono_literals;

  // каждой картинке попытаться найти свободную область
  for (std::size_t i {}; crauto image: images) {
    inser_to_free_space(dst, image, occupied);
    if (Clock::now() - time_point >= 1s) {
      std::cout << "processed " << i << '/' << images.size() << '\n';
      time_point = Clock::now();
    }
    ++i;
  }

  return dst;
}

int main(const int argc, const char* argv[]) {
  // получить путь поиска файлов
  iferror(argc < 2, "Need more args.\n"
    "Usage: ./code-to-image __code_dir_name__");
  cauto search_dir = prepare_seach_dir(argv[1]);
  std::cout << "search dir: \"" << search_dir << '\"' << std::endl;

  // получить список файлов
  auto files = recursive_find(search_dir,
    {".cpp", ".hpp", ".py", ".bat", ".sh", ".txt", ".yml", ".c",
    ".h", ".cxx", ".hxx", ".tpp", "SConscript", "SCons", ".templ" });
  
  // убрать из списка неугодных
  exclude_mask(files, {"thirdparty"});
  iferror(files.empty(), "empty list of files in dir \"" + search_dir + '\"');
  std::cout << "files:\n";
  for (crauto fname: files)
    std::cout << "  \"" << fname << "\"\n";

  // прочитать все картинки
  const Str save_dir = "delme/";
  make_dir_if_not_exist(save_dir);
  Images images;
  for (crauto file: files)
    images.push_back( text_to_image(file) );
  // засейвить картинки в атлас
  prepare_image_list(images);
  cauto atlas = make_atlas(images, 3000, 4000);
  //cauto atlas = make_atlas(images, 600, 800);
  save(atlas, save_dir + "atlas.png");
}
