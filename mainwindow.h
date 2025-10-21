#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
private:
    Graph *graph;
private slots:
    void drawGraph(unsigned char symbol);

};
#endif // MAINWINDOW_H
