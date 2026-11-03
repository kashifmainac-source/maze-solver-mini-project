#ifndef MAZE_H
#define MAZE_H

#include <string>
#include <vector>

struct Point {
    int r, c;
};

struct Maze {
    std::vector<std::string> grid;
    int rows = 0, cols = 0;
    Point start, goal;
    std::string name;
};

// up, right, down, left
const int dr[4] = {-1, 0, 1, 0};
const int dc[4] = {0, 1, 0, -1};

bool loadMaze(Maze &m, const std::string &fileName);
void generateMaze(Maze &m, int rows, int cols);
bool isOpen(const Maze &m, int r, int c);

#endif
