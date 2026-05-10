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
    connect(controller, &GameController::opportunityEnded, this, &MainWindow::onOpportunityEnded);

    // Focus policy to accept keyboard events
    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    stackedWidget = new QStackedWidget(this);

    // --- Start Screen ---
    startScreen = new QWidget();
    QVBoxLayout *startLayout = new QVBoxLayout(startScreen);
    startLayout->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel("发条机械鼠迷宫", startScreen);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: bold; margin-bottom: 30px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QPushButton *btnStart = new QPushButton("开始游戏", startScreen);
    btnStart->setFixedSize(200, 50);
    btnStart->setStyleSheet("font-size: 18px;");

    QPushButton *btnRules = new QPushButton("游戏规则", startScreen);
    btnRules->setFixedSize(200, 50);
    btnRules->setStyleSheet("font-size: 18px;");

    startLayout->addWidget(titleLabel);
    startLayout->addWidget(btnStart, 0, Qt::AlignHCenter);
    startLayout->addSpacing(20);
    startLayout->addWidget(btnRules, 0, Qt::AlignHCenter);

    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);
    connect(btnRules, &QPushButton::clicked, this, &MainWindow::onRulesClicked);

    // --- Game Screen ---
    gameScreen = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(gameScreen);

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

    // --- Assemble StackedWidget ---
    stackedWidget->addWidget(startScreen);
    stackedWidget->addWidget(gameScreen);
    stackedWidget->setCurrentWidget(startScreen);

    setCentralWidget(stackedWidget);
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
    QString msg = win ? QString("你成功到达了终点！\n最终评价: %1 星\n\n是否再来一局？").arg(finalStars) 
                      : "很遗憾，你的机会用尽了！\n\n是否再来一局？";
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    QPushButton *btnRestart = msgBox.addButton("再来一局", QMessageBox::AcceptRole);
    QPushButton *btnQuit = msgBox.addButton("结束游戏", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == btnRestart) {
        controller->startNewGame(10, 10, QPoint(0,0), QPoint(6,7));
        stackedWidget->setCurrentWidget(gameScreen);
        this->setFocus();
    } else {
        this->close(); // Exit the application
    }
}

void MainWindow::onOpportunityEnded(bool reachedGoal) {
    QString title = reachedGoal ? "到达终点！" : "哎呀，撞墙了！";
    QString msg = reachedGoal ? "太棒了，你成功找到了终点！\n迷宫记忆已保存，这是你的第二次机会，试试看能不能走得更快！"
                              : "你不小心撞到了墙壁，本次机会结束。\n迷宫的墙壁记忆已保存，机械鼠已回到起点，请开始你的最后一次机会！";
    
    QMessageBox::warning(this, title, msg);
}

void MainWindow::onStartGameClicked() {
    controller->startNewGame(10, 10, QPoint(0,0), QPoint(6,7));
    stackedWidget->setCurrentWidget(gameScreen);
    this->setFocus(); // Ensure main window gets focus for key events
}

void MainWindow::onRulesClicked() {
    QString rules = "【游戏规则】\n\n"
                    "1. 使用 W/A/S/D 或 方向键 控制机械鼠移动。\n"
                    "2. 迷宫中有隐藏的墙壁，探索时会记录下来。\n"
                    "3. 撞墙或到达终点会结束当前机会，你有两次机会。\n"
                    "4. 尽可能用最少的步数到达终点，步数越少星级越高！\n\n"
                    "祝你好运！";
    QMessageBox::information(this, "游戏规则", rules);
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
