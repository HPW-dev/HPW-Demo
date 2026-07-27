#pragma once
#include "util/vector-types.hpp"

struct Circle;
struct Polygon;

// сделать круг полностью перекрывающий все полигоны
Circle cover_polygons(const Vector<Polygon>& polygons);
