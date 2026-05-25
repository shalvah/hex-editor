#include "mainwindow.h"
#include <QHeaderView>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_model(m_buffer)
{
    setupTable();
    setupFindPanel(); // Set up Find panel before setting up the menu bar, so m_findAction references an existing handle
    setupMenuBar();
    refreshWindowElements();
    resize(1100, 650);
}

void MainWindow::setupTable() {
    m_tableView = new EditorView(this);
    m_tableView->setEditorModel(&m_model);
    setCentralWidget(m_tableView);
}

void MainWindow::setupMenuBar() {
    // File menu
    auto *fileMenu = menuBar()->addMenu("&File");

    auto *openAction = fileMenu->addAction("&Open…");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFileDialog);

    m_saveAction = fileMenu->addAction("&Save");
    m_saveAction->setShortcut(QKeySequence::Save);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    fileMenu->addSeparator();

    auto *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Edit menu
    auto *editMenu = menuBar()->addMenu("&Edit");

    m_findAction = editMenu->addAction("&Find…");
    m_findAction->setShortcut(QKeySequence::Find);
    connect(m_findAction, &QAction::triggered, m_findPanel, &FindPanel::activate);
}

void MainWindow::openFileDialog() {
    if (!promptSaveIfModified()) return;

    QString path = QFileDialog::getOpenFileName(this, "Open File");
    if (!path.isEmpty())
        openFile(path);
}

bool MainWindow::promptSaveIfModified() {
    if (!m_buffer.isModified()) return true;

    auto choice = QMessageBox::question(
        this,
        "Unsaved changes",
        QString("You have unsaved changes to \"%1\".\nDo you want to save them?")
            .arg(m_currentPath.isEmpty() ? "untitled" : m_currentPath),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

    if (choice == QMessageBox::Save) {
        saveFile();
        return !m_buffer.isModified(); // false if save itself failed
    }
    if (choice == QMessageBox::Discard) return true;
    return false; // Cancel
}

void MainWindow::openFile(const QString &path) {
    auto err = m_fileManager.loadFile(path, m_buffer);

    if (err != FileManager::Error::None) {
        QMessageBox::critical(
            this, "Error opening file",
            QString("Could not open \"%1\":\n%2")
                .arg(path, FileManager::errorMessage(err)));
        return;
    }

    m_currentPath = path;
    m_model.reload();
    m_tableView->scrollToTop();
    refreshWindowElements();
}

void MainWindow::saveFile() {
    if (m_currentPath.isEmpty()) return;

    auto err = m_fileManager.saveFile(m_currentPath, m_buffer);

    if (err != FileManager::Error::None) {
        QMessageBox::critical(
            this, "Error saving file",
            QString("Could not save \"%1\":\n%2")
                .arg(m_currentPath, FileManager::errorMessage(err)));
        return;
    }

    // Refresh to clear modified highlights
    m_model.reload();
    refreshWindowElements();
}

void MainWindow::setupFindPanel() {
    m_findPanel = new FindPanel(m_buffer, m_model, this);
    m_findPanel->hide();

    auto *dock = new QDockWidget(this);
    dock->setWidget(m_findPanel);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock->setTitleBarWidget(new QWidget()); // hide title bar
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    connect(m_findPanel, &FindPanel::requestScrollToRow, this, [this](int row) {
        m_tableView->scrollTo(m_model.index(row, 0));
    });
}

void MainWindow::refreshWindowElements() {
    bool fileLoaded = !m_currentPath.isEmpty();
    m_saveAction->setEnabled(fileLoaded);
    m_findAction->setEnabled(fileLoaded);

    if (!fileLoaded) {
        setWindowTitle("Hex Editor");
        return;
    }

    QString name = QFileInfo(m_currentPath).fileName();
    setWindowTitle(QString("%1%2 — Hex Editor")
                       .arg(name)
                       .arg(m_buffer.isModified() ? "*" : ""));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (promptSaveIfModified())
        event->accept();
    else
        event->ignore();
}