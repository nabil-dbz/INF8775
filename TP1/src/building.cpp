#include "building.h"

std::pair<Point, Point> Building::get_critical_points(){
    const Point first = Point(left, height);
    const Point second = Point(right, 0);
    return std::make_pair(first, second);
};