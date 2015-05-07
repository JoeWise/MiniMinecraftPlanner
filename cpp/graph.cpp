#include "graph.h"
#include "crafting.h"

Graph::Graph() {

}

Node* Graph::NameToNode(const std::string& name) {
    std::cout << "LOOKING FOR: " << name << "\n";
    if (nodes.find(name) != nodes.end()) {
        return nodes.at(name);
    }
    else
    {
        return nullptr;
    }
}

void Graph::Search(GraphSearch& gs, std::map<std::string, int>& goalQtys,
    std::map<std::string, int>& initialItems) {
    //assert(gs.goalNode != nullptr);
    std::cout << "STARTING GRAPH SEARCH\n";

    std::priority_queue<State*, std::vector<State*>, CompareStates> frontier;

    State* s = GetStartState(initialItems);
    frontier.push(s);
    std::unordered_map<State*, State*> came_from;
    std::unordered_map<std::string, int> cost_so_far;
    //std::string hash = 
    while (!frontier.empty() > 0) {
        //std::cout << "LOOP\n";
        State* curr = frontier.top();
        frontier.pop();
        gs.nodesExpanded++;
        // If we found our goal, stop here.
        if (curr->IsGoal(goalQtys)) {
            std::cout << "FOUND GOAL\n";
            gs.foundGoal = true;
            gs.timeCost = curr->totalTimeCost;
            gs.actionCost = curr->actionCost;
            gs.inventory = curr->inventory;
            break;
        }

        for (const auto& a : curr->validActions) {
            //std::cout << "EDGE: " << a->name << "\n";
            State* nextState = GetNextState(curr, a);

            if (InsideBounds(nextState)) {
                std::string nsHash = nextState->Hash();
                int newCost = cost_so_far[curr->Hash()] + a->timeCost;
                //td::cout << "CURR: " << curr->Hash() << "\n";
                //std::cout << "NEW_COST: " << nsHash << "\n";        
                if (!cost_so_far.count(nsHash) || newCost < cost_so_far[nextState->Hash()]) {
                    int totalTimeCost = newCost + 0; // new + heuristic
                    nextState->totalTimeCost = totalTimeCost;
                    came_from[nextState] = curr;
                    cost_so_far[nextState->Hash()] = newCost;
                    frontier.push(nextState);

                }            
            }
        }

    }
}


bool Graph::InsideBounds(State* state) {
    assert(state != nullptr);
    bool isInside = true;
    for (const auto& i : state->inventory) {
        if (maxItems.find(i.first) != maxItems.end()) {
            if (i.second > maxItems.at(i.first)) isInside = false;
        }   
    }

    return isInside;
}

void Graph::SetMaxItems(std::vector<std::string>& items) {

    for (const auto& i : items) {
        maxItems.insert({i, 0});
        maxCons.insert({i, 1});
        maxProd.insert({i, 1});
    }

    for (const auto& i : nodes) {
        for (const auto& c : i.second->consumes) {
            maxCons[c.first] = std::max(c.second, maxCons[c.first]);
        }

        for (const auto& c : i.second->produces) {
            maxProd[c.first] = std::max(c.second, maxProd[c.first]);
            
        }
    }

    for (const auto& i : maxCons) {
        maxItems.at(i.first) += i.second;

    }

    for (const auto& i : maxProd) {
        maxItems.at(i.first) += i.second;

    }

    for (const auto& i : maxItems) {
        maxItems.at(i.first) -= 1;
        //std::cout << "MAX[" << i.first << "]: " << maxItems.at(i.first) << "\n";
    }

}


void Graph::AddNode(const std::string& name) {
    Node* node = new Node(name);
    nodes.insert({name, node});
}

void Graph::AddNode(Recipe* recipe) {
    assert(recipe != nullptr);
    Node* node = new Node(recipe->name);
    node->timeCost = recipe->timeCost;
    nodes.insert({node->name, node});

}

void Graph::BuildDeps(std::unordered_map<std::string, Recipe*>& allRecipes,
    std::vector<std::string>& items) {
    // Build the edges for all nodes in our graph.
    // This needs to happen after all nodes in our graph already
    // exist.
    for (auto& node : nodes) {
        if (allRecipes.find(node.first) != allRecipes.end()) {
            Recipe* r = allRecipes.at(node.first);
            for (const auto& item : r->requires) {
                node.second->requires.insert({item.first, 1});
            }

            for (const auto& item : r->consumes) {
                node.second->consumes.insert({item.first, item.second});
            
            }

            for (const auto& item : r->produces) {
                //std::cout << "PRODUCES..." << item.first << "\n";
                //Node* e = NameToNode(item.first);
                node.second->produces.insert({item.first, item.second});

            }
        }  
    }

    SetMaxItems(items);
}

State* Graph::GetStartState(std::map<std::string, int>& initialItems) {
    State* s = new State();
    Node* node = NULL;
    State* ns = GetNextState(s, node);
    ns->actionCost = 0;
    for (const auto& i : initialItems) {
        ns->inventory[i.first] = i.second;
    }
    delete s;
    return ns;
}


State* Graph::GetNextState(State* prevState, Node* node) {

    State* s = new State();
    s->inventory = prevState->inventory;

    // TODO: Remove this. Bad.
    //s->goalQtys = prevState->goalQtys;
    if (node != nullptr)  {
        //s->totalTimeCost = prevState->totalTimeCost + node->timeCost;
        s->actionCost = prevState->actionCost + node->actionCost;
    }

    if (node != nullptr) {
        for (const auto& item : node->produces) {
            //std::cout << "ADD +" << item.second << " " << item.first << "\n";
            if (s->inventory.find(item.first) != s->inventory.end()) {
                s->inventory.at(item.first) += item.second;

            }
            else
            {
                s->inventory.insert({item.first, item.second});
            }
        }

        for (const auto& item : node->consumes) {
            //s->inventory[item.first] -= item.second;
            if (s->inventory.find(item.first) != s->inventory.end()) {
                s->inventory.at(item.first) -= item.second;
            }
        }
    }

    for (const auto& v : nodes) {
        bool canReach = true;

        for (const auto& req : v.second->requires) {
            if (s->GetCount(req.first) < 1) 
                //std::cout << "REQ: " << req.first << "\n";
                canReach = false;
        }

        for (const auto& item : v.second->consumes) {
            if (s->GetCount(item.first) < item.second) canReach = false;
        }

        if (canReach) {
            s->validActions.push_back(v.second);
        }
    }

    return s;
}