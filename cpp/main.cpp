#include <stdio.h>
#include "planner.h"
#include <time.h>
int main(int argc, char** argv) {

    std::string filename("crafting.json");
    if (argc >= 2) filename = argv[1];

    std::cout << "Using file: " << filename << "\n";
    Planner planner(filename);
    GraphSearch gs;

    clock_t tStart = clock();
    planner.g.Search(gs, planner.goalQtys, planner.initialItems);
    double t_diff = (double)(clock() - tStart)/CLOCKS_PER_SEC;
    std::cout << "Search ended with " << gs.nodesExpanded <<
    " node expansions and " << t_diff << " seconds.\n";

    
    if (gs.foundGoal) {
        std::cout << " Goal state reached in " << gs.timeCost 
        << " time with " << gs.actionCost << " actions.\n";
        std::cout << "Goal Inventory:\n";
        for (const auto& inv : gs.inventory) {
            std::cout << "\t" << inv.first << ": " << inv.second << "\n";
        }          

    }
    else
    {
        std::cout << " The goal was not found.\n";
    }

}