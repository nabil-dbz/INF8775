#include "assignment-problem2.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>
#include <vector>
#include <random>

AssignmentProblem2::AssignmentProblem2(const std::string& path) {
    parse_file(path);
    std::srand(time(0));
}

void AssignmentProblem2::parse_file(const std::string& path) {
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
        assignments_constraint.push_back(std::stoi(occurrences));
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
        graph.push_back(std::vector<size_t>());
    }

    // Parse graph
    for(size_t i=0; i<n_edges; i++){
        std::string s;
        std::string e;
        file_reader >> s >> e;
        size_t start = std::stoi(s);
        size_t end = std::stoi(e);

        graph[start].push_back(end);
        graph[end].push_back(start);
    }

    // Close the file.
    file_reader.close();
}

Solution2 AssignmentProblem2::get_greedy_solution(size_t s){
    // Init solution
    std::vector<int8_t> assignments(graph.size());
    std::vector<int> node_energies(graph.size());
    int total_energy = 0;

    for(size_t i = 0; i < graph.size(); i++)
        assignments[i] = -1;
 
    // Create a queue for BFS
    std::deque<size_t> queue;

    // Mark the current node as visited and enqueue it
    queue.push_back(s);

    // affect random atom type to the start node
    const uint8_t atom_type = std::rand() % assignments_constraint.size();
    assignments[s] = atom_type;
    assignments_constraint[atom_type]--;

    while(!queue.empty())
    {
        // Dequeue a vertex from queue
        s = queue.front();
        queue.pop_front();
 
        // Assign types to nodes
        for(const auto& neighbor: graph[s]){
            if (assignments[neighbor] == -1){
                // real algo.
                int min_type = -1;
                int min_interaction = INT32_MAX;
                for(size_t atom_type = 0; atom_type < assignments_constraint.size(); atom_type++){
                    if(assignments_constraint[atom_type] == 0) continue;
                    if(H[atom_type][assignments[s]] < min_interaction){
                        min_interaction = H[atom_type][assignments[s]];
                        min_type = atom_type;
                    }
                }
                assignments[neighbor] = min_type;
                assignments_constraint[min_type]--;
                queue.push_back(neighbor);
            }
            const auto interaction_energy = H[assignments[neighbor]][assignments[s]];
            node_energies[s] += H[assignments[neighbor]][assignments[s]];
            total_energy += interaction_energy;
        }
    }

    const auto res_assignments = std::vector<uint8_t>(assignments.begin(), assignments.end());
    return {.node_assignments=res_assignments, .node_energies = node_energies, .total_energy=total_energy};
}

Solution2 AssignmentProblem2::tabu_algorithm(bool should_print_results) {
    // Get a greedy solution
    const size_t start_node = std::rand() % graph.size();
    Solution2 current_solution = get_greedy_solution(start_node);

    print_results(current_solution);
    // return current_solution;

    Solution2 best_solution = current_solution;
    
    // tabu list
    std::unordered_map<size_t, int8_t> tabu; 
    
    while (true) {
        current_solution = get_best_neighbor_solution(current_solution, tabu);  
        if (current_solution.total_energy < best_solution.total_energy && should_print_results) {
            best_solution = current_solution;
            print_results(best_solution);
        }
    }
}

Solution2 AssignmentProblem2::get_best_neighbor_solution(Solution2& current_solution, std::unordered_map<size_t, int8_t>& tabu) {
    auto assignments = current_solution.node_assignments;
    int best_diff = INT32_MAX;
    std::pair<size_t, size_t> best_pair = {};
    std::pair<int, int> best_diffs = {};

    // parallelize here: https://github.com/bshoshany/thread-pool
    const auto n_nodes = std::min((size_t)5, assignments.size());

    for (size_t j = 0; j < n_nodes; j++){
        const size_t node = std::rand() % assignments.size();
        for(size_t i = 0; i < assignments.size(); i++) {
            if( i == node) continue;
            if(assignments[i] == assignments[node]) continue;
            if (tabu.find(i+node) != tabu.end()) continue;

            int energy_diff_node = get_node_new_energy_diff(current_solution, node, i);

            int energy_diff_swap_node = get_node_new_energy_diff(current_solution, i, node);

            if(energy_diff_node +  energy_diff_swap_node < best_diff){
                best_pair = {node, i};
                best_diffs = {energy_diff_node, energy_diff_swap_node};
                best_diff = energy_diff_node +  energy_diff_swap_node;
            }
        }
    }

    // add swapping elements to tabu list
    tabu[best_pair.first+best_pair.second] = 4 + std::rand() % 4;

    // suppress elements
    for (auto el = tabu.begin(); el != tabu.end(); /**/) {
        if (el->second - 1 == 0) {
            tabu.erase(el++);
        } else {
            el->second = el->second - 1;
            ++el;
        }
    }

    current_solution.node_energies[best_pair.first] += best_diffs.first;
    current_solution.node_energies[best_pair.second] += best_diffs.second;

    update_node_energies(current_solution, best_pair.first, best_pair.first);
    update_node_energies(current_solution, best_pair.second, best_pair.second);
    
    std::swap(assignments[best_pair.first], assignments[best_pair.second]);

    return {.node_assignments = assignments, .node_energies = current_solution.node_energies, .total_energy = current_solution.total_energy + 2*best_diff};
}

void AssignmentProblem2::update_node_energies(Solution2& solution, size_t node, size_t swap_node){
    const auto assignments = solution.node_assignments;
    for(const auto& neighbor: graph[node]){
        if(neighbor == swap_node) continue;
        solution.node_energies[neighbor] += H[assignments[swap_node]][assignments[neighbor]] - H[assignments[node]][assignments[neighbor]];
    }
}

int AssignmentProblem2::get_node_new_energy_diff(const Solution2& solution, size_t node, size_t swap_node){
    const auto assignments = solution.node_assignments;
    int diff = 0;
    for(const auto& neighbor: graph[node]){
        if(neighbor == swap_node) continue;
        diff += H[assignments[neighbor]][assignments[swap_node]] - H[assignments[neighbor]][assignments[node]];
    }
    return diff;
}

void AssignmentProblem2::print_results(const Solution2& solution) {
    for (const auto& assignment: solution.node_assignments) {
        std::cout << (int)assignment << " ";
    }
    std::cout << "[ score : " << solution.total_energy / 2 << " ]" << std::endl;
    // std::cout << "[ score autre: " << std::accumulate(solution.node_energies.begin(), solution.node_energies.end(), 0) / 2 << " ]" << std::endl;

    // std::cout << "[ score vrai : " << get_total_energy(solution.assignments) << " ]" << std::endl;
}