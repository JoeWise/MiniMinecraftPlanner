#ifndef __GRAPH_H__
#define __GRAPH_H__
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <iostream>
#include <unordered_map>
#include <assert.h>
#include <algorithm>
#include "crafting.h"

struct Node;
struct Node {
    const std::string name;
    Node(std::string _name): name(_name) {}
    int priority = 0;
    std::map<std::string, int> requires;
    std::map<std::string, int> consumes;
    std::map<std::string, int> produces;
    int timeCost = 0;
    int actionCost = 1;

};


struct State {
    std::map<std::string, int> inventory;
    std::vector<Node*> validActions;
    int totalTimeCost = 0;
    int actionCost = 1;
    //std::map<std::string, int> goalQtys;

    bool IsGoal(std::map<std::string, int>& goalQtys) {
        bool found = true;
        for (const auto& g : goalQtys) {
            if (inventory.find(g.first) != inventory.end()) {
                int qty = inventory.at(g.first);
                if (qty < g.second) {
                    found = false;
                }
            }
            else
            {
                found = false;
            }
        }
        return found;
    }
   std::string Hash(){
        std::string s("h_");
        for (const auto& i : inventory) {
            s += i.first + std::to_string(i.second);
        }

        return s;
    }
    int GetCount(const std::string& name) {
        if (inventory.find(name) != inventory.end()) {
            return inventory.at(name);
        }
        else
        {
            return 0;
        }
    }
};

class CompareStates {
public:
    bool operator()(State* s1, State* s2) {
        return (s1->totalTimeCost > s2->totalTimeCost);
    }
};


struct GraphSearch {
    unsigned long nodesExpanded = 0;
    bool foundGoal = false;
    unsigned int timeCost = 0;
    unsigned int actionCost = 0;
    std::string goalName;
    std::map<std::string, int> inventory;

};

class Graph {
private: 
    std::map<std::string, Node*> nodes;
    std::map<std::string, int> maxProd;
    std::map<std::string, int> maxCons;
    std::map<std::string, int> maxItems;
    void SetMaxItems(std::vector<std::string>& items);
public:
    Graph();
    bool InsideBounds(State* state);
    State* GetNextState(State* s, Node* node);
    State* GetStartState(std::map<std::string, int>& initialItems);
    Node* NameToNode(const std::string& name);
    void Search(GraphSearch& gs, std::map<std::string, int>& goalQtys,
        std::map<std::string, int>& initial);
    void AddNode(Recipe* recipe);
    void AddNode(const std::string& name);
    void BuildDeps(std::unordered_map<std::string, Recipe*>& allRecipes,
        std::vector<std::string>& items);
    Node* GetStartState(State& s);
};

#endif