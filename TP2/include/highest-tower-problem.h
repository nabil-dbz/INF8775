#ifndef HIGHEST_TOWER_PROBLEM
#define HIGHEST_TOWER_PROBLEM

#include <string>
#include <vector>
#include <memory>
#include "cube.h"

class HighestTowerProblem {
  public:
    HighestTowerProblem() {}
    HighestTowerProblem(const std::string& path);

    std::vector<Cube*> greedy_algorithm();
    std::vector<Cube*> dynamic_programming_algorithm();
    std::vector<Cube*> taboo_algorithm();

    static void print_results(const std::vector<Cube*>& results);

  private:
    // std::vector<Cube> brute_force_algorithm(const std::vector<std::shared_ptr<Cube>> cubes);
    // std::vector<Point> divide_and_conquer_cutoff_algorithm(const std::vector<std::shared_ptr<Building>> buildings);
    // std::vector<Point> merge_buildings(std::vector<Point> first_half, std::vector<Point> second_half);
    void parse_file(const std::string& path);
    std::vector<Cube*> get_path_solution(int first_cube_index, int first_predecessor_index, const std::vector<int>& predecessors);
    
    std::vector<std::shared_ptr<Cube>> cubes;
};

#endif
