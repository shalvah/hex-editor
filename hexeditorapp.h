#pragma once
#include <QObject>
#include "mainwindow.h"

/**
 * @brief The HexEditorApp class is a custpm application wrapper, serving as the main entry point.
 * It initializes the main window, passing in any user arguments.
 */
class HexEditorApp : public QObject {
    Q_OBJECT

public:
    explicit HexEditorApp(QObject *parent = nullptr);
    ~HexEditorApp() override;

    void initialize(int argc, char *argv[]);
    MainWindow* mainWindow() const;

private:
    MainWindow* m_mainWindow = nullptr;
};