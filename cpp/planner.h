#ifndef __PLANNER_H__
#define __PLANNER_H__
#include <queue>
#include <iostream>
#include "graph.h"
#include "rapidjson/document.h"
#include "rapidjson/filestream.h"
#include "crafting.h"


enum class ParseType {
    PT_NULL,
    PT_FALSE,
    PT_TRUE,
    PT_OBJECT,
    PT_ARRAY,
    PT_STRING,
    PT_NUMBER
};


class Planner {
private:
    std::map<std::string, int> inventory;
    std::vector<std::string> items;


public:
    Graph g;
    std::map<std::string, int> goalQtys; 
    std::map<std::string, int> initialItems;   
    Planner(std::string& filename);
    void Search(const std::string& name, const std::string& goalName);
    void ParseJson(const std::string& filename);
    Recipe* ParseRecipe(const std::string name, const rapidjson::Value& r);
};

#endif
