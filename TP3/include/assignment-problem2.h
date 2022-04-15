#ifndef ASSIGNMENT_PROBLEM2
#define ASSIGNMENT_PROBLEM2

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "thread-pool.hpp"


struct Solution2 {  
    std::vector<uint8_t> node_assignments;
    int total_energy;
};

class AssignmentProblem2 {
  public:
    AssignmentProblem2() {}
    AssignmentProblem2(const std::string& path);

    void print_results(const Solution2& solution);
    Solution2 tabu_algorithm(bool should_print_results);
    Solution2 simulated_annealing(bool should_print_results);

  private:
    Solution2 get_initial_solution();
    Solution2 get_greedy_solution();
    Solution2 get_random_solution();

    Solution2 get_best_neighbor_solution(Solution2& current_solution, std::unordered_map<size_t, int8_t>& tabu);
    Solution2 get_best_neighbor_solution2(Solution2& current_solution);

    int get_node_new_energy_diff(const Solution2& solution, size_t node, size_t swap_node);
    int get_total_energy(const std::vector<uint8_t>& assignments);

    double temperature_function(double counter);

    void parse_file(const std::string& path);

    std::vector<std::vector<size_t>> graph;
    std::vector<std::vector<int>> H;
    std::vector<uint32_t> assignments_constraint;
    thread_pool pool;

};

#endif
