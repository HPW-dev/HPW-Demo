# Как добавить новый эффект
создать в папке epge один .cpp файл для эффекта по шаблону:
```cpp
#include "pch.hpp"
#include "epge.hpp"
#include "game/util/locale.hpp" // это нужное

EPGE_CLASS_BEGIN(__название_класса_эффекта__)
  // переменные для эффекта

public:
  void draw(Image& dst) const {
    // код эффекта
  }

  // если надо: void update(const Delta_time dt) {...}

  Params params() { return Params {
    /* Перечисли все параметры. Пример:
    EPGE_PARAM_INT(mode, dither_blue_noise, _mode, 0, 2, 1, 1)
    EPGE_PARAM_BOOL(use_gamma_corr, dither_blue_noise, _use_gamma_corr)
    EPGE_PARAM_BOOL(use_jitter, dither_blue_noise, _use_jitter)
    
    Потом этот use_jitter будет искаться в .json-файле
    русика (тег epge->effect->__название_класса_эффекта__) */
  }; }
EPGE_CLASS_END(__название_класса_эффекта__)
```

Как локализировать эффекта:
```тег epge->effect->__название_класса_эффекта__```
```json
"__название_класса_эффекта__": {
  "name": "имя_на_твоём_языке",
  "desc": "описание",
  "param": {
    "название_параметра": {
      "name": "режим",
      "desc": "0 - быстрый; 1 - аккуратный;"
    },
    "название_параметра_2": {
      "name": "джиттер",
      "desc": "Для перемешивания узора"
    }
  }
}
```