#ifndef MAZEMODEL_H
#define MAZEMODEL_H

#include "Cell.h"
#include <vector>
#include <QPoint>

class MazeModel {
public:
    MazeModel(int width = 10, int height = 10);

    void generateMaze(QPoint start, QPoint goal);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool inBounds(const QPoint& p) const;
    bool inBounds(int x, int y) const;

    Cell& getCell(int x, int y);
    const Cell& getCell(int x, int y) const;
    
    Cell& getCell(const QPoint& p);
    const Cell& getCell(const QPoint& p) const;

    bool hasWall(const QPoint& p, Direction dir) const;

    // BFS to find shortest path from start to goal in the actual maze
    int calculateOptimalSteps(QPoint start, QPoint goal) const;

private:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

    void carvePassagesFrom(int x, int y);
};

#endif // MAZEMODEL_H
