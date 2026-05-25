#include <QApplication>
#include "mainwindow.h"
#include "argumentparser.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString path = ArgumentParser::parse(argc, argv);
    Q_UNUSED(path) // to be used later

    MainWindow w;
    w.show();

    return app.exec();
}