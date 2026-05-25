#include <QApplication>
#include "hexeditorapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    HexEditorApp hexApp;
    hexApp.initialize(argc, argv);

    return app.exec();
}