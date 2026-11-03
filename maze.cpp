#include "maze.h"
#include <fstream>
#include <cstdlib>
using namespace std;

bool isOpen(const Maze &m, int r, int c) {
    if (r < 0 || c < 0 || r >= m.rows || c >= m.cols)
        return false;
    return m.grid[r][c] != '#';
}

bool loadMaze(Maze &m, const string &fileName) {
    ifstream fin(fileName);
    if (!fin)
        return false;

    m.grid.clear();
    string line;
    while (getline(fin, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty())
            m.grid.push_back(line);
    }
    if (m.grid.empty())
        return false;

    m.rows = m.grid.size();
    m.cols = 0;
    for (string &row : m.grid)
        if ((int)row.size() > m.cols)
            m.cols = row.size();

    bool foundS = false, foundG = false;
    for (int i = 0; i < m.rows; i++) {
        m.grid[i].resize(m.cols, '#');
        for (int j = 0; j < m.cols; j++) {
            char ch = m.grid[i][j];
            if (ch == 'S') {
                m.start = {i, j};
                foundS = true;
            } else if (ch == 'G') {
                m.goal = {i, j};
                foundG = true;
            } else if (ch != '#') {
                m.grid[i][j] = ' ';
            }
        }
    }

    m.name = fileName;
    return foundS && foundG;
}

// recursive backtracker, jumps 2 cells at a time and breaks the wall in between
static void carve(Maze &m, int r, int c) {
    int dirs[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(dirs[i], dirs[j]);
    }

    for (int k = 0; k < 4; k++) {
        int d = dirs[k];
        int nr = r + dr[d] * 2;
        int nc = c + dc[d] * 2;
        if (nr > 0 && nc > 0 && nr < m.rows - 1 && nc < m.cols - 1 && m.grid[nr][nc] == '#') {
            m.grid[r + dr[d]][c + dc[d]] = ' ';
            m.grid[nr][nc] = ' ';
            carve(m, nr, nc);
        }
    }
}

void generateMaze(Maze &m, int rows, int cols) {
    if (rows % 2 == 0) rows++;
    if (cols % 2 == 0) cols++;

    m.rows = rows;
    m.cols = cols;
    m.grid.assign(rows, string(cols, '#'));
    m.grid[1][1] = ' ';
    carve(m, 1, 1);

    // a perfect maze has only one route, so BFS and DFS would always give the
    // same path. removing a few walls makes loops and shows the difference
    int extra = rows * cols / 30;
    for (int tries = 0; tries < 2000 && extra > 0; tries++) {
        int r = 1 + rand() % (rows - 2);
        int c = 1 + rand() % (cols - 2);
        if (m.grid[r][c] != '#')
            continue;

        bool leftRight = m.grid[r][c - 1] == ' ' && m.grid[r][c + 1] == ' ' &&
                         m.grid[r - 1][c] == '#' && m.grid[r + 1][c] == '#';
        bool upDown = m.grid[r - 1][c] == ' ' && m.grid[r + 1][c] == ' ' &&
                      m.grid[r][c - 1] == '#' && m.grid[r][c + 1] == '#';
        if (leftRight || upDown) {
            m.grid[r][c] = ' ';
            extra--;
        }
    }

    m.start = {1, 1};
    m.goal = {rows - 2, cols - 2};
    m.grid[1][1] = 'S';
    m.grid[rows - 2][cols - 2] = 'G';
    m.name = "Random maze";
}
