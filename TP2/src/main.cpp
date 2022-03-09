#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include "highest-tower-problem.h"
 
enum AlgorithmType { GREEDY=0, DYNAMIC, TABOO };

int main(int argc, char *argv[])
{ 
    if (argc < 5 || argc > 6) {
        std::cout << "Please provide the following arguments: " << std::endl;
        std::cout << "\t<file_path> <is_execution_time_required> <should_print_results> <algorithm_type>" << std::endl;
        return 1;
    }

    const std::string file_path = argv[1];

    bool is_execution_time_required = false;
    std::istringstream(argv[2]) >> is_execution_time_required;

    bool should_print_results = false;
    std::istringstream(argv[3]) >> should_print_results;

    int type = 0;
    std::istringstream(argv[4]) >> type;
    AlgorithmType algo_type = static_cast<AlgorithmType>(type);

    std::unique_ptr<HighestTowerProblem> highest_tower_problem = std::make_unique<HighestTowerProblem>(file_path);
    std::vector<Cube*> results;

    const std::clock_t start_time = std::clock();
    switch (algo_type) {
        case GREEDY:
            results = highest_tower_problem->greedy_algorithm();
            break;
        case DYNAMIC:
            results = highest_tower_problem->dynamic_programming_algorithm();
            break;
        case TABOO:
            results = highest_tower_problem->taboo_algorithm();
            break;
        default:
            std::cout << "Please make sure to provide the right type of algorithm!" << std::endl;
            return 1;
    }
    const std::clock_t end_time = std::clock();

    if (should_print_results)
        HighestTowerProblem::print_results(results);

    if (is_execution_time_required)
        std::cout << (float)(end_time - start_time) / (CLOCKS_PER_SEC / 1000) << std::endl;

    return 0;
}
