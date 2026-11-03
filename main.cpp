#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include "maze.h"
#include "solver.h"
using namespace std;

const int TOP = 2;      // maze starts from this line on screen
int delayMs = 25;
string speedName = "Normal";

void gotoxy(int x, int y) {
    COORD pos = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void showCursor(bool show) {
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    info.bVisible = show;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void waitKey() {
    setColor(8);
    cout << "\nPress any key to go back...";
    setColor(7);
    _getch();
}

// every cell takes 2 characters so the maze looks square
void drawCell(int r, int c, char type) {
    gotoxy(c * 2, TOP + r);
    switch (type) {
    case '#': setColor(0x77); cout << "  "; break;
    case 'S': setColor(0x0A); cout << "S "; break;
    case 'G': setColor(0x0C); cout << "G "; break;
    case '@': setColor(0x0E); cout << "@ "; break;
    case '.': setColor(0x10); cout << "  "; break;
    case '*': setColor(0xE0); cout << "  "; break;
    default:  setColor(0x07); cout << "  ";
    }
    setColor(7);
}

void drawMaze(const Maze &m, const string &title) {
    system("cls");
    setColor(11);
    cout << title;
    setColor(8);
    cout << "   [" << m.name << "]";
    setColor(7);

    for (int i = 0; i < m.rows; i++)
        for (int j = 0; j < m.cols; j++)
            drawCell(i, j, m.grid[i][j]);
}

// moves cursor to a line below the maze
void below(const Maze &m, int line) {
    gotoxy(0, TOP + m.rows + 1 + line);
}

bool isEnd(const Maze &m, Point p) {
    return (p.r == m.start.r && p.c == m.start.c) || (p.r == m.goal.r && p.c == m.goal.c);
}

void printLegend(const Maze &m) {
    below(m, 0);
    setColor(0x0A); cout << "S";
    setColor(7);    cout << " start   ";
    setColor(0x0C); cout << "G";
    setColor(7);    cout << " goal   ";
    setColor(0x10); cout << "  ";
    setColor(7);    cout << " visited   ";
    setColor(0xE0); cout << "  ";
    setColor(7);    cout << " path";
}

void showSearch(const Maze &m, const Result &res) {
    drawMaze(m, "Solving with " + res.algo);
    printLegend(m);
    below(m, 1);
    setColor(8);
    cout << "(press any key to skip animation)";
    setColor(7);
    showCursor(false);

    bool skip = false;
    for (Point p : res.visitOrder) {
        if (isEnd(m, p))
            continue;
        drawCell(p.r, p.c, '.');
        if (!skip) {
            Sleep(delayMs);
            if (_kbhit()) {
                _getch();
                skip = true;
            }
        }
    }

    for (Point p : res.path) {
        if (isEnd(m, p))
            continue;
        drawCell(p.r, p.c, '*');
        if (!skip)
            Sleep(delayMs);
    }

    below(m, 1);
    cout << "                                  ";
    below(m, 2);
    cout << "Algorithm     : " << res.algo << "\n";
    cout << "Goal reached  : " << (res.found ? "Yes" : "No") << "\n";
    cout << "Visited cells : " << res.visitOrder.size() << "\n";
    if (res.found)
        cout << "Path length   : " << res.path.size() - 1 << " steps\n";
    else
        cout << "Path length   : no path exists\n";
    cout << "Time taken    : " << fixed << setprecision(3) << res.timeMs << " ms\n";

    showCursor(true);
    waitKey();
}

void playGame(const Maze &m) {
    drawMaze(m, "Play mode");
    showCursor(false);

    Point p = m.start;
    int steps = 0;
    drawCell(p.r, p.c, '@');

    below(m, 0);
    cout << "Move with W A S D or arrow keys, Q to quit";

    while (true) {
        below(m, 1);
        cout << "Steps: " << steps << "   ";

        int ch = _getch();
        int d = -1;
        if (ch == 0 || ch == 224) {
            ch = _getch();
            if (ch == 72) d = 0;
            else if (ch == 77) d = 1;
            else if (ch == 80) d = 2;
            else if (ch == 75) d = 3;
        } else {
            ch = tolower(ch);
            if (ch == 'q') break;
            if (ch == 'w') d = 0;
            else if (ch == 'd') d = 1;
            else if (ch == 's') d = 2;
            else if (ch == 'a') d = 3;
        }

        if (d == -1)
            continue;

        int nr = p.r + dr[d];
        int nc = p.c + dc[d];
        if (!isOpen(m, nr, nc))
            continue;

        drawCell(p.r, p.c, m.grid[p.r][p.c]);
        p = {nr, nc};
        steps++;
        drawCell(p.r, p.c, '@');

        if (p.r == m.goal.r && p.c == m.goal.c) {
            Result best = bfs(m);
            int shortest = best.path.size() - 1;

            below(m, 1);
            setColor(10);
            cout << "You reached the goal in " << steps << " steps!\n";
            setColor(7);
            cout << "Shortest possible path is " << shortest << " steps.\n";
            if (steps == shortest)
                cout << "Perfect, you found the shortest route.\n";
            else
                cout << "You took " << steps - shortest << " extra steps.\n";
            break;
        }
    }

    showCursor(true);
    waitKey();
}

void compare(const Maze &m) {
    Result a = bfs(m);
    Result b = dfs(m);

    system("cls");
    setColor(11);
    cout << "BFS vs DFS   ";
    setColor(8);
    cout << "[" << m.name << "]\n\n";
    setColor(7);

    if (!a.found) {
        cout << "Goal can't be reached in this maze.\n";
        waitKey();
        return;
    }

    cout << left << setw(16) << "" << setw(12) << "BFS" << "DFS\n";
    cout << "-------------------------------------\n";
    cout << setw(16) << "Visited cells" << setw(12) << a.visitOrder.size() << b.visitOrder.size() << "\n";
    cout << setw(16) << "Path length" << setw(12) << a.path.size() - 1 << b.path.size() - 1 << "\n";
    cout << setw(16) << "Time (ms)" << fixed << setprecision(3)
         << setw(12) << a.timeMs << b.timeMs << "\n\n";
    cout << right;

    int diff = (int)b.path.size() - (int)a.path.size();
    if (diff == 0)
        cout << "Both found a path of the same length.\n";
    else
        cout << "DFS path is " << diff << " steps longer than BFS (BFS always gives the shortest).\n";

    if (b.visitOrder.size() < a.visitOrder.size())
        cout << "DFS visited fewer cells here, it got lucky with its direction.\n";
    else
        cout << "BFS checked more area around the start before reaching the goal.\n";

    waitKey();
}

void chooseMaze(Maze &m) {
    system("cls");
    cout << "Choose maze\n\n";
    cout << "1. Level 1 (easy)\n";
    cout << "2. Level 2 (medium)\n";
    cout << "3. Level 3 (open rooms)\n";
    cout << "4. Random maze\n";
    cout << "\nEnter choice: ";

    char ch = _getch();
    string file = "";
    if (ch == '1') file = "mazes/level1.txt";
    else if (ch == '2') file = "mazes/level2.txt";
    else if (ch == '3') file = "mazes/level3.txt";
    else if (ch == '4') {
        generateMaze(m, 17, 35);
        return;
    } else {
        return;
    }

    Maze temp;
    if (loadMaze(temp, file)) {
        m = temp;
    } else {
        cout << "\n\nCould not load " << file << " (missing file or no S/G in it)\n";
        waitKey();
    }
}

void chooseSpeed() {
    system("cls");
    cout << "Animation speed\n\n";
    cout << "1. Slow\n2. Normal\n3. Fast\n4. Instant\n";
    cout << "\nEnter choice: ";

    char ch = _getch();
    if (ch == '1') { delayMs = 80; speedName = "Slow"; }
    else if (ch == '2') { delayMs = 25; speedName = "Normal"; }
    else if (ch == '3') { delayMs = 5; speedName = "Fast"; }
    else if (ch == '4') { delayMs = 0; speedName = "Instant"; }
}

int main() {
    srand(time(0));
    SetConsoleTitleA("Maze Solver");

    Maze maze;
    if (!loadMaze(maze, "mazes/level1.txt"))
        generateMaze(maze, 17, 35);

    while (true) {
        system("cls");
        setColor(11);
        cout << "==============================\n";
        cout << "        MAZE SOLVER GAME\n";
        cout << "==============================\n";
        setColor(7);
        cout << "Maze  : " << maze.name << " (" << maze.rows << "x" << maze.cols << ")\n";
        cout << "Speed : " << speedName << "\n\n";
        cout << "1. Play maze\n";
        cout << "2. Solve using BFS\n";
        cout << "3. Solve using DFS\n";
        cout << "4. Compare BFS and DFS\n";
        cout << "5. Change maze\n";
        cout << "6. Animation speed\n";
        cout << "0. Exit\n";
        cout << "\nEnter choice: ";

        char ch = _getch();
        if (ch == '1') playGame(maze);
        else if (ch == '2') showSearch(maze, bfs(maze));
        else if (ch == '3') showSearch(maze, dfs(maze));
        else if (ch == '4') compare(maze);
        else if (ch == '5') chooseMaze(maze);
        else if (ch == '6') chooseSpeed();
        else if (ch == '0') break;
    }

    system("cls");
    cout << "Bye!\n";
    return 0;
}
