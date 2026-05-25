#pragma once
#include <QObject>
#include "mainwindow.h"

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