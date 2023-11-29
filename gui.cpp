#include "gui.h"

gui::gui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->alpha = 1;
    fileName = nullptr;
    ui.lbl_image->setMouseTracking(true);
    ui.lbl_image->installEventFilter(this);
    connect(ui.lineEdit, &QLineEdit::textChanged, this, &gui::on_lineEdit_value_changed);
    ui.lineEdit->setText("1");
}

gui::~gui()
{
}

void gui::on_lineEdit_value_changed()
{
    QString value = ui.lineEdit->text();

    bool ok;
    double doubleValue = value.toDouble(&ok);

    if (!ok || doubleValue <= 0) {
        ui.lineEdit->setText("1");
    }

    this->alpha = ui.lineEdit->text().toDouble();
}


void gui::on_btnSave_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.jpg *.jpeg *.jfif)"));

    if (!filePath.isEmpty()) {
        QPixmap pixmap = ui.lbl_image->pixmap();
        pixmap.save(filePath);
    }
}

void gui::on_btn_image_clicked() {
    fileName = QFileDialog::getOpenFileName(this, tr("Choose Image"), "", tr("Images(*.png *.jpg *.jpeg *.jfif)"));

    if (QString::compare(fileName, QString()) != 0) {
        image = QImage(fileName);

        imgH = image.height();
        imgW = image.width();
            
        ui.lbl_image->clear();
        ui.lbl_image->resize(imgW, imgH);
        ui.lbl_image->setPixmap(QPixmap::fromImage(image));
        ui.lbl_image->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        loaded = true;

        ui.lbl_image->move(0, 0);
    }
}

void gui::uncheckAll()
{
    isFound.fill(0);
}

void gui::resetSelections()
{
    loaded = false;
    objSet = false;
    bckSet = false;
    ui.lbl_obj->setText("Not selected");
    ui.lbl_bck->setText("Not selected");
}
void gui::on_btnGraph_clicked()
{
    if (bckSet && objSet) {
        getPixels();
        addNeighbours();
        createGraph();
        isFound.resize(pixels1D.size());
        maximumFlow();
        ui.btnSave->setEnabled(true);
        resetSelections();
        clearVectors();
    }
    else {
        QMessageBox::critical(this, "Error", "Select object and background seed pixels");
    }
}

void gui::clearVectors() {
    obj.clear();
    bck.clear();
    imgPixels.clear();
    nlinks.clear();
    slinks.clear();
    tlinks.clear();
    isFound.clear();
    pixels1D.clear();
}

void gui::mousePressEvent(QMouseEvent* event)
{
    if (!ui.lbl_image->pixmap().isNull()) {
        QPointF pos = event->localPos();
    }
}

bool gui::checkDifferent(const QPointF& p, int obj) {
    return true;
}

bool gui::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui.lbl_image && event->type() == QEvent::Enter)
    {
        QApplication::setOverrideCursor(Qt::CrossCursor);
    }
    else if (obj == ui.lbl_image && event->type() == QEvent::Leave)
    {
        QApplication::restoreOverrideCursor();
    }
    if (obj == ui.lbl_image && event->type() == QEvent::MouseMove && loaded) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF pos = mouseEvent->localPos();
        int x = pos.x();
        int y = pos.y();
        if (pos.x() >= 0 && pos.y() >= 0 && pos.x() < ui.lbl_image->pixmap().width() && pos.y() < ui.lbl_image->pixmap().height()) {
            ui.lbl_info->setText(QString("X: %1 Y: %2").arg(x).arg(y));
        }
    }
    if (obj == ui.lbl_image && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            QPointF pos = mouseEvent->localPos();
            QColor color = QColor(ui.lbl_image->pixmap().toImage().pixel(pos.toPoint()));
            int x = pos.x();
            int y = pos.y();
            if (x >= 0 && y >= 0 && x < ui.lbl_image->pixmap().width() && y < ui.lbl_image->pixmap().height()) {
                if (ui.btn_obj->isChecked() && loaded && checkDifferent(pos,1)) {
                    objSet = true;
                    myPixel newPixel;
                    newPixel.setCoords(QPoint(x, y));
                    this->obj.push_back(newPixel);
                    ui.lbl_obj->setText(QString("Selected"));
                }
                if (ui.btn_bck->isChecked() && loaded && checkDifferent(pos,0)) {
                    bckSet = true;
                    myPixel newPixel;
                    newPixel.setCoords(QPoint(x, y));
                    this->bck.push_back(newPixel);
                    ui.lbl_bck->setText(QString("Selected"));
                }
            }
            else {
                qDebug() << "Invalid pixel coordinate: " << pos;
            }
            double intensity = (static_cast<double>(color.red()) + color.green() + color.blue()) /3;
        }
    }
    return QObject::eventFilter(obj, event);
}

