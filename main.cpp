#include <QApplication>
#include "mainwindow.h"
#include "argumentparser.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString path = ArgumentParser::parse(argc, argv);

    MainWindow w;
    w.show();

    if (!path.isEmpty()) {
        w.openFile(path);
    }

    return app.exec();
}