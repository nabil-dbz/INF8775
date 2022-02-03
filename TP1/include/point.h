#include <iostream>

class Point
{
    public:
        Point(): x(0), y(0) {}

        Point(uint32_t x, uint32_t y){
            this->x = x;
            this->y = y;
        }
        
        bool operator<(const Point& point);
        bool operator==(const Point& point);
        friend std::ostream& operator<<(std::ostream& os, const Point& point);
        double x, y;
};

