#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QResizeEvent>
#include <QInputEvent>

#include <bits/stdc++.h>
#include "omp.h"
#include "time.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void WaveletTransform();
    void ReverseWaveletTransform();
    void wholeReset();

public slots:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_ImageLineEdit_textChanged(const QString &arg1);

    void on_View_clicked();

    void on_RefreshAlpha_clicked();

private:
    Ui::MainWindow *ui;
    QImage img;
    QImage transformed_img;
    QImage returned_img;
    double alpha = sqrt(2);
};
#endif // MAINWINDOW_H
