#include <string>
#include <vector>
#include <memory>
#include "building.h"

class SkylineProblem {
  public:
    SkylineProblem(): buildings_length(-1), recursive_algorithm_cutoff(0) {}
    SkylineProblem(const std::string& path);

    std::vector<Point> naive_algorithm();
    std::vector<Point> recursive_algorithm();
    std::vector<Point> recursive_cutoff_algorithm(const uint32_t& cutoff);

    void print_results(const std::vector<Point>& results);

  private:
    std::vector<Point> brute_force_algorithm(const std::vector<std::shared_ptr<Building>> buildings);
    std::vector<Point> divide_and_conquer_cutoff_algorithm(const std::vector<std::shared_ptr<Building>> buildings);
    std::vector<Point> merge_buildings(std::vector<Point> first_half, std::vector<Point> second_half);
    void parse_file(const std::string& path);
    
    std::vector<std::shared_ptr<Building>> buildings;
    uint32_t buildings_length;
    uint32_t recursive_algorithm_cutoff;
};