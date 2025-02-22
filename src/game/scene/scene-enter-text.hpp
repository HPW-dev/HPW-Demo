#pragma once
#include <functional>
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "scene.hpp"

// ввод текстовых строк
class Scene_enter_text final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  using Text_processor = std::function<void (cr<utf32>)>; // для обработки введёного текста

  explicit Scene_enter_text(cr<utf32> title, cr<utf32> desc, cr<utf32> edit_title,
    cr<utf32> stast_text, cr<Text_processor> proc);
  ~Scene_enter_text();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "enter text"; }
};
