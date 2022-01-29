#include <string>
#include <vector>
#include <memory>
#include "building.h"

class SkylineProblem {
  public:
    SkylineProblem(): buildings_length(-1) {}
    SkylineProblem(const std::string& path);

    std::vector<Point> naive_algorithm();
    
  private:
    void parse_file(const std::string& path);
    std::vector<std::shared_ptr<Building>> buildings;
    int buildings_length;
};