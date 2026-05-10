#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include "GameController.h"
#include "MazeWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateUI();
    void onGameFinished(bool win, int finalStars);
    void onOpportunityEnded(bool reachedGoal);
    void onEasterEggFound();
    void onStartGameClicked();
    void onRulesClicked();

private:
    GameController *controller;
    
    QStackedWidget *stackedWidget;
    QWidget *startScreen;
    QWidget *gameScreen;
    
    MazeWidget *mazeWidget;
    QTableWidget *attemptTable;
    QLabel *statusLabel;
    
    void setupUI();
};

#endif // MAINWINDOW_H
