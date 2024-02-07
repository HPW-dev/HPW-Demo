#include <cassert>
#include <iostream>
#include <array>
#include "host-util.hpp"
#include "command.hpp"
#include "game/game-core.hpp"
#include "game/game-sync.hpp"
#include "game/util/game-archive.hpp"
#include "util/math/random.hpp"
#include "util/str-util.hpp"
#include "util/file/archive.hpp"
#include "util/error.hpp"
#include "stb/stb_image.h"

void set_target_ups(int new_ups) {
  assert(new_ups > 0);
  hpw::target_ups = new_ups;
  hpw::target_update_time = 1.0 / hpw::target_ups;
}

void print_logo() {
  static const std::array<Str, 21> table_logo {
    R"( |  | +--+ | | | )" "\n"
    R"( +--+ +--+ | | | )" "\n"
    R"( |  | |    +-+-+ )" "\n"
    ,
    R"( : : :': : : : )" "\n"
    R"( :': :'' '.'.' )" "\n"
    ,
    R"(      _        )" "\n"
    R"( |_| |_| | | | )" "\n"
    R"( | | |    V V  )" "\n"
    ,
    R"( :  : :'': : : : )" "\n"
    R"( :..: :..: : : : )" "\n"
    R"( :  : :    '.'.' )" "\n"
    ,
    R"( |   |  +---+  |  |  |)" "\n"
    R"( |   |  |   |  |  |  |)" "\n"
    R"( +---+  +---+  |  |  |)" "\n"
    R"( |   |  |      |  |  |)" "\n"
    R"( |   |  |      +--+--+)" "\n"
    ,
    R"( 8 8 888 8 8 8)" "\n"
    R"( 888 888 8 8 8)" "\n"
    R"( 8 8 8   88888)" "\n"
    ,
    R"( V     ^  > > > V  ^   V   ^)" "\n"
    R"( V     ^  ^     V  ^   V   ^)" "\n"
    R"( V > > ^  ^ < < <  ^   V   ^)" "\n"
    R"( V     V  ^        ^   V   ^)" "\n"
    R"( V     V  ^         < < > > )" "\n"
    ,
    R"(  _    _ _______          __)" "\n"
    R"( | |  | |  __ \ \        / /)" "\n"
    R"( | |__| | |__) \ \  /\  / / )" "\n"
    R"( |  __  |  ___/ \ \/  \/ /  )" "\n"
    R"( | |  | | |      \  /\  /   )" "\n"
    R"( |_|  |_|_|       \/  \/    )" "\n"
    ,
    R"(  _____ _____ _ _ _ )" "\n"
    R"( |  |  |  _  | | | |)" "\n"
    R"( |     |   __| | | |)" "\n"
    R"( |__|__|__|  |_____|)" "\n"
    ,
    R"(     __  ______ _       __)" "\n"
    R"(    / / / / __ \ |     / /)" "\n"
    R"(   / /_/ / /_/ / | /| / / )" "\n"
    R"(  / __  / ____/| |/ |/ /  )" "\n"
    R"( /_/ /_/_/     |__/|__/   )" "\n"
    ,
    R"( __/\\\________/\\\__/\\\\\\\\\\\\\____/\\\______________/\\\_        )" "\n"
    R"(  _\/\\\_______\/\\\_\/\\\/////////\\\_\/\\\_____________\/\\\_       )" "\n"
    R"(   _\/\\\_______\/\\\_\/\\\_______\/\\\_\/\\\_____________\/\\\_      )" "\n"
    R"(    _\/\\\\\\\\\\\\\\\_\/\\\\\\\\\\\\\/__\//\\\____/\\\____/\\\__     )" "\n"
    R"(     _\/\\\/////////\\\_\/\\\/////////_____\//\\\__/\\\\\__/\\\___    )" "\n"
    R"(      _\/\\\_______\/\\\_\/\\\_______________\//\\\/\\\/\\\/\\\____   )" "\n"
    R"(       _\/\\\_______\/\\\_\/\\\________________\//\\\\\\//\\\\\_____  )" "\n"
    R"(        _\/\\\_______\/\\\_\/\\\_________________\//\\\__\//\\\______ )" "\n"
    R"(         _\///________\///__\///___________________\///____\///_______)" "\n"
    ,
    R"(    __ _____ _      __)" "\n"
    R"(   / // / _ \ | /| / /)" "\n"
    R"(  / _  / ___/ |/ |/ / )" "\n"
    R"( /_//_/_/   |__/|__/  )" "\n"
    ,
    R"(  __  __     ______   __     __   )" "\n"
    R"( /\ \_\ \   /\  == \ /\ \  _ \ \  )" "\n"
    R"( \ \  __ \  \ \  _-/ \ \ \/ ".\ \ )" "\n"
    R"(  \ \_\ \_\  \ \_\    \ \__/".~\_\)" "\n"
    R"(   \/_/\/_/   \/_/     \/_/   \/_/)" "\n"
    ,
    R"( ┓┏┏┓┓ ┏)" "\n"
    R"( ┣┫┃┃┃┃┃)" "\n"
    R"( ┛┗┣┛┗┻┛)" "\n"
    ,
    R"( ██╗  ██╗ ██████╗ ██╗    ██╗)" "\n"
    R"( ██║  ██║ ██╔══██╗██║    ██║)" "\n"
    R"( ███████║ ██████╔╝██║ █╗ ██║)" "\n"
    R"( ██╔══██║ ██╔═══╝ ██║███╗██║)" "\n"
    R"( ██║  ██║ ██║     ╚███╔███╔╝)" "\n"
    R"( ╚═╝  ╚═╝ ╚═╝      ╚══╝╚══╝ )" "\n"
    ,
    R"( ╦ ╦ ╔═╗ ╦ ╦)" "\n"
    R"( ╠═╣ ╠═╝ ║║║)" "\n"
    R"( ╩ ╩ ╩   ╚╩╝)" "\n"
    ,
    R"( |         | |`````````, `.               .')" "\n"
    R"( |_________| |'''''''''    `.           .'  )" "\n"
    R"( |         | |               `.   .   .'    )" "\n"
    R"( |         | |                 `.' `.'      )" "\n"
    ,
    R"(     _    )" "\n"
    R"( |_||_)| |)" "\n"
    R"( | ||  |^|)" "\n"
    ,
    R"( _|    _|  _|_|_|    _|          _|  )" "\n"
    R"( _|    _|  _|    _|  _|          _|  )" "\n"
    R"( _|_|_|_|  _|_|_|    _|    _|    _|  )" "\n"
    R"( _|    _|  _|          _|  _|  _|    )" "\n"
    R"( _|    _|  _|            _|  _|      )" "\n"
    ,
    R"(  _   _ _____ __    __)" "\n"
    R"( | |_| || ()_)\ \/\/ /)" "\n"
    R"( |_| |_||_|    \_/\_/ )" "\n"
    ,
    R"(       __      )" "\n"
    R"( |__| |__) |  |)" "\n"
    R"( |  | |    |/\|)" "\n"
    ,                         
  }; // table_logo

  // случайный выбор лого
  std::cout << '\n';
  auto table_idx = rndu_fast(table_logo.size()-1);
  std::cout << table_logo.at(table_idx);
  std::cout << '\n';
  std::cout << "Запуск H.P.W\n";
  std::cout.flush();
} // print_logo

Vector<float> load_ogl_palette(CN<Str> fname) {
  // загрузка png
  int x, y;
  int comp; // сколько цветовых каналов
  assert(hpw::archive);
  cauto mem = hpw::archive->get_file(fname);
  cauto mem_data = cptr2ptr<CP<stbi_uc>>(mem.data.data());
  auto decoded = stbi_load_from_memory(mem_data, mem.data.size(), &x, &y, &comp, STBI_rgb);
  iferror( !decoded, "image data is not decoded");
  iferror(x < 256, "ширина картинки должна быть не меньше 256");

  // rgb -> palette array
  Vector<float> ogl_pal;
  int rgb_index = 0;
  for (int i = 0; i < x * y; ++i) {
    constexpr auto mul = (1.0 / 255.0);
    ogl_pal.push_back(decoded[rgb_index + 0] * mul);
    ogl_pal.push_back(decoded[rgb_index + 1] * mul);
    ogl_pal.push_back(decoded[rgb_index + 2] * mul);
    rgb_index += 3;
  }
  stbi_image_free(decoded);
  return ogl_pal;
} // load_ogl_palette
