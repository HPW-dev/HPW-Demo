#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "util/error.hpp"
#include "util/str.hpp"
#include "util/str-util.hpp"
#include "util/platform.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/image-io.hpp"
#include "graphic/util/util-templ.hpp"

inline Str prepare_seach_dir(CN<Str> path) {
  Str ret = path;
  if (ret.empty())
    ret = "./";
  ret += '/';
  conv_sep(ret);
  return ret;
}

// true если имя файла подходит по маске
inline bool check_file_masks(CN<Str> fname, CN<Strs> masks) {
  for (cnauto mask: masks)
    if (std::filesystem::path(fname).extension() == mask)
      return true;
  return false;
}

inline void recursive_find_helper(Strs& dst,
CN<std::filesystem::directory_iterator> dir, CN<Strs> masks) {
  for (cnauto file: dir) {
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
Strs recursive_find(CN<Str> search_dir, CN<Strs> masks) {
  Strs ret;
  cauto dir = std::filesystem::directory_iterator(search_dir);
  recursive_find_helper(ret, dir, masks);
  return ret;
}

// убирает файлы соответствующие маске
void exclude_mask(Strs& dst, CN<Strs> masks) {
  std::erase_if(dst, [&](CN<Str> fname) {
    for (cnauto mask: masks)
      if (fname.find(mask) != Str::npos)
        return true;
    return false;
  });
}

Image text_to_image(CN<Str> fname) {
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

    for (int x {}; cnauto ch: line) {
      // раскрасить каждый символ
      Pal8 color {};
      if (ch != ' ')
        color = Pal8::white;
      /*if (ch >= '0' && ch <= '9')
        color = Pal8::red;*/
      if (prev_ch == '/' && ch == '/')
        is_comment = true;
      if (is_comment)
        color = Pal8::from_real(0.45);
      prev_ch = ch;

      ret.set(BORDER + x, BORDER + y * SPACE_H, color);
      ++x;
    }
    ++y;
  }

  draw_rect(ret, Rect(0, 0, ret.X, ret.Y), Pal8::white);
  return ret;
} // text_to_image

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
  for (cnauto fname: files)
    std::cout << "  \"" << fname << "\"\n";

  Vector<Image> images {};
  for (cnauto file: files)
    images.push_back(text_to_image(file));
  
  for (uint i {}; cnauto image: images)
    save(image, "delme\\" + n2s(i++) + "-img.png");
}
