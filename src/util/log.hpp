#pragma once
#include <source_location>
#include <string_view>

void hpw_log(const std::string_view msg, const std::source_location location = std::source_location::current());
void iflog(const bool cond, const std::string_view msg, const std::source_location location = std::source_location::current());
void detailed_log(const std::string_view msg, const std::source_location location = std::source_location::current());
void detailed_iflog(const bool cond, const std::string_view msg, const std::source_location location = std::source_location::current());
