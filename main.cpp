#include "argumentparser.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString path = ArgumentParser::parse(argc, argv);
    qDebug() << "Parsed path:" << path;

    return 0;
    // MainWindow w;
    // w.show();
    // return QCoreApplication::exec();
}
