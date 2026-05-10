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
    
    // 玩家已知（已探索）的墙壁地图
    MazeModel knownMaze; 

    bool isEasterEggActive() const { return m_easterEggActive; }
    void setEasterEggActive(bool active) { m_easterEggActive = active; }

signals:
    void stateChanged();
    void gameFinished(bool win, int finalStars);
    void opportunityEnded(bool reachedGoal);
    void easterEggFound();

private:
    GameState state;
    MazeModel maze;
    int optimalSteps;
    bool m_easterEggActive = false;

    void senseCurrentCell();
    int calculateStars(int usedSteps, int optimalSteps) const;
};

#endif // GAMECONTROLLER_H
