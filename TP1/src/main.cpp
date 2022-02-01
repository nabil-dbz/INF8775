#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "skyline-problem.h"
 
int main(int argc, char *argv[])
{
    std::unique_ptr<SkylineProblem> ptr = std::make_unique<SkylineProblem>("N6_0");    
    std::vector<Point> points = ptr->naive_algorithm();
    for (const Point point: points) {
        std::cout << point; 
    }
}
