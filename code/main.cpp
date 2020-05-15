#include "mainwindow.h"
#include <omp.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

#if defined(_OPENMP)
    printf("Compiled by an OpenMP-compliant implementation.\n");
    printf("The result of omp_get_num_threads %i\n", omp_get_num_threads());
#endif

    w.show();
    return a.exec();
}
