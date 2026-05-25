#include "hexeditorapp.h"
#include "argumentparser.h"

HexEditorApp::HexEditorApp(QObject *parent) : QObject(parent) {
    m_mainWindow = new MainWindow();
}

HexEditorApp::~HexEditorApp() {
    delete m_mainWindow;
}

void HexEditorApp::initialize(int argc, char *argv[]) {
    QString path = ArgumentParser::parse(argc, argv);
    
    m_mainWindow->show();
    if (!path.isEmpty()) {
        m_mainWindow->openFile(path);
    }
}

MainWindow* HexEditorApp::mainWindow() const {
    return m_mainWindow;
}