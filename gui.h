#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include "ui_gui.h"
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QGraphicsSceneMouseEvent>
#include "nlink.h"
#include "myPixel.h"
#include "tlink.h"

#define NEUTRAL 2
#define OBJECT 1
#define BACKGROUND 0

class gui : public QMainWindow
{
    Q_OBJECT

public:
    gui(QWidget *parent = nullptr);
    ~gui();

protected:
    void mousePressEvent(QMouseEvent* event);
    bool eventFilter(QObject* obj, QEvent* event);

private slots:
    void on_btn_image_clicked();
    void on_btnGraph_clicked();
    void on_btnSave_clicked();
    void on_lineEdit_value_changed();

private:
    Ui::guiClass ui;
    double alpha;
    QString fileName;
    QImage image;
    QImage workCopyImg;
    int imgW;
    int imgH;
    double maxCap = 0.0;
    double minIntensity = INT_MAX;
    double maxIntensity = 0.0;
    bool loaded = false;
    bool objSet = false;
    bool bckSet = false;
        
    QVector<myPixel> obj;
    QVector<myPixel> bck;

    
    QVector<QVector<Nlink>> nlinks;
    QVector<Tlink> slinks;
    QVector<Tlink> tlinks;
    QVector<int> isFound;
    QVector<QVector<myPixel>> imgPixels;
    QVector<myPixel> pixels1D;
    QVector<int> objPixels;

    void clearVectors();
    void createNlinks();
    void createSTlinks();
    void createGraph();
    void addNeighbours();
    void saveAs1D();
    void getPixels();
    double maximumFlow();
    void uncheckAll();
    double getCapacity(int p, int q);
    bool checkDifferent(const QPointF& p, int obj);
    void resetSelections();
};
