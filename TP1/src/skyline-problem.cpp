#include "skyline-problem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

SkylineProblem::SkylineProblem(const std::string& path) {
    buildings_length = -1;
    parse_file(path);
    for (int index = 0; index < buildings.size(); index++) {
        std::cout << "building " << index + 1 << std::endl;
        std::cout << buildings[index]->get_critical_points().first.x << "," << buildings[index]->get_critical_points().first.y << "  ,   ";
        std::cout << buildings[index]->get_critical_points().second.x << "," << buildings[index]->get_critical_points().second.y << std::endl;
    }
}

void SkylineProblem::parse_file(const std::string& path) {
    // Create a string in which we store each line we read.
    std::string line;

    // Read from the text file.
    std::ifstream file_reader(path);

    // Use a while loop together with the getline() function to read the file line by line.
    while (std::getline(file_reader, line)) {
        std::istringstream stream(line);
        if (buildings_length == -1) {
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
    // Generate critical points.
    std::vector<Point> critical_points;
    for (const std::shared_ptr<Building> building: buildings) {
        const auto points = building->get_critical_points();
        critical_points.push_back(points.first);
        critical_points.push_back(points.second);
    }

    for (const Point point: critical_points) {
        std::cout << point;
    }

    // Sort critical points.
    const auto compare = [](const Point& a, const Point& b) { return a.x < b.x; };
    std::sort(critical_points.begin(), critical_points.end(), compare);

    std::cout << std::endl;
    for (const Point point: critical_points) {
        std::cout << point;
    }

    return critical_points;
}
