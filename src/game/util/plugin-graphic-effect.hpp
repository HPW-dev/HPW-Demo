#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"

class Image;

/// грузить графический эффект из .dll/.so файл
void load_plugin_graphic_effect(Str libname);
/// применяет графический эффект к кадру
void apply_plugin_graphic_effect(Image& dst);
/// выключает текущий графический эффект
void disable_plugin_graphic_effect();
