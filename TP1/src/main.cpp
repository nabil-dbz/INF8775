#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "skyline-problem.h"
 
int main(int argc, char *argv[])
{
    std::unique_ptr<SkylineProblem> ptr = std::make_unique<SkylineProblem>("N4_0");    
    std::vector<Point> points = ptr->naive_algorithm();
}
