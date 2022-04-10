#ifndef EDGE
#define EDGE

#include <stdint.h>
#include <iostream>

class Edge {
  public:
    Edge(size_t start, size_t end): 
      start(start), end(end) {}

    size_t get_other_node(size_t me){
      return start==me ? end : start;
    }

    size_t start;
    size_t end;
    int weight;
};

#endif
