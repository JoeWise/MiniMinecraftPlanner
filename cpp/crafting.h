#ifndef __CRAFTING_H__
#define __CRAFTING_H__
#include <string>
#include <map>
#include <unordered_map>

struct Recipe {
    const std::string name;
    const std::string item_result;
    std::map<std::string, int> produces;
    std::map<std::string, int> consumes;
    std::map<std::string, bool> requires;
    int timeCost = 0;
    int actionCost = 0;
    Recipe(const std::string _name): name(_name) {};
};


Recipe* GetRecipe(const std::string& name);


#endif
