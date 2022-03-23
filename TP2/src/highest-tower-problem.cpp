#include "highest-tower-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>

typedef std::vector<Cube*> Cubes;

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

    // Sort cubes accordingly.
    // TODO: find a way to generalize this by passing a comparing function as an argument.
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
        const auto next_neighbor_solution = get_best_neighbor_solution(current_solution.cubes, candidates);  
        if (next_neighbor_solution.score > current_solution.score) {
            current_solution = next_neighbor_solution;
            counter = 0;
        }
        counter += 1;
    }
    
    return current_solution;
}

Solution HighestTowerProblem::tabu_algorithm_v2() {
    // Get a greedy solution!
    Solution current_solution = greedy_algorithm();

    // for the values in the dict : 
    // -1 means that the cube is in the current solution
    //  0 means that it is a candidate cube
    //  ELSE (greater than 0) means it is a tabu cube and the value is the expiration

    std::unordered_map<Cube*, int8_t> tabu_elements;
    std::unordered_set<Cube*> candidates;

    for(const auto& cube: cubes){
        candidates.insert(cube.get());
    }

    for(const auto& cube: current_solution.cubes){
        candidates.erase(cube);
    }


    uint8_t counter = 0;
    const uint8_t STALL_MAX_ITERATIONS = 100;
    
    while (counter <  STALL_MAX_ITERATIONS) {
        const auto next_neighbor_solution = get_best_neighbor_solution_v2(current_solution.cubes, tabu_elements, candidates);  
        if (next_neighbor_solution.score > current_solution.score) {
            current_solution = next_neighbor_solution;
            counter = 0;
        }
        counter += 1;
    }
    
    return current_solution;
}

Solution HighestTowerProblem::get_best_neighbor_solution_v2(const std::vector<Cube*>& current_solution, std::unordered_map<Cube*, int8_t>& tabu_elements, std::unordered_set<Cube*>& candidates) {

    Solution best_neighbor_solution = {{}, 0};
    Cubes best_neighbor_tabu_list;
    Cube* best_candidate;

    for (auto candidate_cube: candidates) {

        Cubes temp_tabu_list;

        Solution neighbor_solution = get_candidate_next_neighbor(candidate_cube, current_solution, temp_tabu_list);

        if (neighbor_solution.score > best_neighbor_solution.score) {
            best_neighbor_solution = neighbor_solution;
            best_neighbor_tabu_list = temp_tabu_list;
            best_candidate = candidate_cube;
        }
    }
    
    candidates.erase(best_candidate);

    // Add elements with new expiration date to tabu list
    for (Cube* tabu_cube: best_neighbor_tabu_list) {
        tabu_elements.insert_or_assign(tabu_cube, 1 + distribution(generator));
    }
    
    // Update old tabu elements expiration date
    for (auto& tabu_element : tabu_elements) {
        if(tabu_element.second - 1 == 0){
            tabu_elements.erase(tabu_element.first);
            candidates.insert(tabu_element.first);
        }
        else{
            tabu_element.second--;
        }
    }

    return best_neighbor_solution;
}

Solution HighestTowerProblem::get_best_neighbor_solution(const Cubes& current_solution, std::unordered_map<Cube*, int8_t>& candidates) {

    Solution best_neighbor_solution = {{}, 0};
    Cubes best_neighbor_tabu_list;

    for (auto candidate_element: candidates) {
        if (candidate_element.second != 0) continue; // not a candidate

        Cube* candidate_cube = candidate_element.first;
        
        Cubes temp_tabu_list;

        Solution neighbor_solution = get_candidate_next_neighbor(candidate_cube, current_solution, temp_tabu_list);

        if (neighbor_solution.score > best_neighbor_solution.score) {
            best_neighbor_solution = neighbor_solution;
            best_neighbor_tabu_list = temp_tabu_list;
        }
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
    
    // Update current solution elements
    for(const auto& cube: best_neighbor_solution.cubes){
        candidates[cube] = -1;
    }

    return best_neighbor_solution;
}

Solution HighestTowerProblem::get_candidate_next_neighbor(Cube* candidate_cube, const Cubes& current_solution, Cubes& temp_tabu_list){        
    Cubes neighbor_solution;
    uint32_t neighbor_score = 0;

    size_t i = 0;
    while (current_solution[i]->can_hold(candidate_cube)) {
        neighbor_solution.push_back(current_solution[i]);
        neighbor_score += current_solution[i]->height;
        i++;
    }

    neighbor_solution.push_back(candidate_cube);
    neighbor_score += candidate_cube->height;
    
    for(; i < current_solution.size(); i++) {
        if (neighbor_solution.back()->can_hold(current_solution[i])) {
            neighbor_solution.push_back(current_solution[i]);
            neighbor_score += current_solution[i]->height;
        }
        else {
            temp_tabu_list.push_back(current_solution[i]);
        }
    }

    return {neighbor_solution, neighbor_score};
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
