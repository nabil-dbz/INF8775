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
            std::cout << "Executing the naive algorithm" << std::endl;
            results = skyline_problem->naive_algorithm();
            break;
        case RECURSIVE:
            std::cout << "Executing the recursive algorithm" << std::endl;
            results = skyline_problem->recursive_algorithm();
            break;
        case RECURSIVE_CUTOFF:
            if (argc != 6) {
                std::cout << "Please make sure to provide the cutoff of the algorithm!" << std::endl;
                return 1;
            }
            {
                int recursive_algorithm_cutoff = 1;
                std::istringstream(argv[5]) >> recursive_algorithm_cutoff;
                std::cout << "Executing the recursive algorithm with cutoff " << recursive_algorithm_cutoff << std::endl;
                results = skyline_problem->recursive_cutoff_algorithm(recursive_algorithm_cutoff);
            }
            break;
        default:
            std::cout << "Please make sure to provide the right type of algorithm!" << std::endl;
            return 1;
    }
    const std::clock_t end_time = std::clock();

    if (should_print_results)
        skyline_problem->print_results(results);

    if (is_execution_time_required)
        std::cout << "Execution time is: " << (float)(end_time - start_time) / CLOCKS_PER_SEC << "s" << std::endl;

    return 0;
}
