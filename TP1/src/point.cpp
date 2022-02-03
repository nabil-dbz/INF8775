#include "point.h"

bool Point::operator<(const Point& point) {
    return x == point.x ? y > point.y : x < point.x;
}

bool Point::operator==(const Point& point) {
    return x == point.x && y == point.y;
}

std::ostream& operator<<(std::ostream& os, const Point& point){
    os << point.x << "  " << point.y;
    return os;
}
