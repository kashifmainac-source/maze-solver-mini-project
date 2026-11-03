#include "solver.h"
#include <queue>
#include <algorithm>
#include <chrono>
using namespace std;

Result bfs(const Maze &m) {
    Result res;
    res.algo = "BFS";
    auto t1 = chrono::high_resolution_clock::now();

    vector<vector<bool>> visited(m.rows, vector<bool>(m.cols, false));
    vector<vector<Point>> parent(m.rows, vector<Point>(m.cols, {-1, -1}));
    queue<Point> q;

    q.push(m.start);
    visited[m.start.r][m.start.c] = true;

    while (!q.empty()) {
        Point cur = q.front();
        q.pop();
        res.visitOrder.push_back(cur);

        if (cur.r == m.goal.r && cur.c == m.goal.c) {
            res.found = true;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nr = cur.r + dr[i];
            int nc = cur.c + dc[i];
            if (isOpen(m, nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = true;
                parent[nr][nc] = cur;
                q.push({nr, nc});
            }
        }
    }

    if (res.found) {
        Point p = m.goal;
        while (p.r != -1) {
            res.path.push_back(p);
            p = parent[p.r][p.c];
        }
        reverse(res.path.begin(), res.path.end());
    }

    auto t2 = chrono::high_resolution_clock::now();
    res.timeMs = chrono::duration<double, milli>(t2 - t1).count();
    return res;
}

static bool dfsVisit(const Maze &m, int r, int c, vector<vector<bool>> &visited, Result &res) {
    if (!isOpen(m, r, c) || visited[r][c])
        return false;

    visited[r][c] = true;
    res.visitOrder.push_back({r, c});
    res.path.push_back({r, c});

    if (r == m.goal.r && c == m.goal.c)
        return true;

    for (int i = 0; i < 4; i++)
        if (dfsVisit(m, r + dr[i], c + dc[i], visited, res))
            return true;

    // dead end, so this cell is not part of the path
    res.path.pop_back();
    return false;
}

Result dfs(const Maze &m) {
    Result res;
    res.algo = "DFS";
    auto t1 = chrono::high_resolution_clock::now();

    vector<vector<bool>> visited(m.rows, vector<bool>(m.cols, false));
    res.found = dfsVisit(m, m.start.r, m.start.c, visited, res);

    auto t2 = chrono::high_resolution_clock::now();
    res.timeMs = chrono::duration<double, milli>(t2 - t1).count();
    return res;
}
