#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    controller = new GameController(this);
    setupUI();

    connect(controller, &GameController::stateChanged, this, &MainWindow::updateUI);
    connect(controller, &GameController::gameFinished, this, &MainWindow::onGameFinished);

    controller->startNewGame(10, 10, QPoint(0,0), QPoint(6,7));
    
    // Focus policy to accept keyboard events
    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    mazeWidget = new MazeWidget(controller, this);
    mainLayout->addWidget(mazeWidget, 2);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    statusLabel = new QLabel("当前行动次数: 0\n剩余机会: 2", this);
    statusLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    rightLayout->addWidget(statusLabel);

    attemptTable = new QTableWidget(this);
    attemptTable->setColumnCount(5);
    attemptTable->setHorizontalHeaderLabels({"尝试", "行动次数", "与最优差值", "星级", "状态"});
    attemptTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    attemptTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    attemptTable->setSelectionMode(QAbstractItemView::NoSelection);
    attemptTable->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(attemptTable);

    mainLayout->addLayout(rightLayout, 1);

    setCentralWidget(centralWidget);
    resize(800, 500);
    setWindowTitle("Qt 正方形迷宫机器鼠");
}

void MainWindow::updateUI() {
    const GameState& state = controller->getState();
    
    statusLabel->setText(QString("当前行动次数: %1\n剩余机会: %2")
                         .arg(state.currentActionCount)
                         .arg(state.opportunitiesLeft));

    attemptTable->setRowCount(state.attempts.size());
    for (int i = 0; i < state.attempts.size(); ++i) {
        const auto &a = state.attempts[i];
        attemptTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        
        if (a.reachedGoal) {
            attemptTable->setItem(i, 1, new QTableWidgetItem(QString::number(a.actionCount)));
            attemptTable->setItem(i, 2, new QTableWidgetItem(QString(a.diffToOptimal > 0 ? "+%1" : "%1").arg(a.diffToOptimal)));
            QString starsStr;
            for(int s=0; s<a.stars; ++s) starsStr += "★";
            attemptTable->setItem(i, 3, new QTableWidgetItem(starsStr));
            attemptTable->setItem(i, 4, new QTableWidgetItem("成功"));
        } else {
            attemptTable->setItem(i, 1, new QTableWidgetItem("-"));
            attemptTable->setItem(i, 2, new QTableWidgetItem("-"));
            attemptTable->setItem(i, 3, new QTableWidgetItem("-"));
            attemptTable->setItem(i, 4, new QTableWidgetItem("撞墙"));
        }
        
        for(int col=0; col<5; ++col) {
            if(attemptTable->item(i, col)) {
                attemptTable->item(i, col)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
}

void MainWindow::onGameFinished(bool win, int finalStars) {
    QString title = win ? "游戏胜利！" : "游戏失败！";
    QString msg = win ? QString("你成功到达了终点！\n最终评价: %1 星").arg(finalStars) 
                      : "很遗憾，你的机会用尽了！";
    
    QMessageBox::information(this, title, msg);
    
    // Optional: Restart automatically or wait for a button. 
    // Here we restart automatically after dismissing the message.
    controller->startNewGame(10, 10, QPoint(0,0), QPoint(6,7));
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            controller->tryMove(Direction::Up);
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            controller->tryMove(Direction::Down);
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            controller->tryMove(Direction::Left);
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            controller->tryMove(Direction::Right);
            break;
        default:
            // Let the base class handle other keys, but make sure we accept the event 
            // if we want to prevent it from propagating further when not needed.
            QMainWindow::keyPressEvent(event);
            break;
    }
}
