#ifndef CUBE
#define CUBE

#include <stdint.h>
#include <iostream>

class Cube {
  public:
    Cube(uint32_t height, uint32_t width, uint32_t depth): 
      height(height), width(width), depth(depth) {}

    bool can_hold(const Cube* other_cube) const {
      return width > other_cube->width && depth > other_cube->depth;
    }

    uint32_t get_base_area() {
      return width * depth;
    }

    friend std::ostream& operator<<(std::ostream& os, const Cube& cube){
       os << cube.height << " " << cube.width << " " << cube.depth;
       return os;
    }

    uint32_t height;
    uint32_t width;
    uint32_t depth;
};

#endif
