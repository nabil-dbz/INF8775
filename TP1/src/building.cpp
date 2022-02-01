#include "building.h"

std::pair<Point, Point> Building::get_critical_points(){
    return std::make_pair(Point(left, height), Point(right, 0));
}

bool Building::includes(Point point){
    return point.x >= left && point.x < right && point.y < height;
}
