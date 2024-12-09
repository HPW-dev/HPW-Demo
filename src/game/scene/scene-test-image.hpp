#pragma once
#include "util/mem-types.hpp"

class Menu_list_item;
class Image;

Shared<Menu_list_item> get_test_image_list(); // пункт меню для выбора тестовых картинок
void draw_test_image(Image& dst); // нарисовать текущее тестовое изображение на экране
