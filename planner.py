import json
with open('Crafting.json') as f:
    Crafting = json.load(f)
from collections import namedtuple
from heapq import heappush, heappop

# List of items that can be in your inventory:
print Crafting['Items']
# example: ['bench', 'cart', ..., 'wood', 'wooden_axe', 'wooden_pickaxe']

# List of items in your initial inventory with amounts:
print Crafting['Initial']
# {'coal': 4, 'plank': 1}

# List of items needed to be in your inventory at the end of the plan:
# (okay to have more than this; some might be satisfied by initial inventory)
print Crafting['Goal']
# {'stone_pickaxe': 2}

# Dict of crafting recipes (each is a dict):
print Crafting['Recipes']['craft stone_pickaxe at bench']
# example:
# {	'Produces': {'stone_pickaxe': 1},
#	'Requires': {'bench': True},
#	'Consumes': {'cobble': 3, 'stick': 2},
#	'Time': 1
# }


def make_checker(rule):
    # this code runs once
    # do something with rule['Consumes'] and rule['Requires']
    def check(state):
        # this code runs millions of times
        return True # or False

    return check

def make_effector(rule):
    # this code runs once
    # do something with rule['Produces'] and rule['Consumes']
    def effect(state):
        # this code runs millions of times
        #return next_state
        return state

    return effect

def graph(state):
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
        closed_set.append(node[1])

        if is_goal(node[1]):
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
        print(str(node))
        print(node[1])
        for next_node in graph(node[1]):
            if next_node[1] in closed_set:
                continue

            action, next_state, cost = next_node

            cost_to_next_node = times[node[1]] + cost

            if next_state not in q or cost_to_next_node < times[next_state]:
                prev[next_state] = node[1]
                actions[next_state] = action
                times[next_state] = cost + times[node[1]]
                heappush(q, (heuristic(next_state) + cost_to_next_node, next_state))

Recipe = namedtuple('Recipe',['name','check','effect','cost'])
all_recipes = []
for name, rule in Crafting['Recipes'].items:
    checker = make_checker(rule)
    effector = make_effector(rule)
    recipe = Recipe(name, checker, effector, rule['Time'])
    all_recipes.append(recipe)