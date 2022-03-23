#ifndef HIGHEST_TOWER_PROBLEM
#define HIGHEST_TOWER_PROBLEM

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "cube.h"

struct Solution {  
    std::vector<Cube*> cubes;
    uint32_t score;
};

class HighestTowerProblem {
  public:
    HighestTowerProblem() {}
    HighestTowerProblem(const std::string& path);

    Solution greedy_algorithm();
    Solution dynamic_programming_algorithm();
    Solution tabu_algorithm();
    Solution tabu_algorithm_v2();

    static void print_results(const std::vector<Cube*>& results);

  private:
    Solution get_best_neighbor_solution(const std::vector<Cube*>& current_solution, std::unordered_map<Cube*, int8_t>& candidates);
    Solution get_candidate_next_neighbor(Cube* candidate_cube, const std::vector<Cube*>& current_solution, std::vector<Cube*>& temp_tabu_list);
    
    Solution get_best_neighbor_solution_v2(const std::vector<Cube*>& current_solution, std::unordered_map<Cube*, int8_t>& tabu_elements, std::unordered_set<Cube*>& candidates);

    
    void parse_file(const std::string& path);
    std::vector<Cube*> get_path_solution(int first_cube_index, int first_predecessor_index, const std::vector<int>& predecessors);
    
    std::vector<std::shared_ptr<Cube>> cubes;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
};

#endif
