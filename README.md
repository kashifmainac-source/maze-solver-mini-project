# Maze Solver Game (BFS / DFS)

BCA mini project - Shekh Kashif Husen

Console maze game in C++. You can play the maze yourself or watch BFS and DFS
solve it step by step, and compare both of them on the same maze.

## Build and run

```
g++ -std=c++17 -static main.cpp maze.cpp solver.cpp -o maze.exe
maze.exe
```

Run it from the project folder so it can find the `mazes` folder.
Works on Windows only for now (uses windows.h and conio.h for colours and keys).

## Files

- `maze.cpp` - loading maze from file and random maze generation
- `solver.cpp` - BFS (queue) and DFS (recursion)
- `main.cpp` - menu, drawing and play mode
- `mazes/` - level files. `#` is wall, `S` start, `G` goal, space is path

## Controls

W A S D or arrow keys to move, Q to quit play mode.
Any key skips the solving animation.
