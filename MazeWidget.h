#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H

#include <QWidget>
#include "GameController.h"

class MazeWidget : public QWidget {
    Q_OBJECT

public:
    explicit MazeWidget(GameController *controller, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GameController *controller;
    int cellSize;
    
    QPoint mapToScreen(const QPoint& p) const;
};

#endif // MAZEWIDGET_H