void gui::saveAs1D() {
    pixels1D.resize(imgH * imgW);
    maxIntensity = std::numeric_limits<double>::min();
    minIntensity = std::numeric_limits<double>::max();

    for (int j = 0; j < imgH; j++) {
        for (int i = 0; i < imgW; i++) {
            myPixel& curVertex = imgPixels[i][j];
            double intensity = curVertex.calcIntensity();
            maxIntensity = std::max(maxIntensity, intensity);
            minIntensity = std::min(minIntensity, intensity);
            pixels1D[j * imgW + i] = curVertex;
        }
    }
}

void gui::createGraph() {
    saveAs1D();
    createNlinks();
    createSTlinks();
}

void gui::createSTlinks()
{
    slinks.resize(pixels1D.size());
    tlinks.resize(pixels1D.size());

    int sumIntensity = 0;
    for (auto& bckPixel : bck) {
        sumIntensity += imgPixels[bckPixel.getX()][bckPixel.getY()].calcIntensity();
    }
    double avgIntensityBkg = static_cast<double>(sumIntensity) / bck.size();

    sumIntensity = 0;
    for (auto& objPixel : obj) {
        sumIntensity += imgPixels[objPixel.getX()][objPixel.getY()].calcIntensity();
    }
    double avgIntensityObj = static_cast<double>(sumIntensity) / obj.size();


    for (int i = 0; i < pixels1D.size(); i++) {
        double capB = 0.0;
        double capO = 0.0;

        switch (pixels1D[i].getPixelGroup()) {
        case NEUTRAL:
            capB = maxIntensity - std::abs(avgIntensityBkg - pixels1D[i].calcIntensity());
            capO = maxIntensity - std::abs(avgIntensityObj - pixels1D[i].calcIntensity());
            capB = this->alpha * capB;
            capO = this->alpha * capO;

            tlinks[i].setCapacity(capB);
            tlinks[i].setReserve(capB);
            slinks[i].setCapacity(capO);
            slinks[i].setReserve(capO);

            tlinks[i].setIndex(i);
            slinks[i].setIndex(i);

            break;

        case OBJECT:
            tlinks[i].setCapacity(0);
            tlinks[i].setReserve(0);
            slinks[i].setCapacity(INT_MAX);
            slinks[i].setReserve(INT_MAX);

            tlinks[i].setIndex(i);
            slinks[i].setIndex(i);

            break;

        case BACKGROUND:
            tlinks[i].setCapacity(INT_MAX);
            tlinks[i].setReserve(INT_MAX);
            slinks[i].setCapacity(0);
            slinks[i].setReserve(0);

            tlinks[i].setIndex(i);
            slinks[i].setIndex(i);

            break;
        }
    }
}

void gui::addNeighbours()
{
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            myPixel& currentPixel = imgPixels[i][j];

            if (i > 0) {
                currentPixel.appendNeighbour(imgW * j + i - 1);
            }
            if (i < image.width() - 1) {
                currentPixel.appendNeighbour(imgW * j + i + 1);
            }
            if (j > 0) {
                currentPixel.appendNeighbour(imgW * (j - 1) + i);
            }
            if (j < image.height() - 1) {
                currentPixel.appendNeighbour(imgW * (j + 1) + i);
            }
            currentPixel.setPixelGroup(NEUTRAL);
        }
    }

    for (auto& bckPixel : bck) {
        imgPixels[bckPixel.getX()][bckPixel.getY()].setPixelGroup(BACKGROUND);
    }

    for (auto& objPixel : obj) {
        imgPixels[objPixel.getX()][objPixel.getY()].setPixelGroup(OBJECT);
    }
}

void gui::createNlinks() {
    double capMax = -1;
    nlinks.resize(pixels1D.size());

    for (int i = 0; i < pixels1D.size(); i++) {
        myPixel& currentPixel = pixels1D[i];
        int n = currentPixel.getNeighboursNum();

        for (int k = 0; k < n; k++) {
            int index = currentPixel.getNeighbourAtIndex(k);
            double cap = getCapacity(i, index);
            capMax = std::max(capMax, cap);

            Nlink newNlink(cap, cap, i, index);
            nlinks[i].append(newNlink);
        }
    }
    maxCap = capMax;
}



double gui::getCapacity(int p1, int p2)
{
    myPixel pixel1 = pixels1D[p1];
    myPixel pixel2 = pixels1D[p2];

    double cap = (maxIntensity - minIntensity) - abs(pixel1.calcIntensity() - pixel2.calcIntensity());

    return cap;
}

