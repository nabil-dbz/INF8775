#include <stdint.h>
#include "point.h"
#include <list>

class Building {
  public:
    Building(uint32_t left, uint32_t right, uint32_t height) {
      this->left = left;
      this->right = right;
      this->height = height;
    }
    std::pair<Point, Point> get_critical_points();
    bool includes(Point point);
    uint32_t left;
    uint32_t right;
    uint32_t height;
};
