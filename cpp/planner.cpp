#include "planner.h"


Planner::Planner(std::string& name) {
    const std::string fn(name);
    this->ParseJson(fn);
}

void Planner::Search(const std::string& startName, const std::string& goalName) {
    Node* start = this->g.NameToNode(startName);
    Node* goal = this->g.NameToNode(goalName);

    if (start != nullptr && goal != nullptr) {
        //this->g.Search(start, goal);        
    }
    else
    {
        if (start == nullptr) std::cout << "Could not find recipe node for: " << startName << std::endl;
        if (goal == nullptr) std::cout << "Could not find goal node with name: " << goalName << std::endl;
    }
}

void Planner::ParseJson(const std::string& filename) {

    FILE* file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
        std::cerr << "Could not open json file: " << filename << "\n";

    } 
    else
    {
        rapidjson::FileStream is(file);
        rapidjson::Document d;
        d.ParseStream<0>(is);

        if (d.IsObject()) {
            if (d.HasMember("Goal") && d["Goal"].IsObject()) {
                const rapidjson::Value& goal = d["Goal"];
                for (rapidjson::Value::ConstMemberIterator it = goal.MemberBegin(); 
                     it != goal.MemberEnd(); it++) {
                    goalQtys.insert({it->name.GetString(), it->value.GetInt()});

                }              
            }
            
            if (d.HasMember("Initial") && d["Initial"].IsObject()) {
                const rapidjson::Value& initial = d["Initial"];
                for (rapidjson::Value::ConstMemberIterator it = initial.MemberBegin(); 
                     it != initial.MemberEnd(); it++) {
                    initialItems.insert({it->name.GetString(), it->value.GetInt()});

                }              
            }

            if (d.HasMember("Items") && d["Items"].IsArray()) {
                const rapidjson::Value& itemsArray = d["Items"];
                int found = 0;
                for (rapidjson::SizeType i = 0; i < itemsArray.Size(); i++) {
                    
                    items.push_back(itemsArray[i].GetString());
                    //g.AddItem(itemsArray[i].GetString());
                    found++;
                }
                std::cout << "Parsed " << found << " items.\n";
            }
            std::unordered_map<std::string, Recipe*> allRecipes;
            if (d.HasMember("Recipes") && d["Recipes"].IsObject()) {
                const rapidjson::Value& recipes = d["Recipes"];

                for (rapidjson::Value::ConstMemberIterator it = recipes.MemberBegin(); 
                     it != recipes.MemberEnd(); it++) {
                    //std::cout << "NAME: " << it->name.GetString() << "\n";
                    std::string name(it->name.GetString());
                    Recipe* r = ParseRecipe(name, it->value);
                    allRecipes.insert({name, r});
                    g.AddNode(r);
                }
                // Build a dependency graph later.
                g.BuildDeps(allRecipes, items);

            }   
            else
            {
                std::cout << "Invalid recipes object found in file.\n";
            } 
        }
        else
        {
            std::cerr << "Invalid JSON Object found in " << filename << "\n";
        }


        if (file != nullptr) fclose(file);
    }
}

Recipe* Planner::ParseRecipe(const std::string name, const rapidjson::Value& recipe) {
    
    Recipe* r = new Recipe(name);
    //std::cout << "PARSING RECIPE\n";

    if (recipe.HasMember("Time")) {
        r->timeCost = recipe["Time"].GetInt();

    }
    if (recipe.HasMember("Produces") && recipe["Produces"].IsObject()) {
        for (rapidjson::Value::ConstMemberIterator it = recipe["Produces"].MemberBegin(); 
                     it != recipe["Produces"].MemberEnd(); it++) {
            r->produces.insert({it->name.GetString(), it->value.GetInt()});
        }   
    }

    if (recipe.HasMember("Requires") && recipe["Requires"].IsObject()) {

        for (rapidjson::Value::ConstMemberIterator it = recipe["Requires"].MemberBegin(); 
                     it != recipe["Requires"].MemberEnd(); it++) {
            r->requires.insert({it->name.GetString(), it->value.GetBool()});
        } 
    }

    if (recipe.HasMember("Consumes") && recipe["Consumes"].IsObject()) {
        for (rapidjson::Value::ConstMemberIterator it = recipe["Consumes"].MemberBegin(); 
                     it != recipe["Consumes"].MemberEnd(); it++) {
            r->consumes.insert({it->name.GetString(), it->value.GetInt()});
        } 
    }

    return r;
}

