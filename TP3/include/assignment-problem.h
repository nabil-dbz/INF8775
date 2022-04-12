#ifndef ASSIGNMENT_PROBLEM
#define ASSIGNMENT_PROBLEM

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "edge.h"
#include "thread-pool.hpp"


struct Solution {  
    std::vector<uint8_t> assignments;  
    int total_energy;
};

class AssignmentProblem {
  public:
    AssignmentProblem() {}
    AssignmentProblem(const std::string& path);

    void print_results(const Solution& solution);
    Solution tabu_algorithm(bool should_print_results);

  private:
    Solution get_greedy_solution(size_t s);
    Solution get_best_neighbor_solution(Solution& current_solution, std::unordered_map<size_t, int8_t>& tabu);
    Solution get_random_solution();
    int get_total_energy(const std::vector<uint8_t>& assignments);
    int get_node_new_energy_diff(const std::vector<uint8_t>& assignments, size_t node, size_t swap_node);

    void parse_file(const std::string& path);

    std::vector<std::vector<size_t>> graph;
    std::vector<std::vector<int>> H;
    std::vector<uint32_t> assignments_constraint;
    std::vector<std::pair<size_t, size_t>> candidates;
    thread_pool pool;
};

#endif
