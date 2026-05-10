#include "MazeWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

MazeWidget::MazeWidget(GameController *controller, QWidget *parent)
    : QWidget(parent), controller(controller), cellSize(40) {
    setMinimumSize(400, 400);
    
    // 加载机械鼠图片
    if (!mousePixmap.load(":/mouse.png")) {
        // 如果加载失败，pixmap 将保持为空，退回到绘制圆形的备用方案
        qWarning("Failed to load mouse.png");
    }

    connect(controller, &GameController::stateChanged, this, [this](){
        update();
    });
}

QPoint MazeWidget::mapToScreen(const QPoint& p) const {
    const MazeModel& maze = controller->getMaze();
    int h = maze.getHeight();
    // 数学坐标系转换为屏幕坐标系: (x, y) -> (x, height - 1 - y)
    return QPoint(p.x() * cellSize, (h - 1 - p.y()) * cellSize);
}

void MazeWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const MazeModel& knownMaze = controller->knownMaze;
    const GameState& state = controller->getState();

    int w = knownMaze.getWidth();
    int h = knownMaze.getHeight();

    // 自动调整格子大小
    if (w > 0 && h > 0) {
        cellSize = std::min(width() / w, height() / h);
    }

    // 用白色填充整体背景
    painter.fillRect(rect(), Qt::white);

    int offsetX = (width() - w * cellSize) / 2;
    int offsetY = (height() - h * cellSize) / 2;
    painter.translate(offsetX, offsetY);

    // 绘制格子
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            QPoint screenPos = mapToScreen(QPoint(x, y));
            QRect rect(screenPos.x(), screenPos.y(), cellSize, cellSize);

            if (knownMaze.getCell(x, y).known) {
                painter.fillRect(rect, Qt::white); // 已探索（白色）
            } else {
                painter.fillRect(rect, Qt::black); // 未知（黑色）
            }

            if (QPoint(x, y) == state.startPos) {
                painter.fillRect(rect, QColor(144, 238, 144)); // 起点（浅绿色）
            }
            if (controller->isEasterEggActive() && x == w - 1 && y == h - 1) {
                // 绘制彩蛋（黄色问号）
                painter.setPen(QColor(255, 215, 0)); // 金色/黄色
                QFont font = painter.font();
                font.setPixelSize(cellSize * 0.8);
                font.setBold(true);
                painter.setFont(font);
                painter.drawText(rect, Qt::AlignCenter, "?");
            } else if (QPoint(x, y) == state.goalPos) {
                // 绘制蓝色圆形底座
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(30, 144, 255)); // 亮蓝色
                painter.drawEllipse(screenPos.x() + cellSize * 0.2, screenPos.y() + cellSize * 0.7, cellSize * 0.6, cellSize * 0.2);
                
                // 绘制旗杆
                painter.setPen(QPen(QColor(139, 69, 19), std::max(2, cellSize / 15))); // 棕色旗杆
                painter.drawLine(screenPos.x() + cellSize * 0.5, screenPos.y() + cellSize * 0.8, 
                                 screenPos.x() + cellSize * 0.5, screenPos.y() + cellSize * 0.2);
                
                // 绘制红色小旗子
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::red);
                QPolygon flag;
                flag << QPoint(screenPos.x() + cellSize * 0.5, screenPos.y() + cellSize * 0.2)
                     << QPoint(screenPos.x() + cellSize * 0.85, screenPos.y() + cellSize * 0.35)
                     << QPoint(screenPos.x() + cellSize * 0.5, screenPos.y() + cellSize * 0.5);
                painter.drawPolygon(flag);
            }
        }
    }

    // 绘制迷宫边界
    QColor wallColor(255, 165, 0); // 橙黄色，以提高可见度
    painter.setPen(QPen(wallColor, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, w * cellSize, h * cellSize);

    // 绘制已探索的墙壁
    painter.setPen(QPen(wallColor, 3));
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (!knownMaze.getCell(x, y).known) continue;

            QPoint screenPos = mapToScreen(QPoint(x, y));
            const Cell& cell = knownMaze.getCell(x, y);

            if (cell.walls.at(Direction::Up)) {
                painter.drawLine(screenPos.x(), screenPos.y(), screenPos.x() + cellSize, screenPos.y());
            }
            if (cell.walls.at(Direction::Down)) {
                painter.drawLine(screenPos.x(), screenPos.y() + cellSize, screenPos.x() + cellSize, screenPos.y() + cellSize);
            }
            if (cell.walls.at(Direction::Left)) {
                painter.drawLine(screenPos.x(), screenPos.y(), screenPos.x(), screenPos.y() + cellSize);
            }
            if (cell.walls.at(Direction::Right)) {
                painter.drawLine(screenPos.x() + cellSize, screenPos.y(), screenPos.x() + cellSize, screenPos.y() + cellSize);
            }
        }
    }

    // 绘制机械鼠
    QPoint mouseScreenPos = mapToScreen(state.mousePos);
    if (!mousePixmap.isNull()) {
        // 缩放图片使其完美适应格子大小（例如格子的80%大小）
        int padding = cellSize * 0.1;
        int targetSize = cellSize * 0.8;
        painter.drawPixmap(mouseScreenPos.x() + padding, mouseScreenPos.y() + padding, 
                           targetSize, targetSize, 
                           mousePixmap.scaled(targetSize, targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 如果图片加载失败，退回到绘制蓝色的圆形
        painter.setBrush(QBrush(Qt::blue));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(mouseScreenPos.x() + cellSize / 4, mouseScreenPos.y() + cellSize / 4, cellSize / 2, cellSize / 2);
    }
}
