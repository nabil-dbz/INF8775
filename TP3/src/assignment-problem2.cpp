#include "assignment-problem2.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <deque>
#include <vector>
#include <random>

AssignmentProblem2::AssignmentProblem2(const std::string& path) {
    parse_file(path);
    std::srand(time(0));
}

void AssignmentProblem2::parse_file(const std::string& path) {
    // Create a string in which we store each line we read.
    std::string line;

    // Read from the text file.
    std::ifstream file_reader(path);

    // Parse number of sites, atom types, number of edges
    std::getline(file_reader, line);
    std::istringstream stream(line);
    size_t n_atoms, n_atom_types, n_edges;
    stream >> n_atoms >> n_atom_types >> n_edges;

    file_reader.ignore();

    // Parse assignments constraints
    for(size_t i=0; i<n_atom_types; i++){
        std::string occurrences;
        file_reader >> occurrences;
        assignments_constraint.push_back(std::stoi(occurrences));
    }

    file_reader.ignore();

    // Init H matrix
    for(size_t i = 0; i < n_atom_types; i++){
        H.push_back(std::vector<int>(n_atom_types));
    }

    // Parse values of matrix
    for(size_t i=0; i<n_atom_types; i++){
        for(size_t j=0; j<n_atom_types; j++){
            std::string occurrences;
            file_reader >> occurrences;
            H[i][j] = std::stoi(occurrences);
        }  
    }

    file_reader.ignore();

    // Init graph
    for(size_t i=0; i<n_atoms; i++){
        graph.push_back(std::vector<size_t>());
    }

    // Parse graph
    for(size_t i=0; i<n_edges; i++){
        std::string s;
        std::string e;
        file_reader >> s >> e;
        size_t start = std::stoi(s);
        size_t end = std::stoi(e);

        graph[start].push_back(end);
        graph[end].push_back(start);
    }

    // Close the file.
    file_reader.close();
}

Solution2 AssignmentProblem2::simulated_annealing(bool should_print_results){

    Solution2 old_solution = get_initial_solution();

    Solution2 best_solution = old_solution;

    double counter = 0;

    while(true){
        Solution2 new_solution = get_best_neighbor_solution2(old_solution);

        if(new_solution.total_energy < best_solution.total_energy){
            best_solution = new_solution;
            if(should_print_results) print_results(best_solution);
        }

        if( new_solution.total_energy < old_solution.total_energy || 
            std::exp( (old_solution.total_energy - new_solution.total_energy) / temperature_function(counter) ) > ((double)(std::rand())/(double)RAND_MAX) ){
            old_solution  = new_solution;
        }
        counter++;
    }

    return best_solution;
}

double AssignmentProblem2::temperature_function(double counter){
    return (double)1 / (counter*counter) ;
}


Solution2 AssignmentProblem2::tabu_algorithm(bool should_print_results) {
    //Get initial solution
    Solution2 random_solution = get_random_solution();
    Solution2 greedy_solution = get_greedy_solution();

    // Solution2 current_solution = greedy_solution.total_energy < random_solution.total_energy ? greedy_solution : random_solution;
    Solution2 current_solution = greedy_solution;


    print_results(current_solution);
    // return current_solution;

    Solution2 best_solution = current_solution;
    
    // tabu list
    std::unordered_map<size_t, int8_t> tabu; 
    
    while (true) {
        current_solution = get_best_neighbor_solution(current_solution, tabu);  
        if (current_solution.total_energy < best_solution.total_energy) {
            best_solution = current_solution;
            if(should_print_results) print_results(best_solution); 
        }
    }
}

Solution2 AssignmentProblem2::get_best_neighbor_solution(Solution2& current_solution, std::unordered_map<size_t, int8_t>& tabu) {
    auto assignments = current_solution.node_assignments;
    int best_diff = INT32_MAX;
    std::pair<size_t, size_t> best_pair = {};
    std::pair<int, int> best_diffs = {};

    const auto n_nodes = std::min((size_t)10, assignments.size());
    
    auto nodes = std::vector<size_t>();

    for (size_t i = 0; i < n_nodes; i++)
    {
        nodes.push_back(std::rand() % assignments.size());
    }

    // parallelized loop, ref: https://github.com/bshoshany/thread-pool
    auto loop = [this, &tabu, &best_diff, &best_pair, &assignments, &nodes, &current_solution, &best_diffs](const size_t &start, const size_t &end)
    {
        for (size_t i = start; i < end; i++) {
            const auto node = nodes[i];
            for(size_t i = 0; i < assignments.size(); i++) {
                if( i == node) continue;
                if(assignments[i] == assignments[node]) continue;
                if (tabu.find(i+node) != tabu.end()) continue;

                int energy_diff_node = get_node_new_energy_diff(current_solution, node, i);

                int energy_diff_swap_node = get_node_new_energy_diff(current_solution, i, node);

                if(energy_diff_node +  energy_diff_swap_node < best_diff){
                    best_pair = {node, i};
                    best_diffs = {energy_diff_node, energy_diff_swap_node};
                    best_diff = energy_diff_node +  energy_diff_swap_node;
                }
            }
        }
    };

    pool.parallelize_loop(0, nodes.size(), loop);

    // add swapping elements to tabu list
    tabu[best_pair.first+best_pair.second] = 10 + std::rand() % 4;

    // suppress elements
    for (auto el = tabu.begin(); el != tabu.end(); /**/) {
        if (el->second - 1 == 0) {
            tabu.erase(el++);
        } else {
            el->second = el->second - 1;
            ++el;
        }
    }
    
    std::swap(assignments[best_pair.first], assignments[best_pair.second]);

    return {.node_assignments = assignments, .total_energy = current_solution.total_energy + 2*best_diff};
}



