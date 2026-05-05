#include "MazeModel.h"
#include <random>
#include <algorithm>
#include <queue>
#include <chrono>

MazeModel::MazeModel(int width, int height) : width(width), height(height) {
    grid.resize(width, std::vector<Cell>(height));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            grid[x][y] = Cell(x, y);
        }
    }
}

bool MazeModel::inBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool MazeModel::inBounds(const QPoint& p) const {
    return inBounds(p.x(), p.y());
}

Cell& MazeModel::getCell(int x, int y) {
    return grid[x][y];
}

const Cell& MazeModel::getCell(int x, int y) const {
    return grid[x][y];
}

Cell& MazeModel::getCell(const QPoint& p) {
    return grid[p.x()][p.y()];
}

const Cell& MazeModel::getCell(const QPoint& p) const {
    return grid[p.x()][p.y()];
}

bool MazeModel::hasWall(const QPoint& p, Direction dir) const {
    if (!inBounds(p)) return true;
    return getCell(p).walls.at(dir);
}

void MazeModel::generateMaze(QPoint start, QPoint goal) {
    // Reset
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            grid[x][y].visited = false;
            grid[x][y].known = false;
            for (auto& pair : grid[x][y].walls) {
                pair.second = true;
            }
        }
    }

    carvePassagesFrom(start.x(), start.y());

    // Optional: remove some random walls to create multiple paths
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distX(0, width - 1);
    std::uniform_int_distribution<int> distY(0, height - 1);
    std::uniform_int_distribution<int> distDir(0, 3);
    
    int removeCount = (width * height) / 10;
    for (int i = 0; i < removeCount; ++i) {
        int x = distX(rng);
        int y = distY(rng);
        Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
        Direction dir = dirs[distDir(rng)];
        
        QPoint p(x, y);
        QPoint next = p + delta(dir);
        
        if (inBounds(next)) {
            getCell(p).walls[dir] = false;
            getCell(next).walls[opposite(dir)] = false;
        }
    }
}

void MazeModel::carvePassagesFrom(int cx, int cy) {
    getCell(cx, cy).visited = true;

    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
    
    // Shuffle directions
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(dirs), std::end(dirs), std::default_random_engine(seed));

    for (Direction dir : dirs) {
        QPoint next = QPoint(cx, cy) + delta(dir);
        
        if (inBounds(next) && !getCell(next).visited) {
            // Remove walls
            getCell(cx, cy).walls[dir] = false;
            getCell(next).walls[opposite(dir)] = false;
            
            carvePassagesFrom(next.x(), next.y());
        }
    }
}

int MazeModel::calculateOptimalSteps(QPoint start, QPoint goal) const {
    if (start == goal) return 0;

    std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, false));
    std::queue<std::pair<QPoint, int>> q;

    q.push({start, 0});
    visited[start.x()][start.y()] = true;

    while (!q.empty()) {
        auto [curr, steps] = q.front();
        q.pop();

        if (curr == goal) {
            return steps;
        }

        Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
        for (Direction dir : dirs) {
            if (!hasWall(curr, dir)) {
                QPoint next = curr + delta(dir);
                if (inBounds(next) && !visited[next.x()][next.y()]) {
                    visited[next.x()][next.y()] = true;
                    q.push({next, steps + 1});
                }
            }
        }
    }

    return -1; // Unreachable
}
