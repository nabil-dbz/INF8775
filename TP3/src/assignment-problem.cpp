#include "assigment-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>
#include <vector>
#include <random>

AssignmentProblem::AssignmentProblem(const std::string& path) {
    parse_file(path);
    std::srand(time(0));
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

Solution AssignmentProblem::get_greedy_solution(size_t s){
    // Mark all the vertices as not visited
    std::vector<int8_t> assignments(graph.size());
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
        for(auto& edge: graph[s]){
            const auto other_node = edge->get_other_node(s);
            if (assignments[other_node] == -1){
                
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
                assignments[other_node] = min_type;
                assignments_constraint[min_type]--;
                queue.push_back(other_node);
            }
        }
    }

    const auto res_assignments = std::vector<uint8_t>(assignments.begin(), assignments.end());
    const auto res_total_energy = get_total_energy(res_assignments);
    return {.assignments=res_assignments, .total_energy=res_total_energy};
}

Solution AssignmentProblem::get_random_solution(){
    Solution random_solution;

    std::random_device rd;
    std::mt19937 g(rd());
 
    for(size_t i = 0; i < assignments_constraint.size(); i++){
        const uint32_t constraint = assignments_constraint[i]; // combien de type i
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
    // Get a random solution!
    // Solution current_solution = get_random_solution();

    // Get a greedy solution
    const size_t start_node = std::rand() % graph.size();
    Solution current_solution = get_greedy_solution(start_node);

    print_results(current_solution);

    Solution best_solution = current_solution;
    
    // tabu list
    std::unordered_map<size_t, int8_t> tabu; 
    
    while (true) {
        current_solution = get_best_neighbor_solution(current_solution, tabu);  
        if (current_solution.total_energy < best_solution.total_energy && should_print_results) {
            best_solution = current_solution;
            print_results(best_solution);
        }
    }
    
    return best_solution;
}

Solution AssignmentProblem::get_best_neighbor_solution(Solution& current_solution, std::unordered_map<size_t, int8_t>& tabu) {
    auto assignments = current_solution.assignments;
    int best_diff = INT32_MAX;
    std::pair<size_t, size_t> best_pair = {};

    for(size_t i = 0; i < assignments.size(); i++) {
        for(size_t j = i+1; j < assignments.size(); j++){
            if(assignments[i] == assignments[j]) continue;
            if (tabu.find(i+j) != tabu.end()) continue;

            int diff = 0;

            diff += get_node_new_energy_diff(assignments, i, j);
            diff += get_node_new_energy_diff(assignments, j, i);

            if(diff < best_diff){
                best_pair = {i, j};
                best_diff = diff;
            }
        }
    }

    // add swapping elements to tabu list
    tabu[best_pair.first+best_pair.second] = 10 + std::rand() % 4;

    // suppress elements
    for (auto el = tabu.begin(); el != tabu.end(); /**/) {
        if (el->second - 1 == 0) {
            tabu.erase(el++);
        } else {
            el->second = el->second - 1;
            ++el;
        }
    }

    std::swap(assignments[best_pair.first], assignments[best_pair.second]);

    update_node_edges(best_pair.first, assignments);
    update_node_edges(best_pair.second, assignments);

    return {.assignments = assignments, .total_energy = current_solution.total_energy + best_diff};
}

void AssignmentProblem::update_node_edges(size_t node, const std::vector<uint8_t>& assignments){
    for(const auto& edge: graph[node]){
        edge->weight = H[assignments[edge->start]][assignments[edge->end]];
    }
}

int AssignmentProblem::get_node_new_energy_diff(const std::vector<uint8_t>& assignments, size_t node, size_t swap_node){
    int diff = 0;
    for(const auto& edge: graph[node]){
        const auto other_node = edge->get_other_node(node);
        if(other_node == swap_node){
            continue;
        }
        diff += H[assignments[other_node]][assignments[swap_node]] - edge->weight;
    }
    return diff;
}

void AssignmentProblem::print_results(const Solution& solution) {
    for (const auto& assignment: solution.assignments) {
        std::cout << (int)assignment << " ";
    }
    std::cout << "[ score : " << solution.total_energy << " ]" << std::endl;
    // std::cout << "[ score vrai : " << get_total_energy(solution.assignments) << " ]" << std::endl;
}