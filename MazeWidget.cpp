#include "MazeWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

MazeWidget::MazeWidget(GameController *controller, QWidget *parent)
    : QWidget(parent), controller(controller), cellSize(40) {
    setMinimumSize(400, 400);
    
    // Load the mouse image
    if (!mousePixmap.load(":/mouse.png")) {
        // If loading fails, it will remain empty, and we can fallback to drawing a circle or just log it.
        qWarning("Failed to load mouse.png");
    }

    connect(controller, &GameController::stateChanged, this, [this](){
        update();
    });
}

QPoint MazeWidget::mapToScreen(const QPoint& p) const {
    const MazeModel& maze = controller->getMaze();
    int h = maze.getHeight();
    // Mathematical coordinates to screen: (x, y) -> (x, height - 1 - y)
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

    // Auto-adjust cell size
    if (w > 0 && h > 0) {
        cellSize = std::min(width() / w, height() / h);
    }

    // Fill overall background with white
    painter.fillRect(rect(), Qt::white);

    int offsetX = (width() - w * cellSize) / 2;
    int offsetY = (height() - h * cellSize) / 2;
    painter.translate(offsetX, offsetY);

    // Draw cells
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            QPoint screenPos = mapToScreen(QPoint(x, y));
            QRect rect(screenPos.x(), screenPos.y(), cellSize, cellSize);

            if (knownMaze.getCell(x, y).known) {
                painter.fillRect(rect, Qt::white); // Visited (White)
            } else {
                painter.fillRect(rect, Qt::black); // Unknown (Black)
            }

            if (QPoint(x, y) == state.startPos) {
                painter.fillRect(rect, QColor(144, 238, 144)); // Start (Light green)
            }
            if (QPoint(x, y) == state.goalPos) {
                // Draw blue circular base
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(30, 144, 255)); // Dodger Blue
                painter.drawEllipse(screenPos.x() + cellSize * 0.2, screenPos.y() + cellSize * 0.7, cellSize * 0.6, cellSize * 0.2);
                
                // Draw pole
                painter.setPen(QPen(QColor(139, 69, 19), std::max(2, cellSize / 15))); // Brown pole
                painter.drawLine(screenPos.x() + cellSize * 0.5, screenPos.y() + cellSize * 0.8, 
                                 screenPos.x() + cellSize * 0.5, screenPos.y() + cellSize * 0.2);
                
                // Draw red flag
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

    // Draw maze boundary
    QColor wallColor(255, 165, 0); // Orange/Yellow color for better visibility
    painter.setPen(QPen(wallColor, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, w * cellSize, h * cellSize);

    // Draw known walls
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

    // Draw mouse
    QPoint mouseScreenPos = mapToScreen(state.mousePos);
    if (!mousePixmap.isNull()) {
        // Scale pixmap to fit the cell nicely (e.g. 80% of cell size)
        int padding = cellSize * 0.1;
        int targetSize = cellSize * 0.8;
        painter.drawPixmap(mouseScreenPos.x() + padding, mouseScreenPos.y() + padding, 
                           targetSize, targetSize, 
                           mousePixmap.scaled(targetSize, targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // Fallback if image failed to load
        painter.setBrush(QBrush(Qt::blue));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(mouseScreenPos.x() + cellSize / 4, mouseScreenPos.y() + cellSize / 4, cellSize / 2, cellSize / 2);
    }
}
