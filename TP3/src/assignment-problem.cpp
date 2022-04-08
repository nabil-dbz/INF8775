#include "assigment-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>
#include <vector>

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
        graph.push_back(std::vector<Edge>());
    }

    // Parse graph
    for(size_t i=0; i<n_edges; i++){
        std::string s;
        std::string e;
        file_reader >> s >> e;
        size_t start = std::stoi(s);
        size_t end = std::stoi(e);

        Edge edge(start, end);
        edges.push_back(edge);
        graph[start].push_back(edge);
        graph[end].push_back(Edge(end, start));
    }

    // Close the file.
    file_reader.close();
}

// Solution AssignmentProblem::tabu_algorithm() {
//     // Get a greedy solution!
//     Solution current_solution = greedy_algorithm();
//     Solution best_solution = current_solution;

//     // for the values in the dict : 
//     // -1 means that the cube is in the current solution
//     //  0 means that it is a candidate cube
//     //  ELSE (greater than 0) means it is a tabu cube and the value is the expiration

//     std::unordered_map<Cube*, int8_t> candidates; 

//     for(const auto& cube: cubes){
//         candidates.insert({cube.get(), 0});
//     }

//     for(const auto& cube: current_solution.cubes){
//         candidates[cube] = -1;
//     }

//     uint8_t counter = 0;
//     const uint8_t STALL_MAX_ITERATIONS = 100;
    
//     while (counter <  STALL_MAX_ITERATIONS) {
//         current_solution = get_best_neighbor_solution(current_solution, candidates);  
//         if (current_solution.score > best_solution.score) {
//             best_solution = current_solution;
//             counter = 0;
//         }
//         counter += 1;
//     }
    
//     return best_solution;
// }

// Solution AssignmentProblem::get_best_neighbor_solution(const Solution& current_solution, std::unordered_map<Cube*, int8_t>& candidates) {

//     uint32_t best_neighbor_score = 0;
//     Cube* best_candidate = nullptr;

//     for (auto candidate_element: candidates) {
//         if (candidate_element.second != 0) continue; // not a candidate

//         Cube* candidate_cube = candidate_element.first;
        
//         uint32_t neighbor_score = get_candidate_next_neighbor_score(candidate_cube, current_solution);

//         if (neighbor_score > best_neighbor_score) {
//             best_neighbor_score = neighbor_score;
//             best_candidate = candidate_cube;
//         }
//     }
    
//     Cubes best_neighbor_tabu_list;
//     Solution best_neighbor_solution;

//     if(best_candidate != nullptr){
//         auto best_neighbor_cubes = get_candidate_next_neighbor_cubes(best_candidate, current_solution.cubes, best_neighbor_tabu_list);
//         best_neighbor_solution = {.cubes = best_neighbor_cubes, .score = best_neighbor_score};

//         // Update current solution elements, only best candidate needs to be changed        
//         candidates[best_candidate] = -1;

//     }else{
//         // When there are no candidates, we return the same solution, but update tabu elements.
//         best_neighbor_solution = current_solution;
//     }
    
//     // Update old tabu elements expiration date
//     for (auto& candidate_element : candidates) {
//         if (candidate_element.second > 0) {
//             candidate_element.second--;
//         }
//     }

//     // Add elements with new expiration date to tabu list
//     for (Cube* tabu_element: best_neighbor_tabu_list) {
//         candidates[tabu_element] = distribution(generator);
//     }

//     return best_neighbor_solution;
// }

// Cubes AssignmentProblem::get_candidate_next_neighbor_cubes(Cube* candidate_cube, const Cubes& current_solution, Cubes& temp_tabu_list){        
//     Cubes neighbor_solution;

//     size_t i = 0;
//     size_t length = current_solution.size();
//     while (i < length && current_solution[i]->can_hold(candidate_cube)) {
//         neighbor_solution.push_back(current_solution[i]);
//         i++;
//     }

//     neighbor_solution.push_back(candidate_cube);
    
//     for(; i < current_solution.size(); i++) {
//         if (candidate_cube->can_hold(current_solution[i])) {
//             neighbor_solution.insert(neighbor_solution.end(), current_solution.begin()+i, current_solution.end());
//             break;
//         }
//         else {
//             temp_tabu_list.push_back(current_solution[i]);
//         }
//     }

//     return neighbor_solution;
// }

// uint32_t AssignmentProblem::get_candidate_next_neighbor_score(Cube* candidate_cube, const Solution& current_solution){        

//     auto cubes = current_solution.cubes;
    
//     size_t i = 0;
//     size_t length = cubes.size();
//     while (i < length && cubes[i]->can_hold(candidate_cube)) {
//         i++;
//     }

//     uint32_t tabu_cubes_height = 0;
//     while (i < length && !candidate_cube->can_hold(cubes[i])){
//         tabu_cubes_height += cubes[i]->height;
//         i++;
//     }
    
//     return current_solution.score + candidate_cube->height - tabu_cubes_height;
// }

// void AssignmentProblem::print_results(const Cubes& results) {
//     for (const Cube* cube: results) {
//         std::cout << *cube << std::endl;
//     }
// }