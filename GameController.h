#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QPoint>
#include <vector>
#include "MazeModel.h"

struct AttemptResult {
    bool reachedGoal = false;      
    int actionCount = 0;           
    int diffToOptimal = 0;         
    int stars = 0;                 
};

struct GameState {
    QPoint mousePos = QPoint(0,0);
    QPoint startPos = QPoint(0,0);
    QPoint goalPos = QPoint(6,7);

    int currentActionCount = 0;         
    int opportunitiesLeft = 2;          
    std::vector<AttemptResult> attempts; 

    bool gameOver = false;
    bool gameWin = false;
};

class GameController : public QObject {
    Q_OBJECT

public:
    explicit GameController(QObject *parent = nullptr);

    void startNewGame(int width = 10, int height = 10, QPoint start = QPoint(0,0), QPoint goal = QPoint(6,7));
    
    void tryMove(Direction dir);

    const GameState& getState() const { return state; }
    const MazeModel& getMaze() const { return maze; }
    MazeModel& getMazeRef() { return maze; }
    
    // The player's known map of walls
    MazeModel knownMaze; 

signals:
    void stateChanged();
    void gameFinished(bool win, int finalStars);
    void opportunityEnded(bool reachedGoal);

private:
    GameState state;
    MazeModel maze;
    int optimalSteps;

    void senseCurrentCell();
    int calculateStars(int usedSteps, int optimalSteps) const;
};

#endif // GAMECONTROLLER_H
