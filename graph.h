#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>

class Graph : public QWidget
{
    Q_OBJECT
public:
    Graph();
    void drawHigh(int bitCell);
    void drawLow(int bitCell);
    void drawHighToLow(int bitCell);
    void drawChar(const unsigned char byte, bool evenParity, int stopBits);

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.drawPixmap(50, 20, drawingArea);
    }

private:

    const int highYLevel = 75;
    const int lowYLevel = 205;
    const int startX = 80;
    const int step = 70;
    QStringList labels;
    QPixmap drawingArea;
    void initializeDrawingArea();



};

#endif // GRAPH_H
