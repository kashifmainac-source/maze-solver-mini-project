#ifndef SOLVER_H
#define SOLVER_H

#include "maze.h"

struct Result {
    std::string algo;
    bool found = false;
    std::vector<Point> visitOrder;
    std::vector<Point> path;
    double timeMs = 0;
};

Result bfs(const Maze &m);
Result dfs(const Maze &m);

#endif
