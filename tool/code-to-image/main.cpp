#include <filesystem>
#include <fstream>
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
constexpr static const Pal8 TEXT_COLOR = Pal8::white;
constexpr static const Pal8 BORDER_COLOR = Pal8::from_real(0.5, true);
constexpr static const Pal8 COMENT_COLOR = Pal8::from_real(0.45);
//constexpr static const Pal8 BORDER_COLOR = Pal8::black;
//constexpr static const Pal8 COMENT_COLOR = TEXT_COLOR;

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

struct Node {
  Unique<Node> a {};
  Unique<Node> b {};
  Rect area {};
  CP<Image> image_ptr {};
};

// true, если получилось вставить изображение
bool insert_to_tree(Unique<Node>& node, CN<Image> image) {
  /* когда нада есть и картинка уже в ней, то определить
  в какую внутреннюю ветвь добавить изображение */
  if (node->image_ptr) {
    if (!insert_to_tree(node->a, image))
      return insert_to_tree(node->b, image);
    return false;
  }

  // определить что изображение помещается в область
  if (image.X <= node->area.size.x && image.Y <= node->area.size.y) {
    node->a = new_unique<Node>();
    node->a->area = Rect (
      node->area.pos.x + image.X,
      node->area.pos.y,
      std::max<int>(0, node->area.size.x - image.X),
      image.Y
    );
    node->b = new_unique<Node>();
    node->b->area = Rect (
      node->area.pos.x,
      node->area.pos.y + image.Y,
      node->area.size.x,
      std::max<int>(0, node->area.size.y - image.Y)
    );
    node->image_ptr = &image;
    return true;
  }

  return false;
} // insert_to_tree

void draw_tree(CN<Unique<Node>> node, Image& dst) {
  return_if(!node);
  //draw_rect(dst, node->area, Pal8::white);
  if (node->image_ptr)
    insert(dst, *node->image_ptr, node->area.pos);
  if (node->a)
    draw_tree(node->a, dst);
  if (node->b)
    draw_tree(node->b, dst);
}

Image make_atlas(CN<Images> images, int w, int h) {
  assert(!images.empty());

  Unique<Node> root;
  for (cnauto image: images) {
    // есди это первая нода
    if (!root) {
      cont_if (image.X > w);
      cont_if (image.Y > h);
      root = new_unique<Node>();
      root->a = new_unique<Node>();
      root->a->area = Rect(image.X, 0, std::max(0, w - image.X), image.Y);
      root->b = new_unique<Node>();
      root->b->area = Rect(0, image.Y, w, std::max(0, h - image.Y));
      root->area = Rect(0,0, w,h);
      root->image_ptr = &image;
      continue;
    }

    insert_to_tree(root, image);
  }

  Image ret(w, h, Pal8::black);
  draw_tree(root, ret);
  return ret;
}

// фильтрует и сортирует картинки для save_all_images
inline void prepare_image_list(Images& image_list) {
  assert(!image_list.empty());
  // сортировка по размеру
  std::sort(image_list.begin(), image_list.end(), [](CN<Image> a, CN<Image> b) {
    // при одинаковых размерах сортировать по ширине
    if (a.size == b.size)
      return a.X > b.X;
    return a.size > b.size;
  });
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
  for (cnauto fname: files)
    std::cout << "  \"" << fname << "\"\n";

  // прочитать все картинки
  const Str save_dir = "delme/";
  make_dir_if_not_exist(save_dir);
  Images images;
  for (cnauto file: files)
    images.push_back( text_to_image(file) );
  // засейвить картинки в атлас
  prepare_image_list(images);
  cauto atlas = make_atlas(images, 3000, 4000);
  //cauto atlas = make_atlas(images, 600, 800);
  save(atlas, save_dir + "atlas.png");
}
