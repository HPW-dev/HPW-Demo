#include <unordered_map>
#include "difficulty-helper.hpp"
#include "locale.hpp"

utf32 difficulty_to_str(const Difficulty difficulty) {
  static const std::unordered_map<Difficulty, utf32> table {
    {Difficulty::easy, get_locale_str("scene.difficulty_select.difficulty.easy")},
    {Difficulty::normal, get_locale_str("scene.difficulty_select.difficulty.normal")},
    {Difficulty::hardcore, get_locale_str("scene.difficulty_select.difficulty.hardcore")},
  };
  
  return table.at(difficulty);
}
