#pragma once

class Image;

enum class hud_e {
  simple, /// обычный интерфейс с полосками
  asci_1, /// текстовый с @-полосками
};

/// рисует интерфейс игрока
void draw_hud(Image& dst);
