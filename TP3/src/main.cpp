#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include "assigment-problem.h"
 

int main(int argc, char *argv[])
{ 
    if (argc < 2 || argc > 3) {
        std::cout << "Please provide the following arguments: " << std::endl;
        std::cout << "\t<file_path> <should_print_results>" << std::endl;
        return 1;
    }

    const std::string file_path = argv[1];

    bool should_print_results = false;
    std::istringstream(argv[2]) >> should_print_results;

    std::unique_ptr<AssignmentProblem> highest_tower_problem = std::make_unique<AssignmentProblem>(file_path);

    const std::clock_t start_time = std::clock();

    const std::clock_t end_time = std::clock();

    if (should_print_results)
        // AssignmentProblem::print_results(results.cubes);

    return 0;
}
