#ifndef ASSIGNMENT_PROBLEM
#define ASSIGNMENT_PROBLEM

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "edge.h"


struct Solution2 {  
    std::vector<uint8_t> node_assignments;
    std::vector<int> node_energies;
    int total_energy;
};

class AssignmentProblem2 {
  public:
    AssignmentProblem2() {}
    AssignmentProblem2(const std::string& path);

    void print_results(const Solution2& solution);
    Solution2 tabu_algorithm(bool should_print_results);

  private:
    Solution2 get_greedy_solution(size_t s);
    Solution2 get_best_neighbor_solution(Solution2& current_solution, std::unordered_map<size_t, int8_t>& tabu);
    int get_node_new_energy_diff(const Solution2& solution, size_t node, size_t swap_node);
    void update_node_energies(Solution2& solution, size_t node, size_t swap_node);

    void parse_file(const std::string& path);

    std::vector<std::vector<size_t>> graph;
    std::vector<std::vector<int>> H;
    std::vector<uint32_t> assignments_constraint;
};

#endif
