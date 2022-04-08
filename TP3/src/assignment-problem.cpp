#include "assigment-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>
#include <vector>
#include <random>

const int NO_LAST = -1;

AssignmentProblem::AssignmentProblem(const std::string& path): distribution(7,10) {
    parse_file(path);
}

void AssignmentProblem::parse_file(const std::string& path) {
    // Create a string in which we store each line we read.
    std::string line;

    // Read from the text file.
    std::ifstream file_reader(path);

    // Parse number of sites, atom types, number of edges
    std::getline(file_reader, line);
    std::istringstream stream(line);
    size_t n_atoms, n_atom_types, n_edges;
    stream >> n_atoms >> n_atom_types >> n_edges;


    file_reader.ignore();

    // Parse assignments constraints
    for(size_t i=0; i<n_atom_types; i++){
        std::string occurrences;
        file_reader >> occurrences;
        assignments_contraint.push_back(std::stoi(occurrences));
    }

    file_reader.ignore();

    // Init H matrix
    for(size_t i = 0; i < n_atom_types; i++){
        H.push_back(std::vector<int>(n_atom_types));
    }

    // Parse values of matrix
    for(size_t i=0; i<n_atom_types; i++){
        for(size_t j=0; j<n_atom_types; j++){
            std::string occurrences;
            file_reader >> occurrences;
            H[i][j] = std::stoi(occurrences);
        }  
    }

    file_reader.ignore();

    // Init graph
    for(size_t i=0; i<n_atoms; i++){
        graph.push_back(std::vector<std::shared_ptr<Edge>>());
    }

    // Parse graph
    for(size_t i=0; i<n_edges; i++){
        std::string s;
        std::string e;
        file_reader >> s >> e;
        size_t start = std::stoi(s);
        size_t end = std::stoi(e);

        const auto edge = std::make_shared<Edge>(start, end);
        edges.push_back(edge);
        graph[start].push_back(edge);
        graph[end].push_back(edge);
    }

    // Close the file.
    file_reader.close();
}

Solution AssignmentProblem::get_random_solution(){
    Solution random_solution;

    std::random_device rd;
    std::mt19937 g(rd());
 

    for(size_t i = 0; i < assignments_contraint.size(); i++){
        const uint32_t constraint = assignments_contraint[i]; // combien de type i
        for(size_t j = 0; j < constraint; j++){
            random_solution.assignments.push_back(i);
        }
    }

    std::shuffle(random_solution.assignments.begin(), random_solution.assignments.end(), g);

    random_solution.total_energy = get_total_energy(random_solution.assignments);

    return random_solution;
}

int AssignmentProblem::get_total_energy(const std::vector<uint8_t>& assignments){
    int total_energy = 0;

    for(const auto& edge: edges){
        edge->weight = H[assignments[edge->start]][assignments[edge->end]];
        total_energy += edge->weight;
    }
    return total_energy;
}

Solution AssignmentProblem::tabu_algorithm(bool should_print_results) {
    // Get a greedy solution!
    Solution current_solution = get_random_solution();
    Solution best_solution = current_solution;

    while (true) {
        current_solution = get_best_neighbor_solution(current_solution);  
        if (current_solution.total_energy < best_solution.total_energy && should_print_results) {
            best_solution = current_solution;
            print_results(best_solution);
        }
    }
    
    return best_solution;
}

Solution AssignmentProblem::get_best_neighbor_solution(Solution& current_solution) {
    auto assignments = current_solution.assignments;
    int best_diff = INT32_MAX;
    std::pair<size_t, size_t> best_pair = {};
    
    for(size_t i = 0; i < assignments.size(); i++) {
        for(size_t j = i+1; j < assignments.size(); j++){
            if(assignments[i] == assignments[j]) continue;

            int diff = 0;

            for(const auto& edge: graph[i]){
                const auto other_node = edge->start==i ? edge->end : edge->start;
                if(other_node == j){
                    continue;
                }
                diff += H[assignments[other_node]][assignments[j]] - edge->weight;
            }

            for(const auto& edge: graph[j]){
                const auto other_node = edge->start==i ? edge->end : edge->start;
                if(other_node == i){
                    continue;
                }
                diff += H[assignments[other_node]][assignments[i]] - edge->weight;
            }


            if(diff < best_diff){
                best_pair = {i, j};
                best_diff = diff;
            }
        }
    }

    const auto i = best_pair.first;
    const auto j = best_pair.second;

    for(const auto& edge: graph[i]){
        const auto other_node = edge->start==i ? edge->end : edge->start;
        if(other_node == j){
            continue;
        }
        edge->weight = H[assignments[other_node]][assignments[j]];
    }

    for(const auto& edge: graph[j]){
        const auto other_node = edge->start==j ? edge->end : edge->start;
        if(other_node == i){
            continue;
        }
        edge->weight = H[assignments[other_node]][assignments[i]];
    }

    return {.assignments = assignments, .total_energy = current_solution.total_energy + best_diff};
}

void AssignmentProblem::print_results(const Solution& solution) {
    for (const auto& assignment: solution.assignments) {
        std::cout << (int)assignment << " ";
    }
    std::cout << "[ score : " << solution.total_energy << " ]" << std::endl;
}