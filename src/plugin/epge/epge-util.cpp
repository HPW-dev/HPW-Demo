#include <cassert>
#include <unordered_map>
#include <functional>
#include "epge-util.hpp"
#include "game/core/epges.hpp"
#include "util/file/yaml.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"

#include "display-3d.hpp"
#include "flashes.hpp"
#include "flashes-2.hpp"
#include "pixelate.hpp"
#include "epilepsy.hpp"
#include "inversion.hpp"
#include "fading.hpp"
#include "scanline.hpp"
#include "shaker.hpp"
#include "shuffler.hpp"
#include "pixels-per-frame.hpp"
#include "epge-resize.hpp"
#include "mirror.hpp"
#include "color-rotation.hpp"

using Epge_maker = std::function< Unique<epge::Base> ()>;

namespace {
static std::unordered_map<Str, Epge_maker> _epge_makers {};
}

template <class T>
inline void add_epge() {
  cauto epge = new_unique<T>();
  cauto name = epge->name();
  ::_epge_makers[name] = []{ return new_unique<T>(); };
}

// регистрирует плагины в списке
inline static void init_epge_list() {
  add_epge<epge::Epilepsy>();
  add_epge<epge::Color_rotation>();
  add_epge<epge::Shuffler>();
  add_epge<epge::Pixels_per_frame>();
  add_epge<epge::Shaker>();
  add_epge<epge::Pixelate>();
  add_epge<epge::Inversion>();
  add_epge<epge::Mirror>();
  add_epge<epge::Display_3d>();
  add_epge<epge::Resize>();
  add_epge<epge::Flashes_2>();
  add_epge<epge::Flashes>();
  add_epge<epge::Fading>();
  add_epge<epge::Scanline>();
};

void save_epges(Yaml& config) {
  cauto total_epges = graphic::epges.size();
  return_if(total_epges <= 0);
  config.set_int("epge_count", total_epges);

  // сохранить все EPGE
  for (int epge_idx {}; crauto epge: graphic::epges) {
    cauto epge_name = epge->name();
    cont_if(epge_name.empty());
    
    cauto epge_node_name = Str("EPGE_") + n2s(epge_idx);
    auto epge_node = config.make_node(epge_node_name);

    epge_node.set_str("name", epge->name());

    // сохранить все настройки EPGE:
    cauto epge_params = epge->params();
    cauto params_sz = epge_params.size();
    if (params_sz > 0) {
      epge_node.set_int("param_count", params_sz);

      for (int param_idx {}; crauto param: epge_params) {
        cauto param_node_name = "PARAM_" + n2s(param_idx);
        auto param_node = epge_node.make_node(param_node_name);
        param_node.set_str("title", param->title());
        param_node.set_str("value", param->get_value());
        ++param_idx;
      }
    }

    ++epge_idx;
  }
}

void load_epges(cr<Yaml> config) {
  graphic::epges.clear();
  cauto total_epges = config.get_int("epge_count");
  ret_if(total_epges <= 0);
  assert(total_epges < 999'999);

  // загрузить все EPGE
  cfor (epge_idx, total_epges) {
    cauto epge_node_name = Str("EPGE_") + n2s(epge_idx);
    cauto epge_node = config[epge_node_name];
    cont_if(!epge_node.check());

    cauto epge_name = epge_node.get_str("name");
    cont_if(epge_name.empty());

    auto epge_ptr = make_epge(epge_name);
    cont_if(!epge_ptr);
    auto& epge = graphic::epges.emplace_back(std::move(epge_ptr));

    // загрузить все настройки EPGE:
    cauto total_params = epge_node.get_int("param_count");
    cont_if(total_params <= 0);
    assert(total_params < 999'999);

    auto epge_params = epge->params();
    cfor (param_idx, total_params) {
      const Str param_node_name = "PARAM_" + n2s(param_idx);
      auto param_node = epge_node[param_node_name];
      cont_if(!param_node.check());

      auto& param = epge_params.at(param_idx);
      assert(param);
      cauto param_title = param->title();
      if (param_title == param_node.get_str("title")) {
        param->set_value(param_node.get_str("value"));
      } else {
        hpw_log(Str("Несовпадение параметра \"") + param_title + "\" эффекта \"" +
          epge_name + "\". Параметр проигнорирован\n", Log_stream::warning);
      }
    } // params
  } // epges
}

Strs avaliable_epges() {
  if (::_epge_makers.empty())
    init_epge_list();

  Strs list;
  for (crauto [name, _]: ::_epge_makers)
    list.push_back(name);
    
  assert(!list.empty());
  std::sort(list.begin(), list.end());
  return list;
}

Unique<epge::Base> make_epge(cr<Str> name) {
  if (::_epge_makers.empty())
    init_epge_list();

  try {
    return ::_epge_makers.at(name) (); // создать EPGE
  } catch (...) {}

  hpw_log("не удалось загрузить EPGE эффект \"" + name + "\"\n", Log_stream::warning);
  return {};
}

bool remove_epge(cp<epge::Base> address) {
  // удалить все совпадения с адресом
  cauto deleted_count = std::erase_if (
    graphic::epges,
    [address](cr<Unique<epge::Base>> epge) {
      return epge.get() == address;
    }
  );

  return deleted_count > 0 ? true : false;
}
