#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include "assignment-problem2.h"
// #include "assignment-problem.h"

 

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

    std::unique_ptr<AssignmentProblem2> assignment_problem = std::make_unique<AssignmentProblem2>(file_path);

    const Solution2 solution = assignment_problem->tabu_algorithm(should_print_results);

    return 0;
}
