#include "mainwindow.h"
#include <QVBoxLayout>

#include "graph.h"
#include "rshelper.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("COMPorts");
    QWidget *central = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    graph = new Graph();

    RSHelper *helper = new RSHelper();

    connect(helper, &RSHelper::bitSelected, this, &MainWindow::drawGraph);

    mainLayout->addWidget(graph, 5);
    mainLayout->addWidget(helper);

    setCentralWidget(central);
}

void MainWindow::drawGraph(unsigned char symbol){
    graph->drawChar(symbol, 1, 2);
}
