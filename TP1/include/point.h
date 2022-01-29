#include <iostream>

class Point
{
    public:
        Point(uint32_t x, uint32_t y){
            this->x = x;
            this->y = y;
        }
        friend std::ostream& operator<<(std::ostream& os, const Point& point);
        double x, y;
};

