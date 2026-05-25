#include "mainwindow.h"
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_model(m_buffer)
{
    // Hardcoded test data, will be replaced with FileManager usage later
    QByteArray testData;
    for (int i = 0; i < 256; ++i)
        testData.append(static_cast<char>(i));

    // Load file contents
    m_buffer.load(testData);
    // Redraw UI
    m_model.reload();

    setupTable();
    setWindowTitle("Hex Editor");
    resize(1000, 600);
}

void MainWindow::setupTable() {
    m_tableView = new EditorView(this);
    m_tableView->setEditorModel(&m_model);
    setCentralWidget(m_tableView);
}