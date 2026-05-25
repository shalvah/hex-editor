#pragma once
#include <QMainWindow>
#include "bytebuffer.h"
#include "editormodel.h"

class QTableView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupTable();

    ByteBuffer  m_buffer;
    EditorModel m_model;
    QTableView *m_tableView = nullptr;
};