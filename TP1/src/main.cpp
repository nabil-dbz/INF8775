#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include "skyline-problem.h"
 
enum AlgorithmType { NAIVE=0, RECURSIVE, RECURSIVE_CUTOFF };

int main(int argc, char *argv[])
{ 
    if (argc < 5 || argc > 6) {
        std::cout << "Please provide the following arguments: " << std::endl;
        std::cout << "\t<file_path> <is_execution_time_required> <should_print_results> <algorithm_type> <algorithm_cutoff>" << std::endl;
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

    std::unique_ptr<SkylineProblem> skyline_problem = std::make_unique<SkylineProblem>(file_path);
    std::vector<Point> results;

    const std::clock_t start_time = std::clock();
    switch (algo_type) {
        case NAIVE:
            results = skyline_problem->naive_algorithm();
            break;
        case RECURSIVE:
            results = skyline_problem->recursive_algorithm();
            break;
        case RECURSIVE_CUTOFF:
            results = skyline_problem->recursive_cutoff_algorithm(/*recursive_algorithm_cutoff=*/8);
            break;
        default:
            std::cout << "Please make sure to provide the right type of algorithm!" << std::endl;
            return 1;
    }
    const std::clock_t end_time = std::clock();

    if (should_print_results)
        skyline_problem->print_results(results);

    if (is_execution_time_required)
        std::cout << (float)(end_time - start_time) / (CLOCKS_PER_SEC / 1000) << std::endl;

    return 0;
}
