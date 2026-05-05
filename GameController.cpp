#include "GameController.h"
#include <algorithm>

GameController::GameController(QObject *parent) : QObject(parent) {
}

void GameController::startNewGame(int width, int height, QPoint start, QPoint goal) {
    maze = MazeModel(width, height);
    knownMaze = MazeModel(width, height);
    
    // Initially all walls are unknown (assumed no wall until sensed)
    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height; ++y){
            for(auto& pair : knownMaze.getCell(x,y).walls){
                pair.second = false; // Unknown
            }
            knownMaze.getCell(x,y).known = false;
        }
    }

    maze.generateMaze(start, goal);
    optimalSteps = maze.calculateOptimalSteps(start, goal);

    state = GameState();
    state.startPos = start;
    state.goalPos = goal;
    state.mousePos = start;
    state.opportunitiesLeft = 2;
    state.currentActionCount = 0;
    state.gameOver = false;
    state.gameWin = false;

    senseCurrentCell();
    emit stateChanged();
}

void GameController::senseCurrentCell() {
    QPoint pos = state.mousePos;
    knownMaze.getCell(pos).known = true;

    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
    for(Direction dir : dirs) {
        bool hasWall = maze.hasWall(pos, dir);
        knownMaze.getCell(pos).walls[dir] = hasWall;
        
        QPoint next = pos + delta(dir);
        if(knownMaze.inBounds(next)){
            knownMaze.getCell(next).walls[opposite(dir)] = hasWall;
        }
    }
}

int GameController::calculateStars(int usedSteps, int optimalSteps) const {
    if (usedSteps <= optimalSteps + 2) return 3;
    if (usedSteps <= optimalSteps + 6) return 2;
    return 1;
}

void GameController::tryMove(Direction dir) {
    if (state.gameOver) return;

    QPoint next = state.mousePos + delta(dir);
    state.currentActionCount++;

    bool hitWall = !maze.inBounds(next) || maze.hasWall(state.mousePos, dir);
    bool reachedGoal = (next == state.goalPos);

    if (hitWall || reachedGoal) {
        AttemptResult attempt;
        attempt.actionCount = state.currentActionCount;
        attempt.reachedGoal = reachedGoal;

        if (reachedGoal) {
            attempt.diffToOptimal = attempt.actionCount - optimalSteps;
            attempt.stars = calculateStars(attempt.actionCount, optimalSteps);
        } else {
            attempt.diffToOptimal = 0;
            attempt.stars = 0;
        }

        state.attempts.push_back(attempt);

        state.opportunitiesLeft--;
        state.currentActionCount = 0;
        state.mousePos = state.startPos;
        senseCurrentCell();

        // Emit signal for the end of this opportunity
        if (state.opportunitiesLeft > 0) {
            emit opportunityEnded(reachedGoal);
        }

        if (state.opportunitiesLeft == 0) {
            state.gameOver = true;
            state.gameWin = std::any_of(
                state.attempts.begin(), state.attempts.end(),
                [](const AttemptResult &a){ return a.reachedGoal; }
            );

            int bestSteps = 999999;
            for (auto &a : state.attempts) {
                if (a.reachedGoal) {
                    bestSteps = std::min(bestSteps, a.actionCount);
                }
            }

            int finalStars = state.gameWin ? calculateStars(bestSteps, optimalSteps) : 0;
            emit gameFinished(state.gameWin, finalStars);
        }

        emit stateChanged();
        return;
    }

    // Normal move
    state.mousePos = next;
    senseCurrentCell();
    emit stateChanged();
}