void gui::getPixels() {
    int imgWidth = image.width();
    int imgHeight = image.height();

    imgPixels.resize(imgWidth);

    for (int i = 0; i < imgWidth; i++) {
        imgPixels[i].resize(imgHeight);

        for (int j = 0; j < imgHeight; j++) {
            QColor tempColor = image.pixel(i, j);
            QPoint tempCoords(i, j);
            myPixel tempPix(tempColor, tempCoords);

            imgPixels[i][j] = tempPix;
        }
    }
}

double gui::maximumFlow()
{
    int maxIter = 5000;
    double maxFlow = 0.0;

    int pos = 0;
    int cur = 0;
    QPoint prev = QPoint(-1, -1);

    std::vector<int> foundPixels(pixels1D.size() + 1);
    foundPixels.assign(pixels1D.size() + 1, -1);

    int len = 0;
    int last = 0;

    std::vector<QPoint> temp(pixels1D.size() + 1, QPoint(-1, -1));
    temp.resize(pixels1D.size() + 1);

    double pathRes = INT_MAX;
    bool foundT = true;

    for (int i = 0; i < pixels1D.size(); i++) {
        pathRes = std::min(slinks[i].getReserve(), tlinks[i].getReserve());
        if (pathRes > 0) {
            tlinks[i].update(pathRes);
            slinks[i].update(pathRes);
            maxFlow += pathRes;
        }
    }
    
    for (int iter = 0; iter < maxIter; iter++) {
        pos = 0;
        foundT = false;

        foundPixels.assign(pixels1D.size() + 1, -1);
        temp.assign(pixels1D.size() + 1, QPoint(-1, -1));
        len = 0;
        last = 0;
        foundT = false;
        uncheckAll();

        for (int i = 0; i < pixels1D.size(); i++) {
            if (slinks[i].getReserve() > 0 && isFound[i] == 0) {
                isFound[i] = 1;
                foundPixels[len++] = i;
                pixels1D[i].nLinkTo = QPoint(-1, -1);
            }
        }

        while (foundPixels[last] != -1) {
            int temp = foundPixels[last];
            for (int i = 0; i < pixels1D[temp].getNeighboursNum(); i++) {
                Nlink& neighbourLink = nlinks[temp][i];
                int help = neighbourLink.getTo();
                if (neighbourLink.getReserve() > 0 && isFound[help] == 0) {
                    isFound[help] = 1;
                    pixels1D[help].nLinkTo = QPoint(temp, i);
                    foundPixels[len++] = help;
                }
            }

            if (tlinks[foundPixels[last]].getReserve() > 0) {
                foundT = true;
                break; 
            }
            last++;
        }

        if (foundT == false || iter == maxIter - 1) {
            QPixmap pixmap = ui.lbl_image->pixmap().copy();
            QImage img = pixmap.toImage();
            QColor redColor(255, 0, 0);

            QColor setColor(imgPixels[obj[0].getCoords().x()][obj[0].getCoords().y()].getColor());

            for (int i = 0; i < foundPixels.size(); i++) {
                if (foundPixels[i] != -1) {
                    int x = foundPixels[i] % img.width();
                    int y = foundPixels[i] / img.width();
                    if (ui.highlight_obj->isChecked()) {
                        img.setPixelColor(x, y, redColor);
                    }
                    else {
                        img.setPixelColor(x, y, setColor);
                    }
                }
            }
            ui.lbl_image->setPixmap(QPixmap::fromImage(img));
            break;
        }

        if (foundT == true) {
            pos = 0;
            pathRes = INT_MAX;
            cur = foundPixels[last];

            prev = pixels1D[cur].nLinkTo;

            //(-1,-1) bol oznaceny zdroj
            while (prev != QPoint(-1, -1)) {
                temp[pos] = prev;

                cur = nlinks[prev.x()][prev.y()].getFrom();
                pathRes = std::min(pathRes, nlinks[prev.x()][prev.y()].getReserve());

                prev = pixels1D[cur].nLinkTo;
                pos++;
            }

            maxFlow += pathRes;
            tlinks[nlinks[temp[0].x()][temp[0].y()].getTo()].update(pathRes);
            slinks[nlinks[temp[pos - 1].x()][temp[pos - 1].y()].getFrom()].update(pathRes);

            for (int i = 0; i < pos; i++) {
                nlinks[temp[i].x()][temp[i].y()].updateReserve(pathRes);
            }
        }
    }
    return maxFlow;
}

