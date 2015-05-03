import json
with open('Crafting.json') as f:
    Crafting = json.load(f)
from collections import namedtuple
from collections import defaultdict

from heapq import heappush, heappop

# List of items that can be in your inventory:
#print "Item: ", Crafting['Items']
# example: ['bench', 'cart', ..., 'wood', 'wooden_axe', 'wooden_pickaxe']

# List of items in your initial inventory with amounts:
#print "Initial: ", Crafting['Initial']
# {'coal': 4, 'plank': 1}

# List of items needed to be in your inventory at the end of the plan:
# (okay to have more than this; some might be satisfied by initial inventory)
#print "Goal: ", Crafting['Goal']
#print Crafting['Goal']
# {'stone_pickaxe': 2}

# Dict of crafting recipes (each is a dict):
#print "Recipes: ", Crafting["Recipes"]["craft stone_pickaxe at bench"]
#print Crafting['Recipes']['craft stone_pickaxe at bench']
# example:
# {	'Produces': {'stone_pickaxe': 1},
#	'Requires': {'bench': True},
#	'Consumes': {'cobble': 3, 'stick': 2},
#	'Time': 1
# }

class State:

    def __init__(self, inventory, state=None):
        self.inventory = dict((el,0) for el in inventory)
        if (state is not None): self.inventory = state.inventory

    def __hash__(self):
        h = frozenset(self.inventory.items()).__hash__()
        return h

    def __eq__(self,other):
        pass

    def has_items(self, goals):
        has_item = True
        
        for item in goals:
            if (self.inventory[item] < goals[item]):
                has_item = False
                break
            else:
                print "HAS A GOAL: ", item

        return has_item

    
def make_goal_checker(goals):
    # this code runs once
    def is_goal(state, goals):
        # this code runs millions of times
        return state.has_items(goals)
        #return True # or False

    return is_goal


def make_checker(rule):
    # this code runs once
    # do something with rule['Consumes'] and rule['Requires']
    def check(state):
        satisfiesConsumes = True
        satisifiesRequires = True
        if 'Consumes' in rule.keys():
            #satisfiesConsumes = state.has_items(rule["Consumes"])
            pass

        if 'Produces' in rule.keys():
            
            pass

        if 'Requires' in rule.keys():
            pass
        #print rule
        # this code runs millions of times

        #return (satisfiesConsumes and satisifiesRequires) # or False
        return True
    return check

def make_effector(rule):
    # this code runs once
    # do something with rule['Produces'] and rule['Consumes']
    def effect(state):
        # this code runs millions of times
        #return next_state
        newState = State({}, state)
        for item in rule["Consumes"]:
            newState.inventory[item] -= rule["Consumes"][item]

        for item in rule["Produces"]:
            newState.inventory[item] += rule["Produces"][item]

        return newState

    return effect

def heuristic():
    return 0


Recipe = namedtuple('Recipe',['name','check','effect','cost'])
all_recipes = []
for name, rule in Crafting['Recipes'].items():
    checker = make_checker(rule)
    effector = make_effector(rule)
    recipe = Recipe(name, checker, effector, rule['Time'])
    all_recipes.append(recipe)

def graph(state):
  print "Graph called"

  for r in all_recipes:
    if r.check(state):
      yield (r.name, r.effect(state), r.cost)


def search(graph, initial, is_goal, limit, heuristic):


    prev = {initial: None}
    actions = {initial: None}
    times = {initial: 0}
    closed_set = []

    q = [(0, initial)]
    while q:

        node = heappop(q)
        print "I: ", initial
        print "I2: ", node[1]
        if node[0] > limit:
            return float('inf'), []

        closed_set.append(node[1])
        print "N: "
        if is_goal(node[1], Crafting["Goal"]):
            plan = []
            total_cost = 0
            check_node = node[1]
            total_cost = times[check_node]
            while check_node:
                if prev[check_node]:
                    plan.append(actions[check_node])
                check_node = prev[check_node]
            plan.reverse()
            return total_cost, plan

        # neighbors is a list of tuples (action, next_state, cost)
        neighbors = graph(node[1])

        #print(str(node))
        #print(node[1])

        for next_node in neighbors:
            print "NN:", next_node[0]
            if next_node[1] in closed_set:
                continue

            action, next_state, cost = next_node
            print "N1: ", action
            print "TIMES: ", times
            cost_to_next_node = times[node[1]] + cost

            if next_state not in q or cost_to_next_node < times[next_state]:
                prev[next_state] = node[1]
                actions[next_state] = action
                times[next_state] = cost + times[node[1]]
                heappush(q, (heuristic(next_state) + cost_to_next_node, next_state))



initial_state = State(Crafting["Items"])
is_goal = make_goal_checker(Crafting["Goal"])
t_heuristic = heuristic()
t_limit = 30

print "SEARCH: ", search(graph, initial_state, is_goal, t_limit, t_heuristic)
