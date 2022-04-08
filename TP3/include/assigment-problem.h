#ifndef ASSIGNMENT_PROBLEM
#define ASSIGNMENT_PROBLEM

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "edge.h"


struct Solution {  
    std::vector<uint8_t> assignments;  
    int total_energy;
};

class AssignmentProblem {
  public:
    AssignmentProblem() {}
    AssignmentProblem(const std::string& path);

    static void print_results(const Solution& solution);
    Solution tabu_algorithm(bool should_print_results);

  private:
    Solution get_best_neighbor_solution(Solution& current_solution);
    Solution get_random_solution();
    int get_total_energy(const std::vector<uint8_t>& assignments);

    void parse_file(const std::string& path);
    
    std::default_random_engine generator;
    std::uniform_int_distribution<uint8_t> distribution;

    std::vector<std::vector<std::shared_ptr<Edge>>> graph; //
    std::vector<std::shared_ptr<Edge>> edges; //
    std::vector<std::vector<int>> H;
    std::vector<uint32_t> assignments_contraint;
};

#endif
