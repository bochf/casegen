import logging
from typing import List
from igraph import Graph
import csv
import collections


class Traveller:
    def __init__(self):
        self._graph = Graph()

    def load(self, filename):
        rows = None
        with open(filename, "r") as csv_file:
            rows = csv.reader(csv_file)
        
        if not rows:
            logging.error("Invalid or empty file %s", filename)
            raise ValueError
        
        header = rows[0]


class Hierholzer:
    def find_itinerary(self, tickets: List[List[str]]):
        def dfs(cur, graph, res):
            while graph[cur]:  # traversal all edges
                dfs(graph[cur].pop(), graph, res)  # visit and delete
            if not graph[cur]:  # push the vertex into the stack
                res.append(cur)

        # build the graph
        graph = collections.defaultdict(list)
        for start, end in sorted(tickets)[::-1]:
            graph[start].append(end)
        res = []
        dfs(tickets[0][0])
