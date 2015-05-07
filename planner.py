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

# class State:
#
#     def __init__(self, inventory, state=None):
#         self.inventory = dict((el,0) for el in inventory)
#         if (state is not None): self.inventory = state.inventory
#
#     def __hash__(self):
#         h = frozenset(self.inventory.items()).__hash__()
#         return h
#
#     def __eq__(self,other):
#         pass
#
#     def has_items(self, goals):
#         has_item = True
#
#         for item in goals:
#             if (self.inventory[item] < goals[item]):
#                 has_item = False
#                 break
#             else:
#                 print "HAS A GOAL: ", item
#
#         return has_item

def generate_initial_state():
    return tuple((i,0) for i in Crafting['Items'])

def normalize_state(items):
    new_state = []
    for i in Crafting['Items']:
            if i in items:
                new_state.append((i, items[i]))
            else:
                new_state.append((i, 0))
    return tuple(new_state)

def next_state(state, cons, prod):
    newState = list(state)

    for i in range(0, len(state)):
    # for item in rule["Consumes"]:
        newState[i] = (newState[i][0], newState[i][1] - cons[i][1])

    #     newState.inventory[item] -= rule["Consumes"][item]
        newState[i] = (newState[i][0], newState[i][1] + prod[i][1])
    # for item in rule["Produces"]:
    #     newState.inventory[item] += rule["Produces"][item]
    return tuple(newState)

def has_items(state, goals):
    has_item = True
    n_goals = normalize_state(goals)

    for i in range(0, len(n_goals)):
    # for item in goals:
        if (state[i][1] < n_goals[i][1]):
            has_item = False
            break
        else:
            # print "HAS A GOAL: ", state[i]
            pass
    return has_item

    
def make_goal_checker(goals):
    # this code runs once
    def is_goal(state, goals):
        # this code runs millions of times
        return has_items(state, goals)
        #return True # or False

    return is_goal


def make_checker(rule):
    # this code runs once
    # do something with rule['Consumes'] and rule['Requires']
    def check(state):
        satisfiesConsumes = True
        satisfiesRequires = True
        if 'Consumes' in rule.keys():
            satisfiesConsumes = has_items(state, rule["Consumes"])

        if 'Produces' in rule.keys():
            
            pass

        if 'Requires' in rule.keys():
            satisfiesRequires = has_items(state, rule["Requires"])
        #print rule
        # this code runs millions of times

        return (satisfiesConsumes and satisfiesRequires) # or False

    return check


def make_effector(rule):
    # this code runs once
    # do something with rule['Produces'] and rule['Consumes']
    def effect(state):
        # this code runs millions of times
        #return next_state
        if 'Consumes' in rule:
            newState = next_state(state, normalize_state(rule["Consumes"]), normalize_state(rule["Produces"]))
        else:
            newState = next_state(state, generate_initial_state(), normalize_state(rule["Produces"]))

        return newState

    return effect


def t_heuristic(state):
    return 0


def graph(state):
  # print "Graph called"

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
        # print "I: ", initial
        # print "I2: ", node[1]
        if node[0] > limit:
            return float('inf'), []

        closed_set.append(node[1])
        # print "N: "
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
            # print "NN:", next_node[0]
            if next_node[1] in closed_set:
                continue

            action, next_state, cost = next_node
            # print "N1: ", action
            # print "TIMES: ", times
            cost_to_next_node = times[node[1]] + cost

            if next_state not in q or cost_to_next_node < times[next_state]:
                prev[next_state] = node[1]
                actions[next_state] = action
                times[next_state] = cost + times[node[1]]
                priority = heuristic(next_state) + cost_to_next_node
                heappush(q, (priority, next_state))



Recipe = namedtuple('Recipe',['name','check','effect','cost'])
all_recipes = []
for name, rule in Crafting['Recipes'].items():
    checker = make_checker(rule)
    effector = make_effector(rule)
    recipe = Recipe(name, checker, effector, rule['Time'])
    all_recipes.append(recipe)

# initial_state = State(Crafting["Items"])
initial_state = generate_initial_state()
is_goal = make_goal_checker(Crafting["Goal"])
# t_heuristic = heuristic()
t_limit = 30

print "SEARCH: ", search(graph, initial_state, is_goal, t_limit, t_heuristic)
