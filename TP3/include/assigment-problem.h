#ifndef ASSIGNMENT_PROBLEM
#define ASSIGNMENT_PROBLEM

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "edge.h"

class AssignmentProblem {
  public:
    AssignmentProblem() {}
    AssignmentProblem(const std::string& path);

    // static void print_results(const Cubes& results);

  private:
    // Solution get_best_neighbor_solution(const Solution& current_solution, std::unordered_map<Cube*, int8_t>& candidates);
    // uint32_t get_candidate_next_neighbor_score(Cube* candidate_cube, const Solution& current_solution);
    // Cubes get_candidate_next_neighbor_cubes(Cube* candidate_cube, const Cubes& current_solution, Cubes& temp_tabu_list);
    
    void parse_file(const std::string& path);
    
    std::default_random_engine generator;
    std::uniform_int_distribution<uint8_t> distribution;

    std::vector<std::vector<Edge>> graph;
    std::vector<Edge> edges;
    std::vector<std::vector<int>> H;
    std::vector<uint8_t> assignments_contraint;
};

#endif