Solution2 AssignmentProblem2::get_best_neighbor_solution2(Solution2& current_solution) {
    auto assignments = current_solution.node_assignments;
    int best_diff = INT32_MAX;
    std::pair<size_t, size_t> best_pair = {};
    std::pair<int, int> best_diffs = {};

    const auto n_nodes = std::min((size_t)10, assignments.size());
    
    auto nodes = std::vector<size_t>();

    for (size_t i = 0; i < n_nodes; i++)
    {
        nodes.push_back(std::rand() % assignments.size());
    }

    // parallelized loop, ref: https://github.com/bshoshany/thread-pool
    auto loop = [this, &best_diff, &best_pair, &assignments, &nodes, &current_solution, &best_diffs](const size_t &start, const size_t &end)
    {
        for (size_t i = start; i < end; i++) {
            const auto node = nodes[i];
            for(size_t i = 0; i < assignments.size(); i++) {
                if( i == node) continue;
                if(assignments[i] == assignments[node]) continue;

                int energy_diff_node = get_node_new_energy_diff(current_solution, node, i);

                int energy_diff_swap_node = get_node_new_energy_diff(current_solution, i, node);

                if(energy_diff_node +  energy_diff_swap_node < best_diff){
                    best_pair = {node, i};
                    best_diffs = {energy_diff_node, energy_diff_swap_node};
                    best_diff = energy_diff_node +  energy_diff_swap_node;
                }
            }
        }
    };

    pool.parallelize_loop(0, nodes.size(), loop);
    
    std::swap(assignments[best_pair.first], assignments[best_pair.second]);

    return {.node_assignments = assignments, .total_energy = current_solution.total_energy + 2*best_diff};
}

int AssignmentProblem2::get_node_new_energy_diff(const Solution2& solution, size_t node, size_t swap_node){
    const auto assignments = solution.node_assignments;
    int diff = 0;
    for(const auto& neighbor: graph[node]){
        if(neighbor == swap_node) continue;
        diff += H[assignments[neighbor]][assignments[swap_node]] - H[assignments[neighbor]][assignments[node]];
    }
    return diff;
}

int AssignmentProblem2::get_total_energy(const std::vector<uint8_t>& assignments){
    int total_energy = 0;

    for(size_t node = 0; node < graph.size(); node++){
        for(const auto& neighbor: graph[node]){
            total_energy += H[assignments[node]][assignments[neighbor]];
        }
    }
    
    return total_energy;
}

void AssignmentProblem2::print_results(const Solution2& solution) {
    for (const auto& assignment: solution.node_assignments) {
        std::cout << (int)assignment << " ";
    }
    std::cout << "[ score : " << solution.total_energy / 2 << " ]" << std::endl;
}

Solution2 AssignmentProblem2::get_initial_solution(){
    Solution2 random_solution = get_random_solution();
    Solution2 greedy_solution = get_greedy_solution();

    return greedy_solution.total_energy < random_solution.total_energy ? greedy_solution : random_solution;
}

Solution2 AssignmentProblem2::get_greedy_solution(){
    // get start node randomly
    size_t s = std::rand() % graph.size();

    // Init solution
    auto constraints = std::vector<uint32_t>();
    std::copy(assignments_constraint.begin(), assignments_constraint.end(), std::back_inserter(constraints));

    std::vector<int8_t> assignments(graph.size());
    int total_energy = 0;

    for(size_t i = 0; i < graph.size(); i++)
        assignments[i] = -1;
 
    // Create a queue for BFS
    std::deque<size_t> queue;

    // Mark the current node as visited and enqueue it
    queue.push_back(s);

    // affect random atom type to the start node
    const uint8_t atom_type = std::rand() % constraints.size();
    assignments[s] = atom_type;
    constraints[atom_type]--;

    while(!queue.empty())
    {
        // Dequeue a vertex from queue
        s = queue.front();
        queue.pop_front();
 
        // Assign types to nodes
        for(const auto& neighbor: graph[s]){
            if (assignments[neighbor] == -1){
                // real algo.
                int min_type = -1;
                int min_interaction = INT32_MAX;
                for(size_t atom_type = 0; atom_type < constraints.size(); atom_type++){
                    if(constraints[atom_type] == 0) continue;
                    if(H[atom_type][assignments[s]] < min_interaction){
                        min_interaction = H[atom_type][assignments[s]];
                        min_type = atom_type;
                    }
                }
                assignments[neighbor] = min_type;
                constraints[min_type]--;
                queue.push_back(neighbor);
            }
            total_energy += H[assignments[neighbor]][assignments[s]];
        }
    }

    const auto res_assignments = std::vector<uint8_t>(assignments.begin(), assignments.end());
    return {.node_assignments=res_assignments, .total_energy=total_energy};
}

Solution2 AssignmentProblem2::get_random_solution(){
    Solution2 random_solution;

    std::random_device rd;
    std::mt19937 g(rd());
 
    for(size_t i = 0; i < assignments_constraint.size(); i++){
        const uint32_t constraint = assignments_constraint[i]; // combien de type i
        for(size_t j = 0; j < constraint; j++){
            random_solution.node_assignments.push_back(i);
        }
    }

    std::shuffle(random_solution.node_assignments.begin(), random_solution.node_assignments.end(), g);

    random_solution.total_energy = get_total_energy(random_solution.node_assignments);

    return random_solution;
}