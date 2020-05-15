#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setMouseTracking(true);
    ui->Original->setScene(new QGraphicsScene);
    ui->Transformed->setScene(new QGraphicsScene);
    ui->Returned->setScene(new QGraphicsScene);
    connect(ui->Original, SIGNAL(changeAll(QMatrix, QPoint)), ui->Transformed, SLOT(applyChangesInAll(QMatrix, QPoint)));
    connect(ui->Original, SIGNAL(changeAll(QMatrix, QPoint)), ui->Returned, SLOT(applyChangesInAll(QMatrix, QPoint)));
    connect(ui->Transformed, SIGNAL(changeAll(QMatrix, QPoint)), ui->Original, SLOT(applyChangesInAll(QMatrix, QPoint)));
    connect(ui->Transformed, SIGNAL(changeAll(QMatrix, QPoint)), ui->Returned, SLOT(applyChangesInAll(QMatrix, QPoint)));
    connect(ui->Returned, SIGNAL(changeAll(QMatrix, QPoint)), ui->Original, SLOT(applyChangesInAll(QMatrix, QPoint)));
    connect(ui->Returned, SIGNAL(changeAll(QMatrix, QPoint)), ui->Transformed, SLOT(applyChangesInAll(QMatrix, QPoint)));
    img = QImage();
    transformed_img = QImage();
    returned_img = QImage();
    ui->alphaLineEdit->setText(QString::fromStdString(std::to_string(alpha)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::WaveletTransform()
{
    transformed_img = img;
    time_t time = clock();
    #pragma omp parallel
    {
    #pragma omp critical
        qDebug() << "I'm here";
    QColor color1;
    QColor color2;
    #pragma omp for
    for (int y = 0; y < transformed_img.height(); y++) {
        for (int x = 0; x < transformed_img.width() && x + 1 < transformed_img.width(); x += 2)
        {
            color1 = transformed_img.pixelColor(x, y);
            color2 = transformed_img.pixelColor(x + 1, y);
            QColor col1(color1.red(), color1.green(), color1.blue(), (color1.lightness() + color2.lightness())/alpha >= 255 ? 255 : (color1.lightness() + color2.lightness())/alpha);
            QColor col2(color2.red(), color2.green(), color2.blue(), abs(color1.lightness() - color2.lightness())/alpha);
            transformed_img.setPixelColor(x, y, col1);
            transformed_img.setPixelColor(x + 1, y, col2);
        }
    }
    for (int y = 0; y < transformed_img.height(); y++) {
        for (int x = 0; x < transformed_img.width() && x + 1 < transformed_img.width(); x += 2)
        {
            color1 = transformed_img.pixelColor(x, y);
            color2 = transformed_img.pixelColor(x + 1, y);
            QColor col1(color1.red(), color1.green(), color1.blue(), (color1.lightness() + color2.lightness())/alpha >= 255 ? 255 : (color1.lightness() + color2.lightness())/alpha);
            QColor col2(color2.red(), color2.green(), color2.blue(), abs(color1.lightness() - color2.lightness())/alpha);
            transformed_img.setPixelColor(x, y, col1);
            transformed_img.setPixelColor(x + 1, y, col2);
        }
    }
    }
    qDebug() << "transform time: " << (double)(clock() - time)/CLOCKS_PER_SEC;
    transformed_img.save("transformed_img.png", "PNG");
}

void MainWindow::ReverseWaveletTransform()
{
    returned_img = transformed_img;
    time_t time = clock();
    #pragma omp parallel
    {
    QColor color1;
    QColor color2;
    #pragma omp for
    for (int y = 0; y < returned_img.height(); y++) {
        for (int x = 0; x < returned_img.width() && x + 1 < returned_img.width(); x += 2)
        {
            color1 = returned_img.pixelColor(x, y);
            color2 = returned_img.pixelColor(x + 1, y);
            QColor col1(color1.red(), color1.green(), color1.blue(), (color1.lightness() + color2.lightness())*alpha/2 >= 255 ? 255 : (color1.lightness() + color2.lightness())*alpha/2);
            QColor col2(color2.red(), color2.green(), color2.blue(), abs(color1.lightness() - color2.lightness())*alpha/2);
            returned_img.setPixelColor(x, y, col1);
            returned_img.setPixelColor(x + 1, y, col2);
        }
    }
    }
    qDebug() << "reverse transform time: " << (double)(clock() - time)/CLOCKS_PER_SEC;
    returned_img.save("returned_img.png", "PNG");
}

void MainWindow::wholeReset()
{
    ui->Original->scene()->clear();
    ui->Transformed->scene()->clear();
    ui->Returned->scene()->clear();
    ui->Original->resetMatrix();
    ui->Transformed->resetMatrix();
    ui->Returned->resetMatrix();
    ui->Original->resetTransform();
    ui->Transformed->resetTransform();
    ui->Returned->resetTransform();
    ui->ImageLineEdit->clear();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if(img.isNull()) return;
    double s = fmin((double)event->size().width()/event->oldSize().width(),
                    (double)event->size().height()/event->oldSize().height());
    ui->Original->scale(s, s);
    ui->Transformed->scale(s, s);
    ui->Returned->scale(s, s);
}

void MainWindow::on_ImageLineEdit_textChanged(const QString &pathToImage)
{
    ui->Original->scene()->clear();
    ui->Transformed->scene()->clear();
    ui->Returned->scene()->clear();
    if(pathToImage == "") return;
    img = QImage(pathToImage);
    img = img.convertToFormat(QImage::Format_ARGB32);
    img.save("img.png", "PNG");
    if(img.isNull()) {
        ui->info->setText("Image not found");
        return;
    }
    ui->info->setText("Image identified");
    ui->Original->scene()->addPixmap(QPixmap(pathToImage));
    double s = fmin((double)ui->Original->width()/img.width(), (double)ui->Original->height()/img.height());
    ui->Original->scale(s, s);
    WaveletTransform();
    ui->Transformed->scene()->addPixmap(QPixmap("transformed_img.png", "PNG"));
    ui->Transformed->scale(s, s);
    ReverseWaveletTransform();
    ui->Returned->scene()->addPixmap(QPixmap("returned_img.png", "PNG"));
    ui->Returned->scale(s, s);
}

void MainWindow::on_View_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "/home", tr("Image Files (*.png *.jpg *.bmp)"));
    ui->ImageLineEdit->setText(fileName);
}

void MainWindow::on_RefreshAlpha_clicked()
{
    alpha = ui->alphaLineEdit->text().toDouble();
    QString text = ui->ImageLineEdit->text();
    wholeReset();
    ui->ImageLineEdit->setText(text);
}
