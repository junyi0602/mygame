#ifndef DIRECTION_H
#define DIRECTION_H

#include <QPoint>

enum class Direction {
    Up,
    Down,
    Left,
    Right,
    None
};

// Math coordinates: (0,0) is bottom-left
inline QPoint delta(Direction dir) {
    switch (dir) {
        case Direction::Up:    return QPoint(0, 1);
        case Direction::Down:  return QPoint(0, -1);
        case Direction::Left:  return QPoint(-1, 0);
        case Direction::Right: return QPoint(1, 0);
        default:               return QPoint(0, 0);
    }
}

inline Direction opposite(Direction dir) {
    switch (dir) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default:               return Direction::None;
    }
}

#endif // DIRECTION_H
