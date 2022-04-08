#ifndef EDGE
#define EDGE

#include <stdint.h>
#include <iostream>

class Edge {
  public:
    Edge(size_t start, size_t end): 
      start(start), end(end) {}

    // friend std::ostream& operator<<(std::ostream& os, const Cube& cube){
    //    os << cube.height << " " << cube.width << " " << cube.depth;
    //    return os;
    // }

    size_t start;
    size_t end;
};

#endif
