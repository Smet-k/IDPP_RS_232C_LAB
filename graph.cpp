#include <QPainter>
#include <QPainterPath>
#include "graph.h"

Graph::Graph() : labels({"Start", "0", "0", "0", "0", "0", "0", "0", "0", "Par", "Stop1", "Stop2"}) {
    drawingArea = QPixmap(900,300);
    drawingArea.fill(Qt::white);
    initializeDrawingArea();
}

void Graph::initializeDrawingArea(){


    QPainter painter(&drawingArea);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(25, 250, 900, 250);  // X axis
    painter.drawLine(50, 50, 50, 275);    // Y axis

    painter.setPen(Qt::black);
    QFont font("Noto Sans", 10, 500);
    painter.setFont(font);
    painter.drawText(30, highYLevel, "1");
    painter.drawLine(45, highYLevel, 55, highYLevel);
    painter.drawText(30, lowYLevel, "0");
    painter.drawLine(45, lowYLevel, 55, lowYLevel);

    for (int i = 0; i < labels.size(); ++i) {
        int x = startX + i * step;
        painter.drawLine(x, 245, x, 255);
        painter.drawText(x - 10, 270, labels[i]);
    }
    update();
}

void Graph::drawHigh(int bitCell){
    QPainter painter(&drawingArea);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::black, 2));
    int x = startX + bitCell * step;
    painter.drawLine(x, highYLevel, x + step, highYLevel);
    update();
}

void Graph::drawLow(int bitCell){
    QPainter painter(&drawingArea);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::black, 2));
    int x = startX + bitCell * step;
    painter.drawLine(x, lowYLevel, x + step, lowYLevel);
    update();
}

void Graph::drawHighToLow(int bitCell){
    QPainter painter(&drawingArea);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::black, 2));
    int x = startX + bitCell * step;
    painter.drawLine(x, highYLevel, x, lowYLevel);
    update();
}

void Graph::drawChar(const unsigned char byte, bool evenParity, int stopBits){
    labels.clear();
    drawingArea.fill(Qt::white);

    int cellCount = 0;
    bool parity = false;
    if (evenParity) {
        int ones = 0;
        for (int i = 0; i < 8; ++i) {
            if (byte & (1 << i)) ones++;
        }
        parity = (ones % 2 == 1);
    }

    // Start
    drawLow(cellCount);
    drawHighToLow(cellCount);
    cellCount++;
    labels.append("Start");

    // Data
    for (int i = 0; i < 8; ++i){
        if(i > 0 && ((byte >> i) & 1) != ((byte >> (i - 1)) & 1))
            drawHighToLow(cellCount);
        else if(i == 0 && ((byte >> i) & 1) == 1){
            drawHighToLow(cellCount);
        }

        if((byte >> i) & 1){
            drawHigh(cellCount++);
        }else{
            drawLow(cellCount++);
        }

        labels.append(QString::number((byte >> i) & 1));
        if(parity != ((byte >> i) & 1) && i == 7){
            drawHighToLow(cellCount);
        }
    }

    // Parity
    if(parity)
        drawHigh(cellCount++);
    else
        drawLow(cellCount++);

    labels.append("Par");

    // Stop
    if(!parity)
        drawHighToLow(cellCount);

    drawHigh(cellCount);
    cellCount++;
    drawHigh(cellCount);

    labels.append("Stop1");
    labels.append("Stop2");

    initializeDrawingArea();
}

