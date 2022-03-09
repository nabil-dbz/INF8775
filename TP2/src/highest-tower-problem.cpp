#include "highest-tower-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>

const int NO_LAST = -1;

HighestTowerProblem::HighestTowerProblem(const std::string& path) {
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

std::vector<Cube*> HighestTowerProblem::greedy_algorithm() {
    std::vector<Cube*> results;

    // Sort cubes accordingly.
    // TODO: find a way to generalize this by passing a comparing function as an argument.
    const auto compare = [](const std::shared_ptr<Cube>& cube_a, const std::shared_ptr<Cube>& cube_b) { 
        return cube_a->get_base_area() > cube_b->get_base_area(); 
    };
    std::sort(cubes.begin(), cubes.end(), compare);

    for(const std::shared_ptr<Cube>& cube: cubes){
        if(results.empty()){
            results.push_back(cube.get());
            continue;
        }

        if (results.back()->can_hold(cube.get())) {
            results.push_back(cube.get());
        }
    }

    return results;
}

std::vector<Cube*> HighestTowerProblem::dynamic_programming_algorithm() {    
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

    return get_path_solution(top_cube_index, max_predecessor_index, predecessors);
}

std::vector<Cube*> HighestTowerProblem::taboo_algorithm() {
    std::vector<Cube*> results;
    // TODO: Implement this!
    for (auto cube: cubes){
        results.push_back(cube.get());
    }
    return results;
}

void HighestTowerProblem::print_results(const std::vector<Cube*>& results) {
    for (const Cube* cube: results) {
        std::cout << *cube << std::endl;
    }
}

std::vector<Cube*> HighestTowerProblem::get_path_solution(int first_cube_index, int first_predecessor_index, const std::vector<int>& predecessors) {
    std::deque<Cube*> elements;
    elements.push_front(cubes[first_cube_index].get());

    while (first_predecessor_index != NO_LAST) {
        elements.push_front(cubes[first_predecessor_index].get());
        first_predecessor_index = predecessors[first_predecessor_index];
    }

    return std::vector<Cube*>({elements.begin(), elements.end()});
}

// std::vector<Point> HighestTowerProblem::recursive_algorithm() {
//     recursive_algorithm_cutoff = 1;
//     return divide_and_conquer_cutoff_algorithm(buildings);
// }

// std::vector<Point> HighestTowerProblem::recursive_cutoff_algorithm(const uint32_t& cutoff){
//     recursive_algorithm_cutoff = cutoff;
//     return divide_and_conquer_cutoff_algorithm(buildings);
// }

// std::vector<Point> HighestTowerProblem::brute_force_algorithm(const std::vector<std::shared_ptr<Building>> buildings){
//     // Generate critical points.
//     std::vector<Point> critical_points;
//     for (const std::shared_ptr<Building> building: buildings) {
//         const auto points = building->get_critical_points();
//         critical_points.push_back(points.first);
//         critical_points.push_back(points.second);
//     }

//     // Sort critical points.
//     const auto compare = [](const Point& a, const Point& b) { return a.x < b.x; };
//     std::sort(critical_points.begin(), critical_points.end(), compare);

//     // Naive algorithm
//     std::vector<Point> result;
//     for (Point& point: critical_points) {
//         for (const std::shared_ptr<Building> building: buildings) {
//             if (building->includes(point)) {
//                 point.y = building->height;
//             }
//         }
//         if (result.empty()){
//             result.push_back(point);
//             continue;
//         }
        
//         if (result.back().y != point.y && result.back().x != point.x) {
//             result.push_back(point);
//         }
//     }

//     return result;
// }

// std::vector<Point> HighestTowerProblem::divide_and_conquer_cutoff_algorithm(const std::vector<std::shared_ptr<Building>> buildings) {
//     if (buildings.size() <= recursive_algorithm_cutoff)
//         return brute_force_algorithm(buildings);

//     std::size_t const half_size = buildings.size() / 2;
//     std::vector<std::shared_ptr<Building>> first_half(buildings.begin(), buildings.begin() + half_size);
//     std::vector<std::shared_ptr<Building>> second_half(buildings.begin() + half_size, buildings.end());

//     std::vector<Point> first_half_result = divide_and_conquer_cutoff_algorithm(first_half);
//     std::vector<Point> second_half_result = divide_and_conquer_cutoff_algorithm(second_half);

//     return merge_buildings(first_half_result, second_half_result);
// }

// std::vector<Point> HighestTowerProblem::merge_buildings(
//     std::vector<Point> first_half, 
//     std::vector<Point> second_half) {

//     std::vector<Point> result;
//     std::size_t first_height = 0;
//     std::size_t second_height = 0;

//     std::size_t first_index = 0;
//     std::size_t second_index = 0;
//     while (first_index < first_half.size() && second_index < second_half.size()) {
//         Point point;
//         if (first_half[first_index] == second_half[second_index]) {
//             result.push_back(first_half[first_index]);
//             first_height = first_half[first_index++].y;
//             second_height = first_half[second_index++].y;
//             continue;
//         }

//         if (first_half[first_index] < second_half[second_index]) {
//             point = first_half[first_index++];
//             first_height = point.y;
//         } else {
//             point = second_half[second_index++];
//             second_height = point.y;
//         }
//         point.y = std::max(first_height, second_height);
//         if (result.empty() || result.back().y != point.y) {
//             result.push_back(point);
//         }
//     }

//     while (first_index < first_half.size()) {
//         result.push_back(first_half[first_index++]);
//     }

//     while (second_index < second_half.size()) {
//         result.push_back(second_half[second_index++]);
//     }

//     return result;
// }
