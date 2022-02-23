#include "skyline-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

SkylineProblem::SkylineProblem(const std::string& path):
    buildings_length(0), recursive_algorithm_cutoff(1)
{
    parse_file(path);
}

void SkylineProblem::parse_file(const std::string& path) {
    // Create a string in which we store each line we read.
    std::string line;

    // Read from the text file.
    std::ifstream file_reader(path);

    // Use a while loop together with the getline() function to read the file line by line.
    while (std::getline(file_reader, line)) {
        std::istringstream stream(line);
        if (buildings_length == 0) {
            stream >> buildings_length;
            continue;
        }
        uint32_t left, right, height;
        stream >> left >> right >> height;
        buildings.push_back(std::make_shared<Building>(left, right, height));
    }

    // Close the file.
    file_reader.close();
}

std::vector<Point> SkylineProblem::naive_algorithm() {
    return brute_force_algorithm(buildings);
}

std::vector<Point> SkylineProblem::recursive_algorithm() {
    recursive_algorithm_cutoff = 1;
    return divide_and_conquer_cutoff_algorithm(buildings);
}

std::vector<Point> SkylineProblem::recursive_cutoff_algorithm(const uint32_t& cutoff){
    recursive_algorithm_cutoff = cutoff;
    return divide_and_conquer_cutoff_algorithm(buildings);
}

std::vector<Point> SkylineProblem::brute_force_algorithm(const std::vector<std::shared_ptr<Building>> buildings){
    // Generate critical points.
    std::vector<Point> critical_points;
    for (const std::shared_ptr<Building> building: buildings) {
        const auto points = building->get_critical_points();
        critical_points.push_back(points.first);
        critical_points.push_back(points.second);
    }

    // Sort critical points.
    const auto compare = [](const Point& a, const Point& b) { return a.x < b.x; };
    std::sort(critical_points.begin(), critical_points.end(), compare);

    // Naive algorithm
    std::vector<Point> result;
    for (Point& point: critical_points) {
        for (const std::shared_ptr<Building> building: buildings) {
            if (building->includes(point)) {
                point.y = building->height;
            }
        }
        if (result.empty()){
            result.push_back(point);
            continue;
        }
        
        if (result.back().y != point.y && result.back().x != point.x) {
            result.push_back(point);
        }
    }

    return result;
}

std::vector<Point> SkylineProblem::divide_and_conquer_cutoff_algorithm(const std::vector<std::shared_ptr<Building>> buildings) {
    if (buildings.size() <= recursive_algorithm_cutoff)
        return brute_force_algorithm(buildings);

    std::size_t const half_size = buildings.size() / 2;
    std::vector<std::shared_ptr<Building>> first_half(buildings.begin(), buildings.begin() + half_size);
    std::vector<std::shared_ptr<Building>> second_half(buildings.begin() + half_size, buildings.end());

    std::vector<Point> first_half_result = divide_and_conquer_cutoff_algorithm(first_half);
    std::vector<Point> second_half_result = divide_and_conquer_cutoff_algorithm(second_half);

    return merge_buildings(first_half_result, second_half_result);
}

std::vector<Point> SkylineProblem::merge_buildings(
    std::vector<Point> first_half, 
    std::vector<Point> second_half) {

    std::vector<Point> result;
    std::size_t first_height = 0;
    std::size_t second_height = 0;

    std::size_t first_index = 0;
    std::size_t second_index = 0;
    while (first_index < first_half.size() && second_index < second_half.size()) {
        Point point;
        if (first_half[first_index] == second_half[second_index]) {
            result.push_back(first_half[first_index]);
            first_height = first_half[first_index++].y;
            second_height = first_half[second_index++].y;
            continue;
        }

        if (first_half[first_index] < second_half[second_index]) {
            point = first_half[first_index++];
            first_height = point.y;
        } else {
            point = second_half[second_index++];
            second_height = point.y;
        }
        point.y = std::max(first_height, second_height);
        if (result.empty() || result.back().y != point.y) {
            result.push_back(point);
        }
    }

    while (first_index < first_half.size()) {
        result.push_back(first_half[first_index++]);
    }

    while (second_index < second_half.size()) {
        result.push_back(second_half[second_index++]);
    }

    return result;
}

void SkylineProblem::print_results(const std::vector<Point>& results) {
    for (const Point point: results) {
        std::cout << point << std::endl;
    }
}
