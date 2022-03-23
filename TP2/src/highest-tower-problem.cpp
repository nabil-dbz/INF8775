#include "highest-tower-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>


const int NO_LAST = -1;

HighestTowerProblem::HighestTowerProblem(const std::string& path): distribution(7,10) {
    parse_file(path);
}

void HighestTowerProblem::parse_file(const std::string& path) {
    // Create a string in which we store each line we read.
    std::string line;

    // Read from the text file.
    std::ifstream file_reader(path);

    // Use a while loop together with the getline() function to read the file line by line.
    while (std::getline(file_reader, line)) {
        std::istringstream stream(line);
        uint32_t height, width, depth;
        stream >> height >> width >> depth;
        cubes.push_back(std::make_shared<Cube>(height, width, depth));
    }

    // Close the file.
    file_reader.close();
}

Solution HighestTowerProblem::greedy_algorithm() {
    Cubes results;

    // Sort cubes accordingly based on area.
    const auto compare = [](const std::shared_ptr<Cube>& cube_a, const std::shared_ptr<Cube>& cube_b) { 
        return cube_a->get_base_area() > cube_b->get_base_area(); 
    };
    std::sort(cubes.begin(), cubes.end(), compare);

    uint32_t height = 0;
    for(const std::shared_ptr<Cube>& cube: cubes){
        if(results.empty()){
            results.push_back(cube.get());
            height += cube->height;
            continue;
        }

        if (results.back()->can_hold(cube.get())) {
            results.push_back(cube.get());
            height += cube->height;
        }
    }

    return { results, height };
}

Solution HighestTowerProblem::dynamic_programming_algorithm() {    
    // Sort cubes accordingly.
    const auto compare = [](const std::shared_ptr<Cube>& cube_a, const std::shared_ptr<Cube>& cube_b) { 
        return cube_a->get_base_area() > cube_b->get_base_area(); 
    };
    std::sort(cubes.begin(), cubes.end(), compare);

    std::vector<uint32_t> max_heights;
    std::vector<int> predecessors;

    uint32_t max_height_result = 0;
    int max_predecessor_index = NO_LAST; 
    int top_cube_index = 0;

    for (size_t i = 0; i < cubes.size(); i++) {
        Cube* cube = cubes[i].get();
        uint32_t max_height = 0;
        int max_index = NO_LAST;
        
        for(size_t j = 0; j < i; j++) {
            if (cubes[j]->can_hold(cube) && max_heights[j] > max_height){
                max_index = j;
                max_height = max_heights[j];
            }
        }
        
        const uint32_t new_height = max_height + cube->height;
        max_heights.push_back(new_height);
        predecessors.push_back(max_index);
        
        if (new_height > max_height_result) {
            max_height_result = new_height;
            max_predecessor_index = max_index;
            top_cube_index = i;
        }
    }

    return { get_path_solution(top_cube_index, max_predecessor_index, predecessors), max_height_result };
}

Solution HighestTowerProblem::tabu_algorithm() {
    // Get a greedy solution!
    Solution current_solution = greedy_algorithm();
    Solution best_solution = current_solution;

    // for the values in the dict : 
    // -1 means that the cube is in the current solution
    //  0 means that it is a candidate cube
    //  ELSE (greater than 0) means it is a tabu cube and the value is the expiration

    std::unordered_map<Cube*, int8_t> candidates; 

    for(const auto& cube: cubes){
        candidates.insert({cube.get(), 0});
    }

    for(const auto& cube: current_solution.cubes){
        candidates[cube] = -1;
    }

    uint8_t counter = 0;
    const uint8_t STALL_MAX_ITERATIONS = 100;
    
    while (counter <  STALL_MAX_ITERATIONS) {
        current_solution = get_best_neighbor_solution(current_solution, candidates);  
        if (current_solution.score > best_solution.score) {
            best_solution = current_solution;
            counter = 0;
        }
        counter += 1;
    }
    
    return best_solution;
}

Solution HighestTowerProblem::get_best_neighbor_solution(const Solution& current_solution, std::unordered_map<Cube*, int8_t>& candidates) {

    uint32_t best_neighbor_score = 0;
    Cube* best_candidate = nullptr;

    for (auto candidate_element: candidates) {
        if (candidate_element.second != 0) continue; // not a candidate

        Cube* candidate_cube = candidate_element.first;
        
        uint32_t neighbor_score = get_candidate_next_neighbor_score(candidate_cube, current_solution);

        if (neighbor_score > best_neighbor_score) {
            best_neighbor_score = neighbor_score;
            best_candidate = candidate_cube;
        }
    }
    
    
    Cubes best_neighbor_tabu_list;
    Solution best_neighbor_solution;

    if(best_candidate != nullptr){
        auto best_neighbor_cubes = get_candidate_next_neighbor_cubes(best_candidate, current_solution.cubes, best_neighbor_tabu_list);
        best_neighbor_solution = {.cubes = best_neighbor_cubes, .score = best_neighbor_score};
    }else{
        // When there are no candidates, we return the same solution, but update tabu elements.
        best_neighbor_solution = current_solution;
    }
    
    // Update old tabu elements expiration date
    for (auto& candidate_element : candidates) {
        if (candidate_element.second > 0) {
            candidate_element.second--;
        }
    }

    // Add elements with new expiration date to tabu list
    for (Cube* tabu_element: best_neighbor_tabu_list) {
        candidates[tabu_element] = distribution(generator);
    }
    
    // Update current solution elements, only best candidate needs to be changed
    candidates[best_candidate] = -1;
    

    return best_neighbor_solution;
}

Cubes HighestTowerProblem::get_candidate_next_neighbor_cubes(Cube* candidate_cube, const Cubes& current_solution, Cubes& temp_tabu_list){        
    Cubes neighbor_solution;

    size_t i = 0;
    size_t length = current_solution.size();
    while (i < length && current_solution[i]->can_hold(candidate_cube)) {
        neighbor_solution.push_back(current_solution[i]);
        i++;
    }

    neighbor_solution.push_back(candidate_cube);
    
    for(; i < current_solution.size(); i++) {
        if (candidate_cube->can_hold(current_solution[i])) {
            neighbor_solution.insert(neighbor_solution.end(), current_solution.begin()+i, current_solution.end());
            break;
        }
        else {
            temp_tabu_list.push_back(current_solution[i]);
        }
    }

    return neighbor_solution;
}

uint32_t HighestTowerProblem::get_candidate_next_neighbor_score(Cube* candidate_cube, const Solution& current_solution){        

    auto cubes = current_solution.cubes;
    
    size_t i = 0;
    size_t length = cubes.size();
    while (i < length && cubes[i]->can_hold(candidate_cube)) {
        i++;
    }

    uint32_t tabu_cubes_height = 0;
    while (i < length && !candidate_cube->can_hold(cubes[i])){
        tabu_cubes_height += cubes[i]->height;
        i++;
    }
    
    return current_solution.score + candidate_cube->height - tabu_cubes_height;
}

void HighestTowerProblem::print_results(const Cubes& results) {
    for (const Cube* cube: results) {
        std::cout << *cube << std::endl;
    }
}

Cubes HighestTowerProblem::get_path_solution(int first_cube_index, int first_predecessor_index, const std::vector<int>& predecessors) {
    std::deque<Cube*> elements;
    elements.push_front(cubes[first_cube_index].get());

    while (first_predecessor_index != NO_LAST) {
        elements.push_front(cubes[first_predecessor_index].get());
        first_predecessor_index = predecessors[first_predecessor_index];
    }

    return Cubes({elements.begin(), elements.end()});
}