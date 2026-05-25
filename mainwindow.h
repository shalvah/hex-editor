#pragma once
#include <QMainWindow>
#include "bytebuffer.h"
#include "editormodel.h"
#include "editorview.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupTable();

    ByteBuffer  m_buffer;
    EditorModel m_model;
    EditorView *m_tableView = nullptr;
};