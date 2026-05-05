#ifndef CELL_H
#define CELL_H

#include "Direction.h"
#include <map>

struct Cell {
    int x;
    int y;
    
    // Whether there is a wall in a specific direction
    std::map<Direction, bool> walls = {
        {Direction::Up, true},
        {Direction::Down, true},
        {Direction::Left, true},
        {Direction::Right, true}
    };
    
    // Have we visited this cell during generation?
    bool visited = false;

    // Has the mouse sensed this cell?
    bool known = false;

    Cell(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

#endif // CELL_H
